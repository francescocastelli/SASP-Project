/*
  ==============================================================================

    GranularSynthComponent.h
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================

	Controller for the bottom half of the window. It manages all the knobs and 
    buttons that are in this part of the app, so paints and resizes all the 
    components and also	react to events (like clicks and value changed) by 
    updating the state of the model that correspond to that particular button.

    Here we load grains from a folder and play them back using granular synth. 
    We can also apply some filters and play around with the granular synth by 
    changing grain number, overlapping, ecc...

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "KnobLookAndFeel.h"
#include "SpectrogramComponent.h"
#include "TimeFrequencyVisualizer.h"
#include "Model.h"
#include "GrainSelectorThread.h"
#include "AudioLoader.h"
#include "AudioEngine.h"

//state for the transport source 
enum class SynthState
{
    Stopped,
    Playing,
    Loaded,
    Disable
};

class GranularSynthComponent : public juce::Component
{
public:
    GranularSynthComponent(Model& model, GrainSelector& grainSelector, AudioLoader& audioLoader, AudioEngine& audioEngine);

    void paint(juce::Graphics& g) override;

    void resized() override;

private:

    Model& model;

    SynthState currentState;

    GrainSelector& grainSelector;

    AudioLoader& audioLoader;

    KnobLookAndFeel knobLookAndFeel;

    TimeFreqVisualizer grainsVisualizer;

    SpectrogramComponent fftVisualizer;

    //buttons
    juce::TextButton loadGrain;
    juce::TextButton stopAudio;
    juce::TextButton playAudio;
    juce::ToggleButton randomSelectionButton;
    juce::ToggleButton reverseButton;

    //sliders
    juce::Slider masterVolume;
    juce::Slider densitySlider;
    juce::Slider windowLenghtSlider;
    juce::Slider windowPositionSlider;
    juce::Slider randomPositionSlider;

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
    juce::Label randomPositionLabel;
    juce::Label randomSelectionLabel;
    juce::Label reverseLabel;

    //---------------------------- methods ------------------------------

    void changeCurrentState(SynthState newState);

    void setButtonState(bool enableStart, bool enableStop, bool enableLoad, bool enableRandom);

    void setSliderState(bool granulationSlider);

    void setFilterSliderState(bool noFilter, bool filterParam);

    void changeMasterVolume();

    void filterSliderChanged();

    void grainSliderChanged();

    void loadGrainClicked();

    void randomPositionSliderChanged();
    
    void randomSelectionButtonClicked();
    
    void windowSliderChanged();

    void densitySliderChanged();

    void reverseButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

