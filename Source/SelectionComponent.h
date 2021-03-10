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

//grain lenght in seconds
#define GRAINLENGTH 0.1    

//state for the transport source 
enum class SoundState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

class SelectionComponent : public juce::Component, public juce::ChangeBroadcaster 
{
public:
    SelectionComponent(juce::AudioTransportSource& transportSourceRef, juce::File& sampleDir);

    void resized() override;
    void grainSelection(double start, double end, juce::Rectangle<int> bounds);
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void selectionButtonClicked();
    void saveWav(float startTime);
    SoundState getState();
    void setButtonsEnable(bool enablePlay, bool enableStop, bool enableSelect);
    
    void paint(juce::Graphics& g) override;

private:
    
    SoundState state;
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

    //top-buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;

    WaveformComponent thumbnailComp;
    PositionOverlayComponent positionComp;
    GrainSelector displayGrain;

    //sample dir ref
    juce::File& sampleDir;
};

