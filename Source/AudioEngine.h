/*
  ==============================================================================

    AudioEngine.h
    Created: 20 Mar 2021 1:08:58pm
    Author:  Francesco

  ==============================================================================

	Audio engine, contains the method for processing the audio blocks and write audio
	on the output buffer. Based on the audio state of the model it knows if we are 
	playing audio from the input wav file or if we are synthetizing audio with the 
	granular synth. If we are synthesing audio it will also apply filtering and reverb.

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine);
};

