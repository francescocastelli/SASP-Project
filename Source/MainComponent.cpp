#pragma once
#include "MainComponent.h"
#include "AppConstants.h"

//==============================================================================
MainComponent::MainComponent()
    :inputAudioState(SoundState::Stopped),
    selectionComponent(transportSource, sampleDir),
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

    transportSource.addChangeListener(this);
    selectionComponent.addChangeListener(this);
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
    if (source == &transportSource) changeState(transportSource.isPlaying() ? SoundState::Playing : SoundState::Stopped);
    if (source == &selectionComponent) changeState(selectionComponent.getState());
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
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
    selectionComponent.setBoundsRelative(0.02f, 0.05f, 0.97f, 0.55f);
}

//==============================================================================

void MainComponent::changeState(SoundState newState)
{
    if (inputAudioState != newState)
    {
        inputAudioState = newState;

        switch (inputAudioState)
        {
        case SoundState::Stopped:
            selectionComponent.setButtonsEnable(true, false, true);
            transportSource.setPosition(0.0);
            break;

        case SoundState::Starting:
            selectionComponent.setButtonsEnable(false, true, false);
            transportSource.start();
            break;

        case SoundState::Playing:
            selectionComponent.setButtonsEnable(false, true, false);
            break;

        case SoundState::Stopping:
            transportSource.stop();
            break;
        }
    }
}
