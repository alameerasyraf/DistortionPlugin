/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;
using namespace std;

//==============================================================================
DistortionVSTPluginAudioProcessorEditor::DistortionVSTPluginAudioProcessorEditor (DistortionVSTPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the editor's size to whatever you need it to be.
    buildElements();
    setSize(1200, 700);
}

DistortionVSTPluginAudioProcessorEditor::~DistortionVSTPluginAudioProcessorEditor()
{
}

//==============================================================================
void DistortionVSTPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    //Insert the background image
    Image background = ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize);
    g.drawImageAt(background, 0, 0);

    //Draw the semi-transparent rectangle around components
    const Rectangle<float> area(20, 80, 1160, 540);
    g.setColour(Colours::ghostwhite);
    g.drawRoundedRectangle(area, 5.0f, 3.0f);

    //Draw background for rectangle
    g.setColour(Colours::black);
    g.setOpacity(0.7f);
    g.fillRoundedRectangle(area, 5.0f);

    //Draw text labels for each component
    g.setColour(Colours::white);
    g.setFont(18.0f);
    g.drawText("Input Gain",    (getWidth() / 3) - 400,     (getHeight() / 2) + 205, 200, 50, Justification::centred, false);

    g.drawText("Non-Linear",    (getWidth() / 3) - 75,      (getHeight() / 2) + 90, 200, 50, Justification::centred, false);
    g.drawText("Distortion",    (getWidth() / 3) - 75,      (getHeight() / 2) + 110, 200, 50, Justification::centred, false);

    g.drawText("Hard Clip",     (getWidth() / 2) - 207,     (getHeight() / 2) + 15, 200, 50, Justification::centred, false);
    g.drawText("Mix",           (getWidth() / 2) + 17,      (getHeight() / 2) + 15, 200, 50, Justification::centred, false);

    g.drawText("Soft Clip",     (getWidth() / 3) + 270,     (getHeight() / 2) + 100, 200, 50, Justification::centred, false);

    g.drawText("Output Gain",   getWidth() - 198,           (getHeight() / 2) + 205, 200, 50, Justification::centred, false);

    g.setFont(35.0f);
    g.drawText("Multi-Distorter", 20, 0, 1160, 75, Justification::centred, false);
}

void DistortionVSTPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any subcomponents in your editor.
    inputGainSlider.setBounds   ((getWidth() / 3) - 400,    (getHeight() / 2) - 135, 200, 350);

    distortion1Slider.setBounds ((getWidth() / 3) - 200,    (getHeight() / 2) - 10, 185, 225);

    h_s_clipSlider.setBounds    ((getWidth() / 2) - 200,    (getHeight() / 2) - 200, 185, 225);
    mixSlider.setBounds         ((getWidth() / 2) + 25,     (getHeight() / 2) - 200, 185, 225);

    distortion2Slider.setBounds (getWidth() - 400,          (getHeight() / 2) - 10, 185, 225);

    outputGainSlider.setBounds  (getWidth() - 200,          (getHeight() / 2) - 135, 200, 350);
}

void DistortionVSTPluginAudioProcessorEditor::buildElements()
{
    //Building the Input Gain
    inputGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain", inputGainSlider);
    inputGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    inputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    inputGainSlider.setRange(0.0f, 2.0f);
    addAndMakeVisible(&inputGainSlider);

    //Building the Distortion 1
    distortion1Val = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "distort1", distortion1Slider);
    distortion1Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    distortion1Slider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    distortion1Slider.setRange(0.0f, 1.0f);
    addAndMakeVisible(&distortion1Slider);

    //Building the Hard Clip
    h_s_clipVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "h_s_clipSlider", h_s_clipSlider);
    h_s_clipSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    h_s_clipSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    h_s_clipSlider.setRange(0.0f, 1.0f);
    addAndMakeVisible(&h_s_clipSlider);
    
    //Building the Mix
    mixVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "mix", mixSlider);
    mixSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    mixSlider.setRange(0.0f, 1.0f);
    addAndMakeVisible(&mixSlider);

    //Building the Distortion 2
    distortion2Val = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "distort2", distortion2Slider);
    distortion2Slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    distortion2Slider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    distortion2Slider.setRange(0.0f, 1.0f);
    addAndMakeVisible(&distortion2Slider);

    //Building the Output Gain
    outputGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain", outputGainSlider);
    outputGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    outputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    outputGainSlider.setRange(0.0f, 2.0f);
    addAndMakeVisible(&outputGainSlider);
}
