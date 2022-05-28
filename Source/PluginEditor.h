/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;
using namespace std;

//==============================================================================
/**
*/
class DistortionVSTPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistortionVSTPluginAudioProcessorEditor (DistortionVSTPluginAudioProcessor&);
    ~DistortionVSTPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buildElements();

    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputGainVal;

    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> distortion1Val;

    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> h_s_clipVal;
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mixVal;

    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> distortion2Val;

    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> outputGainVal;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistortionVSTPluginAudioProcessor& audioProcessor;

    Slider inputGainSlider;

    Slider distortion1Slider;

    Slider h_s_clipSlider;
    Slider mixSlider;

    Slider distortion2Slider;

    Slider outputGainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionVSTPluginAudioProcessorEditor)
};
