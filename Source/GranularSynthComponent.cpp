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
	densityValue(1),
	cutOffFreq (0.1f),
	qFactor (0.1f)
{
	formatManager.registerBasicFormats();
	
	addAndMakeVisible(grainLabel);
	grainLabel.setText("GRAINS", juce::NotificationType::dontSendNotification);
	grainLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	grainLabel.setFont(juce::Font (13.0f, juce::Font::bold));

	addAndMakeVisible(filterLabel);
	filterLabel.setText("FILTERS", juce::NotificationType::dontSendNotification);
	filterLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	filterLabel.setFont(juce::Font (13.0f, juce::Font::bold));

	addAndMakeVisible(densityLabel);
	densityLabel.setText("DENSITY", juce::NotificationType::dontSendNotification);
	densityLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	densityLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(lengthLabel);
	lengthLabel.setText("LENGTH", juce::NotificationType::dontSendNotification);
	lengthLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	lengthLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(positionLabel);
	positionLabel.setText("POSITION", juce::NotificationType::dontSendNotification);
	positionLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	positionLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(cutoffLabel);
	cutoffLabel.setText("CUT OFF", juce::NotificationType::dontSendNotification);
	cutoffLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	cutoffLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(qFactorLabel);
	qFactorLabel.setText("RESONANCE", juce::NotificationType::dontSendNotification);
	qFactorLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	qFactorLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(gainLabel);
	gainLabel.setText("GAIN", juce::NotificationType::dontSendNotification);
	gainLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	gainLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(filterTypeLabel);
	filterTypeLabel.setText("FILTER", juce::NotificationType::dontSendNotification);
	filterTypeLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	filterTypeLabel.setFont(juce::Font (10.0f, juce::Font::bold));

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("LOAD GRAINS");
	loadGrain.onClick = [this] { readGrains(); changeCurrentState(SynthState::Loaded); };
	loadGrain.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	loadGrain.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	loadGrain.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	loadGrain.setLookAndFeel(&knobLookAndFeel);

	addAndMakeVisible(&playAudio);
	playAudio.setButtonText("PLAY");
	playAudio.onClick = [this] { changeCurrentState(SynthState::Playing); currentGrainIndex = windowPosition; startThread(); };
	playAudio.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	playAudio.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	playAudio.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	playAudio.setLookAndFeel(&knobLookAndFeel);

	//stop audio from playing
	addAndMakeVisible(&stopAudio);
	stopAudio.setButtonText("STOP");
	stopAudio.onClick = [this] { changeCurrentState(SynthState::Stopped); stopThread(100); };
	stopAudio.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	stopAudio.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	stopAudio.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	stopAudio.setLookAndFeel(&knobLookAndFeel);

	//master volume slider
	addAndMakeVisible(masterVolume);
	masterVolume.setRange(0.0f, 0.95f, 0.001);
	masterVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	masterVolume.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	masterVolume.setColour(juce::Slider::trackColourId, AppColours::buttons);
	masterVolume.setValue(0.8);
	masterVolume.setSkewFactor(0.5);
	masterVolume.onValueChange = [this] { outputGain = masterVolume.getValue(); };
	
	//density slider
	addAndMakeVisible(densitySlider);
	densitySlider.setRange(1, 500, 1);
	densitySlider.setTextBoxIsEditable(false);
	densitySlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	densitySlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	densitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	densitySlider.setLookAndFeel(&knobLookAndFeel);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	densitySlider.onValueChange = [this] { slidersChanged(); };
	densitySlider.setValue(2);

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(0, 20, 1);
	windowLenghtSlider.setTextBoxIsEditable(false);
	windowLenghtSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	windowLenghtSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	windowLenghtSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	windowLenghtSlider.setLookAndFeel(&knobLookAndFeel);
	windowLenghtSlider.onValueChange = [this] { slidersChanged(); };
	windowLenghtSlider.setValue(2);

	//window position slider
	addAndMakeVisible(windowPositionSlider);
	windowPositionSlider.setRange(0, 100, 1);
	windowPositionSlider.setTextBoxIsEditable(false);
	windowPositionSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	windowPositionSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	windowPositionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	windowPositionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	windowPositionSlider.setLookAndFeel(&knobLookAndFeel);
	windowPositionSlider.onValueChange = [this] { slidersChanged(); };
	windowPositionSlider.setValue(0);

	//cut off freq slider
	addAndMakeVisible(cutOffFreqSlider);
	cutOffFreqSlider.setRange(1, 15000, 0.5);
	cutOffFreqSlider.setTextBoxIsEditable(false);
	cutOffFreqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	cutOffFreqSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	cutOffFreqSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	cutOffFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	cutOffFreqSlider.setLookAndFeel(&knobLookAndFeel);
	cutOffFreqSlider.onValueChange = [this] { cutOffFreq = cutOffFreqSlider.getValue(); };
	cutOffFreqSlider.setValue(0);

	//q factor slider
	addAndMakeVisible(qFactorSlider);
	qFactorSlider.setRange(0.1, 30, 0.1);
	qFactorSlider.setTextBoxIsEditable(false);
	qFactorSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	qFactorSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	qFactorSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	qFactorSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	qFactorSlider.setLookAndFeel(&knobLookAndFeel);
	qFactorSlider.onValueChange = [this] { qFactor = qFactorSlider.getValue(); };
	qFactorSlider.setValue(0);

	addAndMakeVisible(filterGainSlider);
	filterGainSlider.setRange(0.0, 1.2, 0.1);
	filterGainSlider.setTextBoxIsEditable(false);
	filterGainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	filterGainSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	filterGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	filterGainSlider.setLookAndFeel(&knobLookAndFeel);
	filterGainSlider.onValueChange = [this] { filterGain = filterGainSlider.getValue(); };
	filterGainSlider.setValue(0);

	addAndMakeVisible(filterTypeSlider);
	filterTypeSlider.setRange(0, 3, 1);
	filterTypeSlider.setTextBoxIsEditable(false);
	filterTypeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	filterTypeSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	filterTypeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterTypeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	filterTypeSlider.setLookAndFeel(&knobLookAndFeel);
	filterTypeSlider.onValueChange = [this] { filterType = filterTypeSlider.getValue(); };
	filterTypeSlider.setValue(0);

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
			setSliderState(true);
			setFilterSliderState(false, false);
			spectrogram.setActive(false);
			grainVisualizer.setActive(true);
            break;

		case SynthState::Stopped:
			setButtonState(true, false, true, true); 
			setSliderState(true);
			setFilterSliderState(false, false);
			spectrogram.setActive(false);
			grainVisualizer.setActive(true);
			timeIndex = 0;
			nextGrainStart = 0;
			grainStack.clear();
            break;

        case SynthState::Playing:
			setButtonState(false, true, false, true); 
			setSliderState(true);
			setFilterSliderState(true, true);
			spectrogram.setActive(true); 
			grainVisualizer.setActive(true);
            break;
		case SynthState::Disable:
			setButtonState(false, false, true, false);
			setSliderState(false);
			setFilterSliderState(false, false);
			spectrogram.setActive(false);
			grainVisualizer.setActive(false);
		default:
			break;
        }
}

