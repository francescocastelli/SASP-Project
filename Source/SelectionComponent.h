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
#include "KnobLookAndFeel.h"

//state for the selection component
enum class SelectionState 
{
    Disabled,
    Loaded,
    Stopped,
    Playing, 
    SelectGrain
};

class SelectionComponent : public juce::Component, public juce::ChangeListener, public juce::ChangeBroadcaster
{
public:
    SelectionComponent(juce::File& sampleDir);

    SelectionState getCurrentState();

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);

    void releaseResources();

    //-------------------------------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    
    KnobLookAndFeel knobLookAndFeel;

    //state for the selection component
    SelectionState currentState;

    //sample rate of the current opened audio file
    double sampleRate;

    //current grain lenght selected with the slider in sec
    float currentGrainLenght;

    //current fade value in sec
    float currentFadeValue;

    //manager for the different formats
    juce::AudioFormatManager formatManager;    
    //reader for loading audio files
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache;                 
    //audio thubnail to keep the opened file
    juce::AudioThumbnail thumbnail;

    //tranport source used to play the input audio
    juce::AudioTransportSource transportSource;

    //number of grain for automatic selection
    int automaticSelectionVal;

    //show left or right channel 
    bool showLeft;

    //buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;
    juce::TextButton saveButton;
    juce::TextButton automaticSaveButton;
    juce::TextButton grainLeft;
    juce::TextButton grainRight;

    //text labels
    juce::Label waveformLabel;
    juce::Label grainLabel;
    juce::Label windowLabel;
    juce::Label fadeLabel;

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


    WaveformComponent thumbnailComp;
    PositionOverlayComponent positionComp;

    //sample dir ref
    juce::File& sampleDir;

    //-------------------------------- private methods -------------------------------
    void changeCurrentState(SelectionState newState);

    void setButtonsEnable(bool enableOpen, bool enablePlay, bool enableStop, bool enableSelect, bool enableSave);
     
    void setSliderEnable(bool enableWlenght, bool enableFade, bool enableWselection, bool enableAuto);

    void openButtonClicked();

    void selectionButtonClicked();

    void automaticGrainSelection();

    void grainLenghtChanged();
};

