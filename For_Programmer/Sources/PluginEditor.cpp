#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    // Set up effect selector
    effectSelector.addItem("Reverb", 1);
    effectSelector.addItem("Delay", 2);
    effectSelector.setSelectedId(1);
    effectSelector.onChange = [this] {
        if (effectSelector.getSelectedId() == 1)
            showReverbControls();
        else
            showDelayControls();
        };
    addAndMakeVisible(effectSelector);

    // Set up enable buttons
    reverbEnableButton.setButtonText("Reverb Enable");
    delayEnableButton.setButtonText("Delay Enable");
    addAndMakeVisible(reverbEnableButton);
    addAndMakeVisible(delayEnableButton);

    // Setup Reverb Controls
    addChildComponent(reverbControls);

    roomSizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(roomSizeSlider);
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.attachToComponent(&roomSizeSlider, true);
    reverbControls.addAndMakeVisible(roomSizeLabel);

    dampingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(dampingSlider);
    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.attachToComponent(&dampingSlider, true);
    reverbControls.addAndMakeVisible(dampingLabel);

    wetLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    wetLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(wetLevelSlider);
    wetLabel.setText("Wet Level", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetLevelSlider, true);
    reverbControls.addAndMakeVisible(wetLabel);

    dryLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    dryLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(dryLevelSlider);
    dryLabel.setText("Dry Level", juce::dontSendNotification);
    dryLabel.attachToComponent(&dryLevelSlider, true);
    reverbControls.addAndMakeVisible(dryLabel);

    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(widthSlider);
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.attachToComponent(&widthSlider, true);
    reverbControls.addAndMakeVisible(widthLabel);

    freezeModeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    freezeModeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    reverbControls.addAndMakeVisible(freezeModeSlider);
    freezeLabel.setText("Freeze", juce::dontSendNotification);
    freezeLabel.attachToComponent(&freezeModeSlider, true);
    reverbControls.addAndMakeVisible(freezeLabel);

    // Setup Delay Controls
    addChildComponent(delayControls);

    delayTimeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    delayControls.addAndMakeVisible(delayTimeSlider);
    delayTimeLabel.setText("Delay Time", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, true);
    delayControls.addAndMakeVisible(delayTimeLabel);

    feedbackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    delayControls.addAndMakeVisible(feedbackSlider);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, true);
    delayControls.addAndMakeVisible(feedbackLabel);

    delayMixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    delayMixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    delayControls.addAndMakeVisible(delayMixSlider);
    delayMixLabel.setText("Mix", juce::dontSendNotification);
    delayMixLabel.attachToComponent(&delayMixSlider, true);
    delayControls.addAndMakeVisible(delayMixLabel);

    delayDampingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    delayDampingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
    delayControls.addAndMakeVisible(delayDampingSlider);
    delayDampingLabel.setText("Damping", juce::dontSendNotification);
    delayDampingLabel.attachToComponent(&delayDampingSlider, true);
    delayControls.addAndMakeVisible(delayDampingLabel);

    delaySyncBox.addItemList(juce::StringArray("Free", "1/1", "1/2", "1/4", "1/8", "1/16"), 1);
    delayControls.addAndMakeVisible(delaySyncBox);
    delaySyncLabel.setText("Sync", juce::dontSendNotification);
    delaySyncLabel.attachToComponent(&delaySyncBox, true);
    delayControls.addAndMakeVisible(delaySyncLabel);

    // Create parameter attachments
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "roomSize", roomSizeSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "damping", dampingSlider);
    wetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "wetLevel", wetLevelSlider);
    dryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "dryLevel", dryLevelSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "width", widthSlider);
    freezeModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "freezeMode", freezeModeSlider);

    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "delayTime", delayTimeSlider);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "delayFeedback", feedbackSlider);
    delayMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "delayMix", delayMixSlider);

    reverbEnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.parameters, "reverbEnabled", reverbEnableButton);
    delayEnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.parameters, "delayEnabled", delayEnableButton);

    delayDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "delayDamping", delayDampingSlider);
    delaySyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.parameters, "delaySync", delaySyncBox);

    // Show initial view
    showReverbControls();

    // Set the plugin's window size
    setSize(600, 500);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::showReverbControls()
{
    reverbControls.setVisible(true);
    delayControls.setVisible(false);
    resized(); // Trigger a layout update
}

void PluginEditor::showDelayControls()
{
    reverbControls.setVisible(false);
    delayControls.setVisible(true);
    resized(); // Trigger a layout update
}

void PluginEditor::resized()
{
    auto area = getLocalBounds();
    const int topSectionHeight = 60;
    const int labelWidth = 100;
    const int sliderHeight = 40;
    const int margin = 10;

    // Top section with effect selector and enable buttons
    auto topSection = area.removeFromTop(topSectionHeight);
    effectSelector.setBounds(topSection.removeFromLeft(200).reduced(margin));

    auto enableButtonArea = topSection.reduced(margin);
    reverbEnableButton.setBounds(enableButtonArea.removeFromLeft(enableButtonArea.getWidth() / 2));
    delayEnableButton.setBounds(enableButtonArea);

    // Main controls area
    area.removeFromTop(margin);

    // Setup reverb controls bounds
    auto reverbArea = area;
    roomSizeSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbArea.removeFromTop(margin);
    dampingSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbArea.removeFromTop(margin);
    wetLevelSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbArea.removeFromTop(margin);
    dryLevelSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbArea.removeFromTop(margin);
    widthSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbArea.removeFromTop(margin);
    freezeModeSlider.setBounds(reverbArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    reverbControls.setBounds(area);

    // Setup delay controls bounds
    auto delayArea = area;
    delayTimeSlider.setBounds(delayArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    delayArea.removeFromTop(margin);
    feedbackSlider.setBounds(delayArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    delayArea.removeFromTop(margin);
    delayMixSlider.setBounds(delayArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    delayControls.setBounds(area);
    delayDampingSlider.setBounds(delayArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    delayArea.removeFromTop(margin);
    delaySyncBox.setBounds(delayArea.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
}