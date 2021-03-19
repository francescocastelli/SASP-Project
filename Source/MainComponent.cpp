#pragma once
#include "MainComponent.h"
#include "AppConstants.h"

//==============================================================================
MainComponent::MainComponent()
    :selectionComponent(sampleDir),
    granularSynth(sampleDir),
    //for now set the path as fixed, then the user should be able to set it
    //use the same approach as open file
    sampleDir("C:\\Users\\Francesco\\Desktop\\testSamples")
{

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels (0, 2);
    }

    addAndMakeVisible(selectionComponent);
    addAndMakeVisible(granularSynth);
    setAudioChannels(0, 2);

    setSize (1200, 600);

    //create the directory based on the filename
    sampleDir.createDirectory();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    selectionComponent.prepareToPlay(samplesPerBlockExpected, sampleRate);
    granularSynth.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    selectionComponent.getNextAudioBlock(bufferToFill);
    granularSynth.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    selectionComponent.releaseResources();
    granularSynth.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //background of the entire window
    g.fillAll(AppColours::background);
}

void MainComponent::resized()
{   
    //here we set the component bounds
    //this is the top half of the window
    selectionComponent.setBoundsRelative(0.015f, 0.02f, 0.97f, 0.48f);
    //bottom half
    granularSynth.setBoundsRelative(0.015f, 0.5f, 0.97f, 0.48f);
}

//==============================================================================
