#pragma once

#include <JuceHeader.h>

class PluginProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;  // Previously missing
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    // Effects
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    class Delay
    {
    public:
        Delay() : delayBuffer(2, 192000) {}

        void prepare(double sampleRate)
        {
            currentSampleRate = sampleRate;
            delayBuffer.clear();
            writePosition = 0;

            // Initialize filter coefficients
            float frequency = 1000.0f;  // Default cutoff frequency
            float q = 0.707f;           // Default Q factor
            lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, frequency, q));
        }

        void process(float* left, float* right, int numSamples)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                const float leftIn = left[i];
                const float rightIn = right[i];

                int delayInSamples = static_cast<int>(delayTime * currentSampleRate);
                int readPosition = writePosition - delayInSamples;
                if (readPosition < 0) readPosition += delayBuffer.getNumSamples();

                // Get delayed samples
                float delayedLeft = delayBuffer.getSample(0, readPosition);
                float delayedRight = delayBuffer.getSample(1, readPosition);

                // Apply damping (lowpass filter)
                delayedLeft = lowPassFilter.processSingleSampleRaw(delayedLeft);
                delayedRight = lowPassFilter.processSingleSampleRaw(delayedRight);

                // Write to delay buffer with feedback
                delayBuffer.setSample(0, writePosition, leftIn + delayedLeft * feedback);
                delayBuffer.setSample(1, writePosition, rightIn + delayedRight * feedback);

                // Mix dry and wet
                left[i] = leftIn * (1.0f - mix) + delayedLeft * mix;
                right[i] = rightIn * (1.0f - mix) + delayedRight * mix;

                writePosition++;
                if (writePosition >= delayBuffer.getNumSamples())
                    writePosition = 0;
            }
        }

        void setParameters(float newDelayTime, float newFeedback, float newMix, float newDamping)
        {
            delayTime = newDelayTime;
            feedback = newFeedback;
            mix = newMix;

            // Update filter cutoff based on damping
            float frequency = juce::jmap(newDamping, 0.0f, 1.0f, 20000.0f, 1000.0f);
            lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(currentSampleRate, frequency, 0.707f));
        }

    private:
        juce::AudioBuffer<float> delayBuffer;
        juce::IIRFilter lowPassFilter;
        double currentSampleRate = 44100.0;
        int writePosition = 0;
        float delayTime = 0.5f;   // seconds
        float feedback = 0.5f;    // 0 to 1
        float mix = 0.5f;         // 0 to 1
    };

    Delay delay;
    bool reverbEnabled = true;
    bool delayEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};