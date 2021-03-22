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
#include "SpectrogramComponent.h"

class AudioEngine
{
public:
    AudioEngine(Model& model);

    //-------------------------------------------------------------

    void setfftVisualizer(SpectrogramComponent* fftVisualizer);

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);

    void processNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void releaseResources();

private:

    Model& model;

    SpectrogramComponent* fftVisualizer;

    //---------------------- private method -------------------------------

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine);
};

