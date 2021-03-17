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

//state for the transport source 
enum class SynthState 
{
    Stopped,
    Playing,
    Loaded,
    Disable
};

class GranularSynthComponent: public juce::Component, public juce::AudioSource, public juce::ChangeBroadcaster, public juce::Thread
{
public:
    GranularSynthComponent(juce::File& sampleDir);

    //-------------------------------------------------------------

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;

    void run() override;

    //-------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void resized() override;

private:

    //synth state 
    SynthState currentState;

    //directory of the grains and format manager for reading
    juce::File& sampleDir;
    juce::AudioFormatManager formatManager;
    juce::Array<juce::File> grainFileStack;

    //buttons
    juce::TextButton loadGrain;
    juce::TextButton stopAudio;
    juce::TextButton playAudio;
    juce::ToggleButton randomSelectionButton;

    //sliders
    juce::Slider masterVolume;
    juce::Slider densitySlider;
    juce::Slider windowLenghtSlider;
    juce::Slider windowPositionSlider;

    //Grain parameters
    //stack of grains
    std::deque<Grain> grainStack;
    //time index 
    long long timeIndex;
 
    //density of grains -> number of grains in the current window
	int densityValue;
    //window lenght in samples
    int windowLenght;
    //window startint position in samples
    int windowPosition;
    //current index for linear selection of grains
    int currentGrainIndex;

    //grain position parameters
    long long nextGrainStart;
    int randomPosOffset;

    //audio processing parameters
    juce::Reverb reverb;
    float outputGain;
    int sampleRate;

    //flags
    bool audioIsPlaying;
    bool grainLoaded;

    //visualization components
    SpectrogramComponent spectrogram;

    //---------------------- private method -------------------------------
    void setButtonState(bool enableStart, bool enableStop, bool enableLoad, bool enableRandom);

    void setSliderState(bool enableWindowPos, bool enableWindowL);

    void readGrains();

    void slidersChanged();

    void changeCurrentState(SynthState newState);

    int selectNextGrain();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

