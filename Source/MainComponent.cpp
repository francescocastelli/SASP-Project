#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :inputAudioState(SoundState::Stopped),
    waveComponent(transportSource, sampleDir),
    //for now set the path as fixed, then the user should be able to set it
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

    addAndMakeVisible(waveComponent);

    transportSource.addChangeListener(this);
    waveComponent.addChangeListener(this);
    setSize (800, 600);

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
    if (source == &waveComponent) changeState(waveComponent.getState());
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
}

void MainComponent::resized()
{
   /* openButton.setBoundsRelative(0.82f, 0.05f, 0.05f, 0.03f);
    playButton.setBoundsRelative(0.88f, 0.05f, 0.05f, 0.03f);
    stopButton.setBoundsRelative(0.94f, 0.05f, 0.05f, 0.03f);
    selectionButton.setBoundsRelative(0.82f, 0.1f, 0.125f, 0.03f);

    waveDrawer.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.3f);
    positionDrawer.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.3f);*/
    waveComponent.setBoundsRelative(0.02f, 0.05f, 0.97f, 0.4f);
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
            waveComponent.setButtonsEnable(true, false, true);
            transportSource.setPosition(0.0);
            break;

        case SoundState::Starting:
            waveComponent.setButtonsEnable(false, true, false);
            transportSource.start();
            break;

        case SoundState::Playing:
            waveComponent.setButtonsEnable(false, true, false);
            break;

        case SoundState::Stopping:
            transportSource.stop();
            break;
        }
    }
}
