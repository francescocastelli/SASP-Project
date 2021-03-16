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

class GranularSynthComponent: public juce::Component, public juce::AudioSource, public juce::ChangeBroadcaster
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

    //master volume slider
    juce::Slider masterVolume;

    float outputGain;

    //test
    juce::Array<Grain*> grainStack;
    int index;
    int currentGrainIndex;
    int nextGrainIndex;
    long time;
    juce::Random rand;

    //flag
    bool audioIsPlaying;
    int skipGrain;

    SpectrogramComponent spectrogram;


    //private method
    void setButtonState(bool enableStart, bool enableStop, bool enableLoad);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

