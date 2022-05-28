/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;
using namespace std;

//==============================================================================
DistortionVSTPluginAudioProcessor::DistortionVSTPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "Parameter", createParameters())
#endif
{
}

DistortionVSTPluginAudioProcessor::~DistortionVSTPluginAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionVSTPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionVSTPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionVSTPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionVSTPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionVSTPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionVSTPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionVSTPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionVSTPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionVSTPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionVSTPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionVSTPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto gain = parameters.getRawParameterValue("inGain");
    startGain = gain->load();
}

void DistortionVSTPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionVSTPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo())
    {
        if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
            return true;
        else
            return false;
    }
    else
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
    return true;
  #endif
}
#endif

void DistortionVSTPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Gain control of input signal
    inputGainControl(buffer);

    //MAIN DSP TAKES PLACE HERE
    //First obtain channel data
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        distortions(buffer);
    }

    //Gain control of output signal
    outputGainControl(buffer);

    // In case we have more outputs than inputs, this clears any output channels that didn't contain input data.
    // This is here to avoid people getting screaming feedback but  you don't need to keep this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

//==============================================================================
bool DistortionVSTPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionVSTPluginAudioProcessor::createEditor()
{
    return new DistortionVSTPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionVSTPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DistortionVSTPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessorValueTreeState::ParameterLayout DistortionVSTPluginAudioProcessor::createParameters()
{
    vector<unique_ptr<RangedAudioParameter>> parameterVector;

    parameterVector.push_back(make_unique<AudioParameterFloat>("inGain",            "Input Gain",   0.0f, 2.0f, 1.0f));

    parameterVector.push_back(make_unique<AudioParameterFloat>("distort1",          "Distortion 1", 0.0f, 1.0f, 0.5f));

    parameterVector.push_back(make_unique<AudioParameterFloat>("h_s_clipSlider",    "Hard Clip",    0.1f, 1.0f, 0.5f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("mix",               "Mix",          0.0f, 1.0f, 0.5f));

    parameterVector.push_back(make_unique<AudioParameterFloat>("distort2",          "Distortion 2", 0.0f, 1.0f, 0.5f));

    parameterVector.push_back(make_unique<AudioParameterFloat>("outGain",           "Output Gain",  0.0f, 2.0f, 1.0f));

    return { parameterVector.begin(), parameterVector.end() };
}

void DistortionVSTPluginAudioProcessor::inputGainControl(AudioBuffer<float>& buffer)
{
    auto gain = parameters.getRawParameterValue("inGain");
    float gainValue = gain->load();
    if (gainValue == startGain)
    {
        buffer.applyGain(gainValue);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), startGain, gainValue);
        startGain = gainValue;
    }
}

void DistortionVSTPluginAudioProcessor::distortions(AudioBuffer<float>& buffer)
{
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float out, an, bn, cn;

            auto dist1 = parameters.getRawParameterValue("distort1");
            float dist1Val = dist1->load();

            auto dist2 = parameters.getRawParameterValue("distort2");
            float dist2Val = dist2->load();

            auto mix = parameters.getRawParameterValue("mix");
            float mixVal = mix->load();

            auto hclip = parameters.getRawParameterValue("h_s_clipSlider");
            float hclipVal = hclip->load();

            const float inSample = buffer.getSample(channel, sample);
            an = distortionByJW(inSample, dist1Val);
            bn = h_s_clips(an, dist2Val, 's');
            cn = h_s_clips(inSample, hclipVal, 'h');
            out = an + bn + (mixVal * cn);

            buffer.setSample(channel, sample, out);
        }
    }
}

float DistortionVSTPluginAudioProcessor::distortionByJW(float sample, float alpha)
{
    const float z = (22/7) * alpha;
    const float s = 1.f / sin(z);
    const float b = 1.f / alpha;

    if (sample > b) 
        return 1.f;
    else
        return sin(z * sample) * s;
}

float DistortionVSTPluginAudioProcessor::h_s_clips(const float& sample, float thresh, char clip)
{
    if (clip == 's')
    {
        if (sample < -1.f) {
            return -thresh;
        }
        else if (sample > 1.f) {
            return thresh;
        }
        else {
            return sample - ((sample * sample * sample) / 3.f);
        }
    }
    else if (clip == 'h')
    {
        if (sample < -thresh)
            return -thresh;
        else if (sample > thresh)
            return thresh;
        else
            return sample;
    }
}

void DistortionVSTPluginAudioProcessor::outputGainControl(AudioBuffer<float>& buffer)
{
    auto gain = parameters.getRawParameterValue("outGain");
    float gainValue = gain->load();
    if (gainValue == finalGain)
    {
        buffer.applyGain(gainValue);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), finalGain, gainValue);
        finalGain = gainValue;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionVSTPluginAudioProcessor();
}
