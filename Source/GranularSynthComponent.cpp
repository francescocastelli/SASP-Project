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
	windowPosition(0),
	sampleRate(44100),
	currentGrainIndex(0),
	nextGrainStart(0),
	randomPosOffset(2000),
	densityValue(1)
{
	formatManager.registerBasicFormats();

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("Load grain");
	loadGrain.onClick = [this] { readGrains(); changeCurrentState(SynthState::Loaded); };
	loadGrain.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);

	addAndMakeVisible(&playAudio);
	playAudio.setButtonText("Play");
	playAudio.onClick = [this] { changeCurrentState(SynthState::Playing); currentGrainIndex = windowPosition; startThread(); };
	playAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

	//stop audio from playing
	addAndMakeVisible(&stopAudio);
	stopAudio.setButtonText("Stop");
	stopAudio.onClick = [this] { changeCurrentState(SynthState::Stopped); stopThread(100); };
	stopAudio.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

	//master volume slider
	addAndMakeVisible(masterVolume);
	masterVolume.setRange(0.0f, 0.95f, 0.001);
	masterVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	masterVolume.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	masterVolume.setValue(0.8);
	masterVolume.setSkewFactor(0.5);
	masterVolume.onValueChange = [this] { outputGain = masterVolume.getValue(); };
	
	//density slider
	addAndMakeVisible(densitySlider);
	densitySlider.setRange(1, 500, 1);
	densitySlider.setTextBoxIsEditable(false);
	densitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 15);
	//densitySlider.hideTextBox(true);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	densitySlider.onValueChange = [this] { slidersChanged(); };
	densitySlider.setValue(2);

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(0, 20, 1);
	windowLenghtSlider.setTextBoxIsEditable(false);
	windowLenghtSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 15);
	//windowLenghtSlider.hideTextBox(true);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	windowLenghtSlider.onValueChange = [this] { slidersChanged(); };
	windowLenghtSlider.setValue(2);

	//window position slider
	addAndMakeVisible(windowPositionSlider);
	windowPositionSlider.setRange(0, 100, 1);
	windowPositionSlider.setTextBoxIsEditable(false);
	windowPositionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 15);
	//windowPositionSlider.hideTextBox(true);
	windowPositionSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	windowPositionSlider.onValueChange = [this] { slidersChanged(); };
	windowPositionSlider.setValue(0);

	//random selection button
	addAndMakeVisible(randomSelectionButton);

	//fft visualizer
	addAndMakeVisible(spectrogram);

	//grain visualizer
	addAndMakeVisible(grainVisualizer);

	//set everything to the initial correct value
	changeCurrentState(SynthState::Disable);
}

void GranularSynthComponent::changeCurrentState(SynthState newState)
{
	currentState = newState;

	switch (currentState)
        {
        case SynthState::Loaded:
			setButtonState(true, false, true, true);
			setSliderState(true, true);
			spectrogram.setActive(false);
			grainVisualizer.setActive(true);
            break;

		case SynthState::Stopped:
			setButtonState(true, false, true, true); 
			setSliderState(true, true);
			spectrogram.setActive(false);
			grainVisualizer.setActive(true);
			timeIndex = 0;
			nextGrainStart = 0;
			grainStack.clear();
            break;

        case SynthState::Playing:
			setButtonState(false, true, false, true); 
			setSliderState(true, true);
			spectrogram.setActive(true); 
			grainVisualizer.setActive(true);
            break;
		case SynthState::Disable:
			setButtonState(false, false, true, false);
			setSliderState(false, false);
			spectrogram.setActive(false);
			grainVisualizer.setActive(false);
		default:
			break;
        }
}

void GranularSynthComponent::slidersChanged()
{
	if (currentState != SynthState::Disable)
	{
		windowPosition = windowPositionSlider.getValue();
		windowLenght = windowLenghtSlider.getValue();
		densityValue = densitySlider.getValue();

		//update the ranges
		windowPositionSlider.setRange(0, grainFileStack.size(), 1);
		windowLenghtSlider.setRange(0, grainFileStack.size() - windowPosition + 1, 1);
		//densitySlider.setRange(1, windowLenght, 1);
	}
}

void GranularSynthComponent::readGrains() 
{
	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(sampleDir.getFullPathName()), false))
	{
		grainFileStack.add(entry.getFile());
	}

	grainVisualizer.setGrains(grainFileStack);
}

void GranularSynthComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	juce::Reverb::Parameters param = { 0.3f, 0.1f, 0.33f, 0.4f, 1.5f, 0.f};
	reverb.setParameters(param);
	reverb.setSampleRate(sampleRate);

	//save samplerate
	this->sampleRate = sampleRate;
}

void GranularSynthComponent::releaseResources()
{
}

int GranularSynthComponent::selectNextGrain()
{
	//linear or random selection based on the state of the button
	if (randomSelectionButton.getToggleState()) 
	{
		currentGrainIndex = juce::Random::getSystemRandom().nextInt(juce::Range<int> (windowPosition, windowPosition + windowLenght));
	}
	else
	{
		if (currentGrainIndex > windowPosition + windowLenght) 
			currentGrainIndex = windowPosition;
		else 
			++currentGrainIndex;

		return currentGrainIndex;
	}
}

void GranularSynthComponent::run()
{
	juce::AudioBuffer<float> tempBuf;
	juce::AudioFormatReader* formatReader;
	double waitTime = -1;

	while (!threadShouldExit())
	{
		//read the buffer for the current grain
		formatReader = formatManager.createReaderFor(grainFileStack[selectNextGrain()]);
		if (formatReader)
		{
			tempBuf = juce::AudioBuffer<float> (2, formatReader->lengthInSamples);
			formatReader->read(&tempBuf, 0, formatReader->lengthInSamples, 0, true, true);	


			if (nextGrainStart == 0) nextGrainStart = timeIndex;

			//float offset = juce::Random::getSystemRandom().nextInt(juce::Range<int> (-randomPosOffset, randomPosOffset));

			//set the start index for the current grain
			//500 for the wait of the thread
			long long grainStart = nextGrainStart + 500;

			float duration = (sampleRate / densityValue) + tempBuf.getNumSamples();

			//set the start index for the next grain 
			// next = start pos of this + its duration
			nextGrainStart = grainStart + duration;
			//time to wait
			waitTime = (duration * 1000)/sampleRate;
			//add the buffer to the grain stack
			grainStack.push_back((Grain(tempBuf, grainStart)));
		}

		//remove the grains already played
		if (grainStack.size() > 0)
			grainStack.erase(std::remove_if(grainStack.begin(), grainStack.end(), [this](Grain x) {return x.hasEnded(timeIndex); }), grainStack.end());

		wait(waitTime);
	}
}

void GranularSynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (currentState == SynthState::Playing)
	{
		std::deque<Grain> localStack = grainStack;

		for (int i = 0; i < bufferToFill.numSamples; ++i)
		{
			for (int j = 0; j < localStack.size(); ++j)
				if (localStack[j].canPlay(timeIndex))
					localStack[j].processBlock(bufferToFill, timeIndex);

			++timeIndex;
		}

		//apply some reverb to the output
		reverb.processStereo(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
		//apply gain at the output sound
		bufferToFill.buffer->applyGain(outputGain);
		//visualize fft of the block
		spectrogram.setNextAudioBlock(bufferToFill);
	}
}

void GranularSynthComponent::setButtonState(bool enableStart, bool enableStop, bool enableLoad, bool enableRandom)
{
	playAudio.setEnabled(enableStart);
	stopAudio.setEnabled(enableStop);
	loadGrain.setEnabled(enableLoad);
	randomSelectionButton.setEnabled(enableRandom);
}

void GranularSynthComponent::setSliderState(bool enableWindowPos, bool enableWindowL)
{
	windowLenghtSlider.setEnabled(enableWindowL);
	windowPositionSlider.setEnabled(enableWindowPos);
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
	spectrogram.setBoundsRelative(0.6f, 0.48f, 0.39f, 0.5f);

	//grain vis
	grainVisualizer.setBoundsRelative(0.6f, 0.04f, 0.39f, 0.4f);

	//master volume slider
	masterVolume.setBoundsRelative(0.2f, 0.9f, 0.2f, 0.08f);

	//density slider
	densitySlider.setBoundsRelative(-0.02f, 0.50f, 0.15f, 0.3f);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.08f, 0.1f, 0.15f, 0.3f);

	//window position slider
	windowPositionSlider.setBoundsRelative(-0.02f, 0.1f, 0.15f, 0.3f);

	//random selection button
	randomSelectionButton.setBoundsRelative(0.4f, 0.3f, 0.2f, 0.2f);
}

