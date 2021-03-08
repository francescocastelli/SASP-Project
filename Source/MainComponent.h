#pragma once

#include <JuceHeader.h>
#include "WaveFormComponent.h"


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
    
    SoundState inputAudioState;         //state for the input audio file
    WaveFormComponent waveComponent;
    juce::AudioTransportSource transportSource;

    //directory where to store the samples
    juce::File sampleDir;
    //==============================================================================

    void changeState(SoundState newState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
