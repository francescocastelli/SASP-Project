/*
  ==============================================================================

    Grainh.h
    Created: 14 Mar 2021 12:23:36am
    Author:  Francesco

  ==============================================================================

	This represent the single grain, contains all the info about a grain and the 
    audio buffer containg the audio. Process block basically add the content of 
    the grain audio buffer to the buffer pass as parameter. Then we have also 
    methods for applying fade in/out, used for doing overlap and add.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Grain 
{
public:

    Grain();

    Grain(juce::AudioBuffer<float>& buf, int startTime, int id);

    Grain(const Grain&) = default;
    Grain(Grain&&) noexcept = default;

    Grain& operator=(const Grain&) = default ;
    Grain& operator=(Grain&& grain) noexcept = default;

    ~Grain() = default;

    void processBlock(const juce::AudioSourceChannelInfo& bufferToFill, int timeIndex);

    void fadeOut(int startSample);

    void fadeIn(int endSample);

    bool canPlay(int timeIndex) const;
    
    bool hasEnded(int timeIndex) const;

    int getStartIndex() const;

    int getId() const;

    bool isFinished();

private:

    juce::AudioBuffer<float> dataBuffer;

    int dataBufferIndex;

    int startIndex;

    int id;

    bool finished;
};
