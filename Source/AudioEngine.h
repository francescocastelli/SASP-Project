/*
  ==============================================================================

    AudioEngine.h
    Created: 20 Mar 2021 1:08:58pm
    Author:  Francesco

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "Grainh.h"
#include "Model.h"

class AudioEngine
{
public:
    AudioEngine(Model& model);

    //-------------------------------------------------------------

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);

    void processNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void releaseResources();

private:


    Model& model;

    //---------------------- private method -------------------------------

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine);
};

