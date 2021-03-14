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

    Grain(juce::AudioBuffer<float>& buf);

    juce::AudioBuffer<float>& getBufferPointer();

private:

    juce::AudioBuffer<float> dataBuffer;
};
