/*
  ==============================================================================

    GranularSynthComponent.h
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Grainh.h"
#include "SpectrogramComponent.h"

class GranularSynthComponent: public juce::Component, juce::AudioSource
{
public:
    GranularSynthComponent(juce::File& sampleDir);

    //-------------------------------------------------------------
    void readGrains();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;
    //-------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void resized() override;

private:

    juce::File& sampleDir;

    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader;

    //buttons
    juce::TextButton loadGrain;
    juce::TextButton stopAudio;
    juce::TextButton playAudio;

    //test
    juce::Array<Grain> grainStack;
    int index;
    int currentGrain;
    juce::Random rand;

    //flag
    bool audioIsPlaying;
    bool skipGrain;

    SpectrogramComponent spectrogram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

