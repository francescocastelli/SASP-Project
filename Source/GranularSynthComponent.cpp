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
	skipGrain(3),
	audioIsPlaying(false),
	time(0),
	currentGrainIndex(0)
{
	formatManager.registerBasicFormats();

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("Load grain");
	loadGrain.onClick = [this] { readGrains(); setButtonState(true, false, true); };
	loadGrain.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	loadGrain.setEnabled(true);

	addAndMakeVisible(&playAudio);
	playAudio.setButtonText("Play");
	playAudio.onClick = [this] { audioIsPlaying = true; setButtonState(false, true, false); spectrogram.setActive(true); };
	playAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playAudio.setEnabled(false);

	//stop audio from playing
	addAndMakeVisible(&stopAudio);
	stopAudio.setButtonText("Stop");
	stopAudio.onClick = [this] { audioIsPlaying = false; setButtonState(true, false, true); spectrogram.setActive(false);};
	stopAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopAudio.setEnabled(false);

	//grain lenght slider
	addAndMakeVisible(masterVolume);
	masterVolume.setRange(0.0f, 0.95f, 0.001);
	masterVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	masterVolume.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	masterVolume.setValue(0.8);
	masterVolume.setSkewFactor(0.5);
	masterVolume.onValueChange = [this] { outputGain = masterVolume.getValue(); };

	addAndMakeVisible(spectrogram);
	spectrogram.setActive(false);
}

void GranularSynthComponent::readGrains() 
{
	int fileNum (0);
	juce::AudioBuffer<float> tempBuf;

	if (grainStack.size() > 0) {
		for (int i = 0; i < grainStack.size(); ++i)
			delete(grainStack[i]);

		grainStack.clear();
	}

	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(sampleDir.getFullPathName()), false))
	{
		formatReader = formatManager.createReaderFor(entry.getFile());
		tempBuf = juce::AudioBuffer<float> (2, formatReader->lengthInSamples);
		if (formatReader)
		{
			formatReader->read(&tempBuf, 0, formatReader->lengthInSamples, 0, true, true);	
			grainStack.add(new Grain(tempBuf));
		}
	}
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
		
		auto currentGrain = grainStack[currentGrainIndex];
		currentGrain->processBlock(bufferToFill);

		//if (currentGrain->getSamplesLeft() - 120< 0)
		//	grainStack[(currentGrainIndex+1) % ( grainStack.size() -2)]->processBlock(bufferToFill);
		//grainStack[rand.nextInt(grainStack.size() - 1)]->processBlock(bufferToFill);
		
		//no samples left
	    if (!currentGrain->isPlaying())
	    {
			if (skipGrain > 0 )
			{
				//currentGrainIndex = grainStack.size() - 1;
				skipGrain -= 1;
			}
			else 
			{
				currentGrainIndex = (currentGrainIndex+1)%( grainStack.size() -1);
				skipGrain = 0;
			}
	    }

		//apply gain at the output sound
		bufferToFill.buffer->applyGain(outputGain);
		//visualize fft of the block
		spectrogram.setNextAudioBlock(bufferToFill);
	}
}

void GranularSynthComponent::setButtonState(bool enableStart, bool enableStop, bool enableLoad)
{
	playAudio.setEnabled(enableStart);
	stopAudio.setEnabled(enableStop);
	loadGrain.setEnabled(enableLoad);
}

void GranularSynthComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX() + 24, getLocalBounds().getBottomLeft().getY() - 38, getLocalBounds().getBottomLeft().getX() + 672, getLocalBounds().getBottomLeft().getY() - 38);
}

void GranularSynthComponent::resized()
{
	//buttons
	loadGrain.setBoundsRelative(0.02f, 0.9f, 0.16f, 0.08f);
	playAudio.setBoundsRelative(0.41f, 0.9f, 0.08f, 0.08f);
	stopAudio.setBoundsRelative(0.50f, 0.9f, 0.08f, 0.08f);

	//spec
	spectrogram.setBoundsRelative(0.6f, 0.05f, 0.39f, 0.93f);

	//master volume slider
	masterVolume.setBoundsRelative(0.2f, 0.9f, 0.2f, 0.08f);
}

