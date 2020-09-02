#pragma once

#include <JuceHeader.h>
#include "WaveFormComponent.h"


#define GRAINLENGTH 0.1    //grain length in seconds

//==============================================================================

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener
{
public:


    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    enum SoundState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    //==============================================================================
    
    SoundState inputAudioState;         //state for the input audio file
    juce::AudioFormatManager formatManager;    //manager for the different formats
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    double sampleRate;
    juce::AudioBuffer<float> grainBuffer;
    juce::AudioThumbnail thumb;

    //buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;

    //drawing
    
    juce::AudioThumbnailCache thumbnailCache;                 
    WaveFormComponent inputWaveform;
    //==============================================================================

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void selectionButtonClicked();
    void changeState(SoundState newState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
