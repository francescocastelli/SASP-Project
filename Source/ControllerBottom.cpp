/*
  ==============================================================================

    ControllerBottom.cpp
    Created: 20 Mar 2021 4:09:50pm
    Author:  Francesco

  ==============================================================================
*/

#include "ControllerBottom.h"

GranularSynthComponent::GranularSynthComponent(Model& model, GrainSelector& grainSelector, AudioLoader& audioLoader, AudioEngine& audioEngine)
	:model(model),
	fftVisualizer(),
	grainsVisualizer(model),
	grainSelector(grainSelector),
	audioLoader(audioLoader)
{

	addAndMakeVisible(fftVisualizer);
	addAndMakeVisible(grainsVisualizer);
	addAndMakeVisible(randomSelectionButton);
	addAndMakeVisible(reverseButton);

	randomSelectionButton.onClick = [this] {randomSelectionButtonClicked(); };
	reverseButton.onClick = [this] {reverseButtonClicked(); };

	fftVisualizer.setEnabled(false, 0);
	grainsVisualizer.setEnabled(false);

	//set the fftvisualizer in the audioEngine
	audioEngine.setfftVisualizer(&fftVisualizer);

	knobLookAndFeel.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	knobLookAndFeel.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	knobLookAndFeel.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	knobLookAndFeel.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	knobLookAndFeel.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);

	addAndMakeVisible(grainLabel);
	grainLabel.setText("GRAINS", juce::NotificationType::dontSendNotification);
	grainLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	grainLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	addAndMakeVisible(filterLabel);
	filterLabel.setText("FILTERS", juce::NotificationType::dontSendNotification);
	filterLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	filterLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	addAndMakeVisible(densityLabel);
	densityLabel.setText("DENSITY", juce::NotificationType::dontSendNotification);
	densityLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	densityLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(lengthLabel);
	lengthLabel.setText("LENGTH", juce::NotificationType::dontSendNotification);
	lengthLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	lengthLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(positionLabel);
	positionLabel.setText("POSITION", juce::NotificationType::dontSendNotification);
	positionLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	positionLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(randomPositionLabel);
	randomPositionLabel.setText("RAND", juce::NotificationType::dontSendNotification);
	randomPositionLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	randomPositionLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(cutoffLabel);
	cutoffLabel.setText("CUT OFF", juce::NotificationType::dontSendNotification);
	cutoffLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	cutoffLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(qFactorLabel);
	qFactorLabel.setText("RESONANCE", juce::NotificationType::dontSendNotification);
	qFactorLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	qFactorLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(filterTypeLabel);
	filterTypeLabel.setText("TYPE", juce::NotificationType::dontSendNotification);
	filterTypeLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	filterTypeLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(randomSelectionLabel);
	randomSelectionLabel.setText("  RANDOM\nSELECTION", juce::NotificationType::dontSendNotification);
	randomSelectionLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	randomSelectionLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(reverseLabel);
	reverseLabel.setText("REVERSE", juce::NotificationType::dontSendNotification);
	reverseLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	reverseLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("LOAD GRAINS");
	loadGrain.onClick = [this] { loadGrainClicked();  };
	loadGrain.setLookAndFeel(&knobLookAndFeel);

	addAndMakeVisible(&playAudio);
	playAudio.setButtonText("PLAY");
	playAudio.onClick = [this] { changeCurrentState(SynthState::Playing); };
	playAudio.setLookAndFeel(&knobLookAndFeel);

	//stop audio from playing
	addAndMakeVisible(&stopAudio);
	stopAudio.setButtonText("STOP");
	stopAudio.onClick = [this] { changeCurrentState(SynthState::Stopped); };
	stopAudio.setLookAndFeel(&knobLookAndFeel);

	// freq ax labels
	addAndMakeVisible(label1e1);
	label1e1.setText("10", juce::NotificationType::dontSendNotification);
	label1e1.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label1e1.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(label1e2);
	label1e2.setText("100", juce::NotificationType::dontSendNotification);
	label1e2.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label1e2.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(label1e3);
	label1e3.setText("1k", juce::NotificationType::dontSendNotification);
	label1e3.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label1e3.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(label1e4);
	label1e4.setText("10k", juce::NotificationType::dontSendNotification);
	label1e4.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label1e4.setFont(juce::Font(10.0f, juce::Font::bold));

	// dB level labels
	addAndMakeVisible(label80dB);
	label80dB.setText("-80dB", juce::NotificationType::dontSendNotification);
	label80dB.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label80dB.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(label0dB);
	label0dB.setText("0dB", juce::NotificationType::dontSendNotification);
	label0dB.setColour(juce::Label::textColourId, AppColours::knobLabel);
	label0dB.setFont(juce::Font(10.0f, juce::Font::bold));

	//master volume slider
	addAndMakeVisible(masterVolume);
	masterVolume.setRange(0.0f, 1.0f, 0.00001);
	masterVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	masterVolume.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	masterVolume.setColour(juce::Slider::trackColourId, AppColours::buttons);
	masterVolume.setValue(0.7f);
	masterVolume.setSkewFactor(0.3);
	masterVolume.onValueChange = [this] { changeMasterVolume(); };

	//density slider
	addAndMakeVisible(densitySlider);
	densitySlider.setLookAndFeel(&knobLookAndFeel);
	densitySlider.setRange(1, 200, 1);
	densitySlider.setTextBoxIsEditable(false);
	densitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	densitySlider.onValueChange = [this] { densitySliderChanged(); };
	densitySlider.setValue(2);

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(1, 20, 1);
	windowLenghtSlider.setTextBoxIsEditable(false);
	windowLenghtSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	windowLenghtSlider.setLookAndFeel(&knobLookAndFeel);
	windowLenghtSlider.onValueChange = [this] { windowSliderChanged(); };
	windowLenghtSlider.setValue(2);

	//window position slider
	addAndMakeVisible(windowPositionSlider);
	windowPositionSlider.setRange(0, 100, 1);
	windowPositionSlider.setTextBoxIsEditable(false);
	windowPositionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	windowPositionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	windowPositionSlider.setLookAndFeel(&knobLookAndFeel);
	windowPositionSlider.onValueChange = [this] { grainSliderChanged(); };
	windowPositionSlider.setValue(0);

	//random position slider
	addAndMakeVisible(randomPositionSlider);
	randomPositionSlider.setRange(-0.99f, 0.99f, 0.01f);
	randomPositionSlider.setTextBoxIsEditable(false);
	randomPositionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
	randomPositionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	randomPositionSlider.setLookAndFeel(&knobLookAndFeel);
	randomPositionSlider.onValueChange = [this] { randomPositionSliderChanged(); };
	randomPositionSlider.setValue(0);

	//cut off freq slider
	addAndMakeVisible(cutOffFreqSlider);
	cutOffFreqSlider.setRange(20, 15000, 0.5);
	cutOffFreqSlider.setTextBoxIsEditable(false);
	cutOffFreqSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	cutOffFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	cutOffFreqSlider.setSkewFactor(0.4);
	cutOffFreqSlider.setLookAndFeel(&knobLookAndFeel);
	cutOffFreqSlider.onValueChange = [this] { filterSliderChanged(); };
	cutOffFreqSlider.setValue(0);

	//q factor slider
	addAndMakeVisible(qFactorSlider);
	qFactorSlider.setRange(0.1, 30, 0.1);
	qFactorSlider.setTextBoxIsEditable(false);
	qFactorSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	qFactorSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	qFactorSlider.setLookAndFeel(&knobLookAndFeel);
	qFactorSlider.onValueChange = [this] { filterSliderChanged(); };
	qFactorSlider.setValue(0);

	addAndMakeVisible(filterTypeSlider);
	filterTypeSlider.setRange(0, 3, 1);
	filterTypeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	filterTypeSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	filterTypeSlider.setColour(juce::Slider::backgroundColourId, AppColours::waveformBackground);
	filterTypeSlider.setColour(juce::Slider::trackColourId, AppColours::buttons);
	filterTypeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterTypeSlider.setTextBoxIsEditable(false);
	filterTypeSlider.onValueChange = [this] { filterSliderChanged(); };
	filterTypeSlider.setValue(0);

	/*
	addAndMakeVisible(filterTypeSlider);
	filterTypeSlider.setRange(0, 3, 1);
	filterTypeSlider.setTextBoxIsEditable(false);
	filterTypeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterTypeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	filterTypeSlider.setLookAndFeel(&knobLookAndFeel);
	filterTypeSlider.onValueChange = [this] {filterSliderChanged(); };
	filterTypeSlider.setValue(0);
	*/

	changeCurrentState(SynthState::Disable);
}

