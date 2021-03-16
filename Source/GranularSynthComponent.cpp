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
	timeIndex(0),
	windowLenght(0),
	sampleRate(44100),
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
	
	//density slider
	addAndMakeVisible(densitySlider);
	densitySlider.setRange(0, 10, 1);
	densitySlider.setTextBoxIsEditable(false);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	densitySlider.setValue(2);
	densitySlider.onValueChange = [this] { densityValue = densitySlider.getValue(); grainSelectionAndPositioning(); };

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(0, 10, 0.01);
	windowLenghtSlider.setTextBoxIsEditable(false);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	windowLenghtSlider.setValue(2);
	windowLenghtSlider.onValueChange = [this] { windowLenght = windowLenghtSlider.getValue() * sampleRate; grainSelectionAndPositioning(); };

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
	
	//set max value for density 
	densitySlider.setRange(0, grainStack.size(), 1);
	//select and position grain in the window
	grainSelectionAndPositioning();
}

void GranularSynthComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
}

void GranularSynthComponent::releaseResources()
{
}

void GranularSynthComponent::grainSelectionAndPositioning()
{
	std::vector<int> v(grainStack.size());
	std::iota(v.begin(), v.end(), 0);
	std::random_shuffle(v.begin(), v.end());
	
	//clear the selected grain array
	selectedGrain.clear();

	//fill in a random way the selected grain array
	for (int i = 0; i < std::min(densityValue, grainStack.size()); ++i)
		selectedGrain.add(grainStack[v[i]]);

	//set the starting position inside the window for each grain
	for (int i = 0; i < selectedGrain.size(); ++i)
		selectedGrain[i]->setStartIndex(rand.nextInt(windowLenght));
}

void GranularSynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (grainStack.size() > 0 && audioIsPlaying )
	{
		//iterate over the selected grains
		//only the ones with active = true will really process the block
		for (int i = 0; i < selectedGrain.size(); ++i)
		{
			if (std::abs(selectedGrain[i]->getStartIndex() - timeIndex) < bufferToFill.numSamples)
				selectedGrain[i]->activeGrain();

			selectedGrain[i]->processBlock(bufferToFill, timeIndex);
		}
		
		//increment the time index
		timeIndex += bufferToFill.numSamples;
		if (timeIndex > windowLenght)
		{
			timeIndex = 0;
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

	//density slider
	densitySlider.setBoundsRelative(0.1f, 0.4f, 0.4f, 0.4f);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.4f, 0.4f, 0.2, 0.4f);
}