void GranularSynthComponent::setButtonState(bool enableStart, bool enableStop, bool enableLoad, bool enableRandom)
{
	playAudio.setEnabled(enableStart);
	stopAudio.setEnabled(enableStop);
	loadGrain.setEnabled(enableLoad);
	randomSelectionButton.setEnabled(enableRandom);
}

//TODO: check if they are always equal in state
void GranularSynthComponent::setSliderState(bool granulationSlider)
{
	//granulation slider
	windowLenghtSlider.setEnabled(granulationSlider);
	windowPositionSlider.setEnabled(granulationSlider);
	densitySlider.setEnabled(granulationSlider);
}

void GranularSynthComponent::setFilterSliderState(bool noFilter, bool filterParam)
{
	cutOffFreqSlider.setEnabled(filterParam);
	qFactorSlider.setEnabled(filterParam);
	filterGainSlider.setEnabled(filterParam);
	filterTypeSlider.setEnabled(noFilter);
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
	//set up reverb
	juce::Reverb::Parameters param = { 0.3f, 0.1f, 0.33f, 0.4f, 1.5f, 0.f};
	reverb.setParameters(param);
	reverb.setSampleRate(sampleRate);

	//set up the filter
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlockExpected;
	spec.numChannels = 2;

	filter.prepare(spec);
	filter.reset();

	//save samplerate
	this->sampleRate = sampleRate;
}

void GranularSynthComponent::releaseResources()
{
}

