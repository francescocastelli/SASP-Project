#pragma once
#include <JuceHeader.h>
#include "SelectionComponent.h"
#include "GranularSynthComponent.h"

//==============================================================================

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener
{
public:


    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==============================================================================
    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    //top part of the window, contains the waveform and the grain selection (also buttons)
    SelectionComponent selectionComponent;
    //bottom half of the window, contains the granular synth
    GranularSynthComponent granularSynth;

    //directory where to store the samples
    juce::File sampleDir;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
