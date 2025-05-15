#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "Parameters", {
    // Reverb parameters
    std::make_unique<juce::AudioParameterFloat>("roomSize", "Room Size", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f),
    std::make_unique<juce::AudioParameterFloat>("dryLevel", "Dry Level", 0.0f, 1.0f, 0.4f),
    std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("freezeMode", "Freeze Mode", 0.0f, 1.0f, 0.0f),
    // Delay parameters
    std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 0.0f, 2.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Feedback", 0.0f, 0.95f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", 0.0f, 1.0f, 0.5f),
    // Effect enable parameters
    std::make_unique<juce::AudioParameterBool>("reverbEnabled", "Reverb Enable", true),
    std::make_unique<juce::AudioParameterBool>("delayEnabled", "Delay Enable", true),

    std::make_unique<juce::AudioParameterFloat>("delayDamping", "Delay Damping", 0.0f, 1.0f, 0.0f),
        // Add note length parameter for tempo sync
        std::make_unique<juce::AudioParameterChoice>("delaySync", "Delay Sync",
            juce::StringArray("Free", "1/1", "1/2", "1/4", "1/8", "1/16"), 0)
        })
{
    // Add listeners for all parameters
    parameters.addParameterListener("roomSize", this);
    parameters.addParameterListener("damping", this);
    parameters.addParameterListener("wetLevel", this);
    parameters.addParameterListener("dryLevel", this);
    parameters.addParameterListener("width", this);
    parameters.addParameterListener("freezeMode", this);
    parameters.addParameterListener("delayTime", this);
    parameters.addParameterListener("delayFeedback", this);
    parameters.addParameterListener("delayMix", this);
    parameters.addParameterListener("reverbEnabled", this);
    parameters.addParameterListener("delayEnabled", this);
    parameters.addParameterListener("delayDamping", this);
    parameters.addParameterListener("delaySync", this);

    // Initialize reverb parameters
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.4f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);
}

PluginProcessor::~PluginProcessor()
{
    parameters.removeParameterListener("roomSize", this);
    parameters.removeParameterListener("damping", this);
    parameters.removeParameterListener("wetLevel", this);
    parameters.removeParameterListener("dryLevel", this);
    parameters.removeParameterListener("width", this);
    parameters.removeParameterListener("freezeMode", this);
    parameters.removeParameterListener("delayTime", this);
    parameters.removeParameterListener("delayFeedback", this);
    parameters.removeParameterListener("delayMix", this);
    parameters.removeParameterListener("reverbEnabled", this);
    parameters.removeParameterListener("delayEnabled", this);
}

void PluginProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "roomSize")
        reverbParams.roomSize = newValue;
    else if (parameterID == "damping")
        reverbParams.damping = newValue;
    else if (parameterID == "wetLevel")
        reverbParams.wetLevel = newValue;
    else if (parameterID == "dryLevel")
        reverbParams.dryLevel = newValue;
    else if (parameterID == "width")
        reverbParams.width = newValue;
    else if (parameterID == "freezeMode")
        reverbParams.freezeMode = newValue;
    else if (parameterID == "delayTime" || parameterID == "delayFeedback" ||
        parameterID == "delayMix" || parameterID == "delayDamping" ||
        parameterID == "delaySync")
    {
        float delayTime = *parameters.getRawParameterValue("delayTime");
        float feedback = *parameters.getRawParameterValue("delayFeedback");
        float mix = *parameters.getRawParameterValue("delayMix");
        float damping = *parameters.getRawParameterValue("delayDamping");

        // Handle tempo sync
        int syncMode = static_cast<int>(*parameters.getRawParameterValue("delaySync"));
        if (syncMode > 0)
        {
            auto hostBpm = getPlayHead()->getPosition()->getBpm();
            if (hostBpm.hasValue())
            {
                // Calculate delay time based on tempo and note length
                double bpm = *hostBpm;
                double beatsPerSecond = bpm / 60.0;
                double wholeBeatLength = 1.0 / beatsPerSecond;

                switch (syncMode)
                {
                case 1: delayTime = wholeBeatLength; break;        // 1/1
                case 2: delayTime = wholeBeatLength * 0.5; break;  // 1/2
                case 3: delayTime = wholeBeatLength * 0.25; break; // 1/4
                case 4: delayTime = wholeBeatLength * 0.125; break;// 1/8
                case 5: delayTime = wholeBeatLength * 0.0625; break;// 1/16
                }
            }
        }

        delay.setParameters(delayTime, feedback, mix, damping);
    }
    else if (parameterID == "reverbEnabled")
        reverbEnabled = newValue >= 0.5f;
    else if (parameterID == "delayEnabled")
        delayEnabled = newValue >= 0.5f;

    if (parameterID.startsWith("reverb"))
        reverb.setParameters(reverbParams);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverb.setSampleRate(sampleRate);
    delay.prepare(sampleRate);
}

void PluginProcessor::releaseResources()
{
    // Empty release resources method
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Process delay if enabled
    if (delayEnabled && totalNumInputChannels == 2)
    {
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        delay.process(leftChannel, rightChannel, buffer.getNumSamples());
    }

    // Process reverb if enabled
    if (reverbEnabled)
    {
        if (totalNumInputChannels == 1)
        {
            float* channelData = buffer.getWritePointer(0);
            reverb.processMono(channelData, buffer.getNumSamples());
        }
        else
        {
            float* leftChannel = buffer.getWritePointer(0);
            float* rightChannel = buffer.getWritePointer(1);
            reverb.processStereo(leftChannel, rightChannel, buffer.getNumSamples());
        }
    }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() { return new PluginEditor(*this); }
bool PluginProcessor::hasEditor() const { return true; }
const juce::String PluginProcessor::getName() const { return JucePlugin_Name; }
bool PluginProcessor::acceptsMidi() const { return false; }
bool PluginProcessor::producesMidi() const { return false; }
bool PluginProcessor::isMidiEffect() const { return false; }
double PluginProcessor::getTailLengthSeconds() const { return 0.0; }
int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram(int index) {}
const juce::String PluginProcessor::getProgramName(int index) { return {}; }
void PluginProcessor::changeProgramName(int index, const juce::String& newName) {}
void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {}
void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}