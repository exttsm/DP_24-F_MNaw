#pragma once

#include "PluginProcessor.h"

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void showReverbControls();
    void showDelayControls();

    PluginProcessor& processorRef;

    // Effect selection
    juce::ComboBox effectSelector;

    // Effect enable buttons
    juce::ToggleButton reverbEnableButton;
    juce::ToggleButton delayEnableButton;

    // Reverb controls
    juce::Component reverbControls;
    juce::Slider roomSizeSlider;
    juce::Slider dampingSlider;
    juce::Slider wetLevelSlider;
    juce::Slider dryLevelSlider;
    juce::Slider widthSlider;
    juce::Slider freezeModeSlider;

    juce::Label roomSizeLabel;
    juce::Label dampingLabel;
    juce::Label wetLabel;
    juce::Label dryLabel;
    juce::Label widthLabel;
    juce::Label freezeLabel;

    // Delay controls
    juce::Component delayControls;
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider delayMixSlider;

    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label delayMixLabel;

    juce::Slider delayDampingSlider;
    juce::Label delayDampingLabel;
    juce::ComboBox delaySyncBox;
    juce::Label delaySyncLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freezeModeAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayMixAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverbEnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayEnableAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> delaySyncAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};