void GranularSynthComponent::paint(juce::Graphics& g)
{
	g.setColour(AppColours::waveformBorder);

	// bottom line 
	g.drawLine(getLocalBounds().getBottomLeft().getX() + getWidth() * 0.02f, 
		       getLocalBounds().getBottomLeft().getY() - getHeight() * 0.14f, 
		       getLocalBounds().getBottomLeft().getX() + getWidth() * 0.545f, 
			   getLocalBounds().getBottomLeft().getY() - getHeight() * 0.14f);

	// line btw filters and grains
	g.drawLine(getLocalBounds().getTopLeft().getX() + getWidth() * 0.374f, 
		       getLocalBounds().getTopLeft().getY() + getHeight() * 0.14f, 
		       getLocalBounds().getBottomLeft().getX() + getWidth() * 0.374f, 
		       getLocalBounds().getBottomLeft().getY() - getHeight() * 0.2f);
}

void GranularSynthComponent::resized()
{
	//spec
	fftVisualizer.setBoundsRelative(0.6f, 0.48f, 0.39f, 0.47f);

	//grain vis
	grainsVisualizer.setBoundsRelative(0.6f, 0.04f, 0.39f, 0.4f);

	//buttons
	loadGrain.setBoundsRelative(0.02f, 0.9f, 0.14f, 0.08f);
	playAudio.setBoundsRelative(0.41f, 0.9f, 0.06f, 0.08f);
	stopAudio.setBoundsRelative(0.485f, 0.9f, 0.06f, 0.08f);

	//master volume slider
	masterVolume.setBoundsRelative(0.18f, 0.9f, 0.2f, 0.08f);

	//density slider
	densitySlider.setBoundsRelative(-0.01f, 0.575f, AppConstants::knobWidth, 
									AppConstants::knobHeigth);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.08f, 0.2f, AppConstants::knobWidth, 
										 AppConstants::knobHeigth);

	//window position slider
	windowPositionSlider.setBoundsRelative(-0.01f, 0.2f, AppConstants::knobWidth, 
										   AppConstants::knobHeigth);
	randomPositionSlider.setBoundsRelative(0.08f, 0.575f, AppConstants::knobWidth, 
										   AppConstants::knobHeigth);

	//random selection button
	randomSelectionButton.setBoundsRelative(0.315f, 0.25f, 0.05f, 0.1f);
	reverseButton.setBoundsRelative(0.23f, 0.25f, 0.05f, 0.1f);

	//cut off 
	cutOffFreqSlider.setBoundsRelative(0.35f, 0.2f, AppConstants::knobWidth, 
									   AppConstants::knobHeigth);

	//qfactor 
	qFactorSlider.setBoundsRelative(0.45f, 0.2f, AppConstants::knobWidth, 
									AppConstants::knobHeigth);

	//filter type 
	//filterTypeSlider.setBoundsRelative(0.45f, 0.575f, AppConstants::knobWidth, 
	//								   AppConstants::knobHeigth);
	filterTypeSlider.setBoundsRelative(0.40f, 0.575f, AppConstants::knobWidth , 
									   AppConstants::knobHeigth *0.8f);

	//labels
	grainLabel.setBoundsRelative(0.175f, 0.03f, 0.2f, 0.1f);

	//labels
	filterLabel.setBoundsRelative(0.445f, 0.03f, 0.2f, 0.1f);
	densityLabel.setBoundsRelative(0.0377f, 0.50f, 0.2f, 0.1f);
	positionLabel.setBoundsRelative(0.035f, 0.13f, 0.2f, 0.1f);
	randomPositionLabel.setBoundsRelative(0.135f, 0.50f, 0.2f, 0.1f);
	lengthLabel.setBoundsRelative(0.13f, 0.13f, 0.2f, 0.1f);
	cutoffLabel.setBoundsRelative(0.398f, 0.13f, 0.2f, 0.1f);
	qFactorLabel.setBoundsRelative(0.493f, 0.13f, 0.2f, 0.1f);
	filterTypeLabel.setBoundsRelative(0.455f, 0.5f, 0.2f, 0.1f);
	reverseLabel.setBoundsRelative(0.22f, 0.13f, 0.2f, 0.1f);
	randomSelectionLabel.setBoundsRelative(0.3f, 0.13f, 0.2f, 0.1f);

	// freq ax label
	//label1e1.setBoundsRelative(0.595f, 0.895f, 0.2f, 0.15f);
	label1e2.setBoundsRelative(0.71f, 0.895f, 0.2f, 0.15f);
	label1e3.setBoundsRelative(0.83f, 0.895f, 0.2f, 0.15f);
	label1e4.setBoundsRelative(0.945f, 0.895f, 0.2f, 0.15f);

	// db levels label
	label80dB.setBoundsRelative(0.57f, 0.88f, 0.2f, 0.1f);
	label0dB.setBoundsRelative(0.578f, 0.45f, 0.2f, 0.1f);
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
		fftVisualizer.setEnabled(false, 0.);
		grainsVisualizer.setEnabled(true);
		break;

	case SynthState::Stopped:
		setButtonState(true, false, true, true);
		setSliderState(true);
		setFilterSliderState(false, false);
		fftVisualizer.setEnabled(false, 0.);
		grainsVisualizer.setEnabled(true);
		model.getWriteTime() = 0;
		model.getAudioState() = ModelAudioState::StopAudio;
		grainSelector.reset();
		break;

	case SynthState::Playing:
		setButtonState(false, true, false, true);
		setSliderState(true);
		setFilterSliderState(true, true);
		model.getAudioState() = ModelAudioState::grainPlay;
		grainSelector.start();
		model.getGrainCurrentIndex().store(model.getGrainPosition());
		fftVisualizer.setEnabled(true, model.getReadSamplerate());
		grainsVisualizer.setEnabled(true);
		break;
	case SynthState::Disable:
		setButtonState(false, false, true, false);
		setSliderState(false);
		setFilterSliderState(false, false);
		fftVisualizer.setEnabled(false, 0.);
		grainsVisualizer.setEnabled(false);
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

