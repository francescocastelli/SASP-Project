/*
  ==============================================================================

    GranularSynthComponent.cpp
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#include "GranularSynthComponent.h"

GranularSynthComponent::GranularSynthComponent(juce::File& sampleDir)
	:Thread("Thread"),
	sampleDir(sampleDir),
	audioIsPlaying(false),
	grainLoaded(false),
	timeIndex(0),
	windowLenght(0),
	windowPosition (0),
	sampleRate(44100),
	densityValue(0)
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
	densitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	densitySlider.hideTextBox(true);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	densitySlider.onValueChange = [this] { slidersChanged(); };
	densitySlider.setValue(2);

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(0, 20, 1);
	windowLenghtSlider.setTextBoxIsEditable(false);
	//windowLenghtSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	//windowLenghtSlider.hideTextBox(true);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	windowLenghtSlider.onValueChange = [this] { slidersChanged(); };
	windowLenghtSlider.setValue(2);
	windowLenghtSlider.setEnabled(false);

	//window position slider
	addAndMakeVisible(windowPositionSlider);
	windowPositionSlider.setRange(0, 100, 1);
	windowPositionSlider.setTextBoxIsEditable(false);
	//windowPositionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	//windowPositionSlider.hideTextBox(true);
	windowPositionSlider.setEnabled(false);
	windowPositionSlider.onValueChange = [this] { slidersChanged(); };
	windowPositionSlider.setValue(0);

	addAndMakeVisible(spectrogram);
	spectrogram.setActive(false);
}

void GranularSynthComponent::slidersChanged()
{
	if (grainLoaded)
	{
		windowPosition = windowPositionSlider.getValue();
		windowLenght = windowLenghtSlider.getValue();
		densityValue = densitySlider.getValue();

		//update the ranges
		windowPositionSlider.setRange(0, grainStack.size(), 1);
		windowLenghtSlider.setRange(0, grainStack.size() - windowPosition + 1, 1);
		//densitySlider.setRange(0, windowLenght);
	}
}

void GranularSynthComponent::readGrains() 
{
	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(sampleDir.getFullPathName()), false))
	{
		grainFileStack.add(entry.getFile());
	}

	grainLoaded = true;
	startThread();
}

void GranularSynthComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	juce::Reverb::Parameters param = { 0.3f, 0.1f, 0.33f, 0.4f, 1.5f, 0.f};
	reverb.setParameters(param);
	reverb.setSampleRate(sampleRate);
}

void GranularSynthComponent::releaseResources()
{
}

void GranularSynthComponent::run()
{
	juce::AudioBuffer<float> tempBuf;
	juce::AudioFormatReader* formatReader;

	while (!threadShouldExit())
	{
		//read the buffer for the current grain
		formatReader = formatManager.createReaderFor(grainFileStack[0]);
		if (formatReader)
		{
			tempBuf = juce::AudioBuffer<float> (2, formatReader->lengthInSamples);
			formatReader->read(&tempBuf, 0, formatReader->lengthInSamples, 0, true, true);	
			//add the buffer to the grain stack
			grainStack.push_back((Grain(tempBuf, timeIndex + 500)));
		}

		//remove the grains already played
		if (grainStack.size() > 0)
			grainStack.erase(std::remove_if(grainStack.begin(), grainStack.end(), [this](Grain x) {return x.hasEnded(timeIndex); }), grainStack.end());

		wait(100);
	}
}

void GranularSynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (grainLoaded )
	{
		std::deque<Grain> localStack = grainStack;

		for (int i = 0; i < bufferToFill.numSamples; ++i)
		{
			for (int j = 0; j < localStack.size(); ++j)
				if (localStack[j].canPlay(timeIndex))
					localStack[j].processBlock(bufferToFill, timeIndex);

			++timeIndex;
		}

		//reverb.processStereo(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
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
	densitySlider.setBoundsRelative(-0.02f, 0.62f, 0.15f, 0.25f);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.1f, 0.62f, 0.15f, 0.25f);

	//window position slider
	windowPositionSlider.setBoundsRelative(0.1f, 0.30f, 0.25f, 0.2f);
}

