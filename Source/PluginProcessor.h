/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;
using namespace std;

//==============================================================================
/**
*/
class DistortionVSTPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DistortionVSTPluginAudioProcessor();
    ~DistortionVSTPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState parameters;

private:

    float startGain;
    float finalGain;

    AudioProcessorValueTreeState::ParameterLayout createParameters();

    void DistortionVSTPluginAudioProcessor::inputGainControl(AudioBuffer<float>& buffer);

    void DistortionVSTPluginAudioProcessor::distortions(AudioBuffer<float>& buffer);
    float DistortionVSTPluginAudioProcessor::distortionByJW(const float sample, const float alpha);
    float DistortionVSTPluginAudioProcessor::h_s_clips(const float& sample, float thresh, char clip);

    void DistortionVSTPluginAudioProcessor::outputGainControl(AudioBuffer<float>& buffer);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionVSTPluginAudioProcessor)
};
