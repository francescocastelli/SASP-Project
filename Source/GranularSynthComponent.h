/*
  ==============================================================================

    GranularSynthComponent.h
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <set>
#include "AppConstants.h"
#include "Grainh.h"
#include "SpectrogramComponent.h"
#include "TimeFrequencyVisualizer.h"
#include "KnobLookAndFeel.h"

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

    KnobLookAndFeel knobLookAndFeel;

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
    //filter sliders
    juce::Slider cutOffFreqSlider;
    juce::Slider qFactorSlider;
    juce::Slider filterGainSlider;
    juce::Slider filterTypeSlider;

    //labels
    juce::Label grainLabel;
    juce::Label filterLabel;
    juce::Label densityLabel;
	juce::Label positionLabel;
	juce::Label lengthLabel;
    juce::Label cutoffLabel;
    juce::Label gainLabel;
    juce::Label qFactorLabel;
    juce::Label filterTypeLabel;

    //Grain parameters
    //stack of grains
    std::deque<Grain> grainStack;
    //time index 
    long long timeIndex;

    std::mutex mtx;
 
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

    //filtering
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
    float cutOffFreq;
    float qFactor;
    float filterGain;
    int filterType;
    bool noFiltering;

    //flags
    bool audioIsPlaying;
    bool grainLoaded;

    //visualization components
    SpectrogramComponent spectrogram;
    GrainVisualizer grainVisualizer;

    //---------------------- private method -------------------------------

    void updateFilter();

    void setButtonState(bool enableStart, bool enableStop, bool enableLoad, bool enableRandom);

    void setSliderState(bool granulationSlider);

    void setFilterSliderState(bool noFilter, bool filterParam);

    void readGrains();

    void slidersChanged();

    void changeCurrentState(SynthState newState);

    void selectNextGrain();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

