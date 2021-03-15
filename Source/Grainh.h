/*
  ==============================================================================

    Grainh.h
    Created: 14 Mar 2021 12:23:36am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Grain 
{
public:

    Grain();

    ~Grain();

    Grain(juce::AudioBuffer<float>& buf);

    juce::AudioBuffer<float>& getBufferPointer();

    int getCurrentIndex();

    int getTotSamples();

    int getSamplesLeft();

    void incIndex(int numSamples);

    void processBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void prepareToPlay(double sampleRate, double rampLenghtInSecond);

    bool isPlaying();

private:

    juce::AudioBuffer<float> dataBuffer;

    juce::SmoothedValue<float> smoother;

    int dataBufferIndex;
};