void GranularSynthComponent::setSliderState(bool granulationSlider)
{
	//granulation slider
	windowLenghtSlider.setEnabled(granulationSlider);
	windowPositionSlider.setEnabled(granulationSlider);
	densitySlider.setEnabled(granulationSlider);
	randomPositionSlider.setEnabled(granulationSlider);
}

void GranularSynthComponent::setFilterSliderState(bool noFilter, bool filterParam)
{
	cutOffFreqSlider.setEnabled(filterParam);
	qFactorSlider.setEnabled(filterParam);
	filterTypeSlider.setEnabled(noFilter);
}

void GranularSynthComponent::changeMasterVolume()
{
	model.getWriteGain() = masterVolume.getValue();
}

void GranularSynthComponent::filterSliderChanged()
{
	model.getFilterCutoff() = cutOffFreqSlider.getValue();
	model.getFilterResonance() = qFactorSlider.getValue();
	model.getFilterType() = filterTypeSlider.getValue();
}

void GranularSynthComponent::grainSliderChanged()
{
	if (currentState != SynthState::Disable)
	{
		model.getGrainPosition() = windowPositionSlider.getValue();
		model.getGrainCurrentIndex().store(model.getGrainPosition());

		//update the ranges
		windowPositionSlider.setRange(0, juce::jmax(model.getWriteGrainstack().size()-1, 1), 1);
		if (model.getWriteGrainstack().size() - model.getGrainPosition() <= 0)
			windowLenghtSlider.setEnabled(false);
		else
		{
			windowLenghtSlider.setRange(0, model.getWriteGrainstack().size() - model.getGrainPosition(), 1);
			windowLenghtSlider.setEnabled(true);
		}
	}
}

void GranularSynthComponent::windowSliderChanged()
{
	model.getGrainWindowLength() = windowLenghtSlider.getValue();
}
 
void GranularSynthComponent::densitySliderChanged()
{
	model.getWriteDensity() = densitySlider.getValue();
}

void GranularSynthComponent::loadGrainClicked()
{
	audioLoader.loadGrainFile(); 
	if (model.getWriteGrainstack().size() != 0)
	{
		grainsVisualizer.setGrains();
		grainsVisualizer.setEnabled(true);
		changeCurrentState(SynthState::Loaded); 
		grainSliderChanged();
	} else
		juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, "No grains found!", "Please select some grains before loading");
}

void GranularSynthComponent::randomPositionSliderChanged()
{
	model.getRandomPosition() = randomPositionSlider.getValue();
}
   
void GranularSynthComponent::randomSelectionButtonClicked()
{
	model.setRandomSelection(randomSelectionButton.getToggleStateValue().getValue());
}
    
void GranularSynthComponent::reverseButtonClicked()
{
	model.setReverse(reverseButton.getToggleStateValue().getValue());
}
