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

    Grain(juce::AudioBuffer<float>& buf, int startTime);

    ~Grain();

    void processBlock(const juce::AudioSourceChannelInfo& bufferToFill, int timeIndex);

    bool canPlay(int timeIndex);
    
    bool hasEnded(int timeIndex);

    int getStartIndex();

private:

    juce::AudioBuffer<float> dataBuffer;

    int dataBufferIndex;

    int startIndex;
};
