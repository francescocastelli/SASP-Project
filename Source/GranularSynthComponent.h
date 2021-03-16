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

    //density slider
    juce::Slider densitySlider;
    
    //window lenght slider
    juce::Slider windowLenghtSlider;

    float outputGain;

    //test
    juce::Array<Grain*> grainStack;
    //currently selected grain that can be used in the window
    juce::Array<Grain*> selectedGrain;

    int index;
    int currentGrainIndex;
    int nextGrainIndex;

    //density of grains -> number of grains in the current window
	int densityValue;
    //time index along the window
    long timeIndex;
    //window lenght in samples
    int windowLenght;

    int sampleRate;

    juce::Random rand;


    //flag
    bool audioIsPlaying;
    int skipGrain;

    SpectrogramComponent spectrogram;


    //private method
    void setButtonState(bool enableStart, bool enableStop, bool enableLoad);

    void grainSelectionAndPositioning();

    void readGrains();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

