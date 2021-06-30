/*
  ==============================================================================

    ControllerMainComponent.cpp
    Created: 20 Mar 2021 4:04:30pm
    Author:  Francesco

  ==============================================================================
*/

#include "ControllerMainComponent.h"

ControllerMainComponent::ControllerMainComponent()
    :selectionComponent(model, audioLoader, grainProcessor),
    granularSynthComponent(model, grainSelector, audioLoader, audioEngine),
    audioEngine(model),
    audioLoader(model),
    grainProcessor(model),
    grainSelector(model)
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
    addAndMakeVisible(granularSynthComponent);
    setAudioChannels(0, 2);

    setSize (1200, 600);

	//create the directory based on the filename
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::QuestionIcon, "Grains folder", 
										   "Choose a folder to store grains", juce::String(), this,
										   juce::ModalCallbackFunction::create(selectGrainFolder, this));

    //dir.createDirectory();
    //model.getGrainDirectory() = dir;
}

ControllerMainComponent::~ControllerMainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void ControllerMainComponent::paint(juce::Graphics& g)
{
    //background of the entire window
    g.fillAll(AppColours::background);
}

void ControllerMainComponent::resized()
{
    //here we set the component bounds
    //this is the top half of the window
    selectionComponent.setBoundsRelative(0.015f, 0.02f, 0.97f, 0.48f);
    //bottom half
    granularSynthComponent.setBoundsRelative(0.015f, 0.5f, 0.97f, 0.48f);
}

void ControllerMainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    model.getWriteSamplerate() = sampleRate;
    audioEngine.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void ControllerMainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    //audio engine will process the block
    audioEngine.processNextAudioBlock(bufferToFill);
}

void ControllerMainComponent::releaseResources()
{
    model.getAudioThumbnail().clear();
    model.getTransportSource().releaseResources();
}

