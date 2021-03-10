#pragma once
#include <JuceHeader.h>
#include "SelectionComponent.h"


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

    //state for the input audio file
    SoundState inputAudioState;         
    //top part of the window, contains the waveform and the grain selection (also buttons)
    SelectionComponent waveComponent;
    //transport source used to load and play audio 
    juce::AudioTransportSource transportSource;

    //directory where to store the samples
    juce::File sampleDir;

    //==============================================================================

    void changeState(SoundState newState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
