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

    void processBlock(const juce::AudioSourceChannelInfo& bufferToFill, long currentTimeIndex);

    void setStartIndex(int startIndex);
    
    int getStartIndex();

    bool isActive();

    void activeGrain();

private:

    juce::AudioBuffer<float> dataBuffer;

    juce::SmoothedValue<float> smoother;

    int dataBufferIndex;

    int startIndex;

    bool active;

    //private methods
    int getSamplesLeft();
};