void GranularSynthComponent::updateFilter()
{
	switch (filterType)
	{
	case 0:
		noFiltering = true;
		break;
	case 1:
		noFiltering = false;
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, cutOffFreq, qFactor, filterGain);
		break;
	case 2:
		noFiltering = false;
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, cutOffFreq, qFactor);
		break;
	case 3:
		noFiltering = false;
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, cutOffFreq, qFactor, filterGain);
		break;
	default:
		break;
	}
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
		auto currentIndex = selectNextGrain();

		formatReader = formatManager.createReaderFor(grainFileStack[currentIndex]);
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
			grainStack.push_back((Grain(tempBuf, grainStart, currentIndex)));
		}

		mtx.lock();
		//remove the grains already played
		if (grainStack.size() > 0)
			grainStack.erase(std::remove_if(grainStack.begin(), grainStack.end(), [this](Grain x) {return x.hasEnded(timeIndex); }), grainStack.end());
		mtx.unlock();

		wait(waitTime);
	}
}

void GranularSynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (currentState == SynthState::Playing)
	{
		if (grainStack.size() > 0)
		{
			//lock to prevent the thread to modify the grain stack during the copy 
			mtx.lock();
			std::deque<Grain> localStack = grainStack;
			//unlock 
			mtx.unlock();

			for (int i = 0; i < bufferToFill.numSamples; ++i)
			{
				for (int j = 0; j < localStack.size(); ++j)
					if (localStack[j].canPlay(timeIndex))
					{
						localStack[j].processBlock(bufferToFill, timeIndex);
						grainVisualizer.addCurrentIndex(localStack[j].getId());
					}

				++timeIndex;
			}

			//filtering 
			updateFilter();

			if (!noFiltering)
			{
				juce::dsp::AudioBlock<float> currentBlock(*bufferToFill.buffer);
				filter.process(juce::dsp::ProcessContextReplacing<float>(currentBlock));
			}

			//apply some reverb to the output
			reverb.processStereo(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);
			//apply gain at the output sound
			//TODO used db
			bufferToFill.buffer->applyGain(outputGain);
			//visualize fft of the block
			spectrogram.setNextAudioBlock(bufferToFill);
		}
	}
}


void GranularSynthComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX() + getWidth() * 0.02f, getLocalBounds().getBottomLeft().getY() - getHeight() * 0.14f, getLocalBounds().getBottomLeft().getX() + getWidth()* 0.575f , getLocalBounds().getBottomLeft().getY() - getHeight()*0.14);
	g.drawLine(getLocalBounds().getTopLeft().getX() + getWidth() * 0.215f, getLocalBounds().getTopLeft().getY() + getHeight() * 0.14f, getLocalBounds().getBottomLeft().getX() + getWidth()* 0.215f , getLocalBounds().getBottomLeft().getY() - getHeight()*0.2f);
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
	densitySlider.setBoundsRelative(-0.01f, 0.575f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.08f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//window position slider
	windowPositionSlider.setBoundsRelative(-0.01f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//random selection button
	randomSelectionButton.setBoundsRelative(0.575f, 0.145f, 0.2f, 0.2f);

	//cut off 
	cutOffFreqSlider.setBoundsRelative(0.2f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//qfactor 
	qFactorSlider.setBoundsRelative(0.3f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//filter type 
	filterTypeSlider.setBoundsRelative(0.3f, 0.575f, AppConstants::knobWidth, AppConstants::knobHeigth);

	filterGainSlider.setBoundsRelative(0.2f, 0.575f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//labels
	grainLabel.setBoundsRelative(0.08f, 0.03f, 0.2f, 0.1f);

	//labels
	filterLabel.setBoundsRelative(0.295f, 0.03f, 0.2f, 0.1f);

	densityLabel.setBoundsRelative(0.0377f, 0.50f, 0.2f, 0.1f);

	positionLabel.setBoundsRelative(0.035f, 0.13f, 0.2f, 0.1f);

	lengthLabel.setBoundsRelative(0.13f, 0.13f, 0.2f, 0.1f);

	cutoffLabel.setBoundsRelative(0.248f, 0.13f, 0.2f, 0.1f);

	qFactorLabel.setBoundsRelative(0.343f, 0.13f, 0.2f, 0.1f);

	gainLabel.setBoundsRelative(0.255f, 0.5f, 0.2f, 0.1f);

	filterTypeLabel.setBoundsRelative(0.351f, 0.5f, 0.2f, 0.1f);
}

