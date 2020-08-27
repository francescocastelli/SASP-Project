#pragma once

#include <JuceHeader.h>


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
    

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    
    //==============================================================================

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void changeState(SoundState newState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
