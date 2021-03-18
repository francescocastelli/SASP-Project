/*
  ==============================================================================

    InputSceneComponent.h
    Created: 28 Aug 2020 10:55:51am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveformComponent.h"
#include "PositionOverlayComponent.h"
#include "GrainSelector.h"
#include "SpectrogramComponent.h"
#include "GrainProcessingComponent.h"
#include "AppConstants.h"

//state for the transport source 
enum class SoundState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//state for the selection component
enum class SelectionState 
{
    Disabled,
    Loaded,
    Playing, 
    SelectGrain
};

class SelectionComponent : public juce::Component, public juce::ChangeBroadcaster, public juce::ActionBroadcaster
{
public:
    SelectionComponent(juce::AudioTransportSource& transportSourceRef, juce::File& sampleDir);

    SoundState getSoundState();

    void automaticGrainSelection();

    void saveGrain(juce::AudioBuffer<float> buf);
     
    void changeCurrentState(SelectionState newState);

    //----------------------------------------------------------------------------------
    void openButtonClicked();

    void playButtonClicked();

    void stopButtonClicked();

    void selectionButtonClicked();

    void saveButtonClicked();

    void setButtonsEnable(bool enableOpen, bool enablePlay, bool enableStop, bool enableSelect, bool enableSave);
     
    void setSliderEnable(bool enableWlenght, bool enableFade, bool enableWselection, bool enableAuto);

    //-------------------------------------------------------------------------------------
    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    
    //state for the selection component
    SelectionState currentState;

    SoundState state;

    //sample rate of the current opened audio file
    double sampleRate;

    //manager for the different formats
    juce::AudioFormatManager formatManager;    
    //reader for loading audio files
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache;                 
    juce::AudioThumbnail thumbnail;
    //reference to the transport source 
    //the actual object is in the main
    juce::AudioTransportSource& transportSource;

    int automaticSelectionVal;

    //buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;
    juce::TextButton saveButton;
    juce::TextButton automaticSaveButton;

    //sliders
    juce::Slider grainLenghtSlider;
    juce::Slider fadeSlider;
    juce::Slider automaticSelectionSlider;

    //grain processing buttons
    juce::ComboBox windowsMenu;

    //where we perform windowing of the grain
    GrainProcessingComponent grainProcessing;
    //display grain
    GrainSelector displayGrain;
    float currentGrainLenght;

    WaveformComponent thumbnailComp;
    PositionOverlayComponent positionComp;

    //spectrogram computation and display component
    SpectrogramComponent specComp;

    //sample dir ref
    juce::File& sampleDir;
};

