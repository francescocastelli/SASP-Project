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

    Grain(juce::AudioBuffer<float>& buf, int startTime, int id);

    ~Grain();

    void processBlock(const juce::AudioSourceChannelInfo& bufferToFill, int timeIndex) const;

    void fadeOut(int startSample);

    void fadeIn(int endSample);

    bool canPlay(int timeIndex) const;
    
    bool hasEnded(int timeIndex) const;

    int getStartIndex() const;

    int getId() const;

private:

    juce::AudioBuffer<float> dataBuffer;

    int dataBufferIndex;

    int startIndex;

    int id;
};
