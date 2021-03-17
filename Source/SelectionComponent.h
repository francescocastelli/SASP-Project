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

class SelectionComponent : public juce::Component, public juce::ChangeBroadcaster, public juce::ActionBroadcaster
{
public:
    SelectionComponent(juce::AudioTransportSource& transportSourceRef, juce::File& sampleDir);

    SoundState getSoundState();

    void automaticGrainSelection();

    void saveGrain(juce::AudioBuffer<float> buf);

    //----------------------------------------------------------------------------------
    void openButtonClicked();

    void playButtonClicked();

    void stopButtonClicked();

    void selectionButtonClicked();

    void saveButtonClicked();

    void setButtonsEnable(bool enablePlay, bool enableStop, bool enableSelect);

    //-------------------------------------------------------------------------------------
    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    
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

