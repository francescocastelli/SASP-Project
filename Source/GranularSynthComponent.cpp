/*
  ==============================================================================

    GranularSynthComponent.cpp
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#include "GranularSynthComponent.h"


GranularSynthComponent::GranularSynthComponent(juce::File& sampleDir)
	:sampleDir(sampleDir),
	index(0),
	skipGrain(false),
	audioIsPlaying(false),
	currentGrain(0)
{
	formatManager.registerBasicFormats();

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("Load grain");
	loadGrain.onClick = [this] { readGrains(); };
	loadGrain.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	loadGrain.setEnabled(true);

	addAndMakeVisible(&playAudio);
	playAudio.setButtonText("Play");
	playAudio.onClick = [this] { audioIsPlaying = true; playAudio.setEnabled(false); stopAudio.setEnabled(true); };
	playAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playAudio.setEnabled(false);

	//stop audio from playing
	addAndMakeVisible(&stopAudio);
	stopAudio.setButtonText("Stop");
	stopAudio.onClick = [this] { audioIsPlaying = false; playAudio.setEnabled(true); stopAudio.setEnabled(false); };
	stopAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	stopAudio.setEnabled(false);

	addAndMakeVisible(spectrogram);
}

void GranularSynthComponent::readGrains() 
{
	int fileNum (0);

	juce::BigInteger range;
	range.setRange(0, 128, true);
	juce::AudioBuffer<float> buf = juce::AudioBuffer<float> (2, 4410); 

	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(sampleDir.getFullPathName()), false))
	{
		formatReader = formatManager.createReaderFor(entry.getFile());
		//synth.addSound(new juce::SamplerSound(entry.getFile().getFileName(), *formatReader, range, 60, 0.1, 0.1, 0.1));
		if (formatReader)
		{
			formatReader->read(&buf, 0, 4410, 0, true, true);	
			grainStack.add(Grain(buf));
		}
	}

	//enable the play audio button
	playAudio.setEnabled(true);
}

void GranularSynthComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
}

void GranularSynthComponent::releaseResources()
{
}

void GranularSynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (grainStack.size() > 0 && audioIsPlaying )
	{
		auto numSamples = int(grainStack[currentGrain].getBufferPointer().getNumSamples());

		for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
			bufferToFill.buffer->copyFrom(channel, 0, grainStack[currentGrain].getBufferPointer(), channel, index, juce::jmin(bufferToFill.numSamples, numSamples - index));

		index += bufferToFill.numSamples;
	    if (index > numSamples)
	    {
			index = 0;
			currentGrain = rand.nextInt(grainStack.size() - 1);
			skipGrain = true;
	    }
		spectrogram.setNextAudioBlock(bufferToFill);
	}
}

void GranularSynthComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::waveformBorder);
	g.drawRect(getLocalBounds());
}

void GranularSynthComponent::resized()
{
	//buttons
	loadGrain.setBoundsRelative(0.80f, 0.4f, 0.2f, 0.08f);
	playAudio.setBoundsRelative(0.80f, 0.5f, 0.2f, 0.08f);
	stopAudio.setBoundsRelative(0.8f, 0.6f, 0.2f, 0.08f);

	//spec
	spectrogram.setBoundsRelative(0.02f, 0.05f, 0.4f, 0.9f);
}

