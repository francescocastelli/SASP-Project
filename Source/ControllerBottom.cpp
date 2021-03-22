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
	audioLoader(audioLoader),
	noGrainsAlert("No grains found!", "Please select some grains before loading", juce::AlertWindow::InfoIcon)
{

	addAndMakeVisible(fftVisualizer);
	addAndMakeVisible(grainsVisualizer);

	fftVisualizer.setEnabled(false);
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

	addAndMakeVisible(gainLabel);
	gainLabel.setText("GAIN", juce::NotificationType::dontSendNotification);
	gainLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	gainLabel.setFont(juce::Font(10.0f, juce::Font::bold));

	addAndMakeVisible(filterTypeLabel);
	filterTypeLabel.setText("FILTER", juce::NotificationType::dontSendNotification);
	filterTypeLabel.setColour(juce::Label::textColourId, AppColours::knobLabel);
	filterTypeLabel.setFont(juce::Font(10.0f, juce::Font::bold));

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

	//master volume slider
	addAndMakeVisible(masterVolume);
	masterVolume.setRange(0.0f, 0.95f, 0.001);
	masterVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	masterVolume.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	masterVolume.setColour(juce::Slider::trackColourId, AppColours::buttons);
	masterVolume.setValue(0.8);
	masterVolume.setSkewFactor(0.5);
	masterVolume.onValueChange = [this] { changeMasterVolume(); };

	//density slider
	addAndMakeVisible(densitySlider);
	densitySlider.setLookAndFeel(&knobLookAndFeel);
	densitySlider.setRange(1, 200, 1);
	densitySlider.setTextBoxIsEditable(false);
	densitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	densitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	densitySlider.onValueChange = [this] { grainSliderChanged(); };
	densitySlider.setValue(2);

	//window lenght slider
	addAndMakeVisible(windowLenghtSlider);
	windowLenghtSlider.setRange(1, 20, 1);
	windowLenghtSlider.setTextBoxIsEditable(false);
	windowLenghtSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	windowLenghtSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	windowLenghtSlider.setLookAndFeel(&knobLookAndFeel);
	windowLenghtSlider.onValueChange = [this] { grainSliderChanged(); };
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
	randomPositionSlider.setRange(-1.0f, 1.0f, 0.01f);
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

	addAndMakeVisible(filterGainSlider);
	filterGainSlider.setRange(0.0, 1.2, 0.01);
	filterGainSlider.setTextBoxIsEditable(false);
	filterGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	filterGainSlider.setLookAndFeel(&knobLookAndFeel);
	filterGainSlider.onValueChange = [this] {filterSliderChanged(); };
	filterGainSlider.setValue(0);

	addAndMakeVisible(filterTypeSlider);
	filterTypeSlider.setRange(0, 3, 1);
	filterTypeSlider.setTextBoxIsEditable(false);
	filterTypeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	filterTypeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	filterTypeSlider.setLookAndFeel(&knobLookAndFeel);
	filterTypeSlider.onValueChange = [this] {filterSliderChanged(); };
	filterTypeSlider.setValue(0);

	changeCurrentState(SynthState::Disable);
}

void GranularSynthComponent::paint(juce::Graphics& g)
{
	g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX() + getWidth() * 0.02f, getLocalBounds().getBottomLeft().getY() - getHeight() * 0.14f, getLocalBounds().getBottomLeft().getX() + getWidth() * 0.575f, getLocalBounds().getBottomLeft().getY() - getHeight() * 0.14);
	g.drawLine(getLocalBounds().getTopLeft().getX() + getWidth() * 0.215f, getLocalBounds().getTopLeft().getY() + getHeight() * 0.14f, getLocalBounds().getBottomLeft().getX() + getWidth() * 0.215f, getLocalBounds().getBottomLeft().getY() - getHeight() * 0.2f);
}

void GranularSynthComponent::resized()
{
	//spec
	fftVisualizer.setBoundsRelative(0.6f, 0.48f, 0.39f, 0.5f);

	//grain vis
	grainsVisualizer.setBoundsRelative(0.6f, 0.04f, 0.39f, 0.4f);

	//buttons
	loadGrain.setBoundsRelative(0.02f, 0.9f, 0.16f, 0.08f);
	playAudio.setBoundsRelative(0.41f, 0.9f, 0.08f, 0.08f);
	stopAudio.setBoundsRelative(0.50f, 0.9f, 0.08f, 0.08f);

	//master volume slider
	masterVolume.setBoundsRelative(0.2f, 0.9f, 0.2f, 0.08f);

	//density slider
	densitySlider.setBoundsRelative(-0.01f, 0.575f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//window lenght slider
	windowLenghtSlider.setBoundsRelative(0.08f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//window position slider
	windowPositionSlider.setBoundsRelative(-0.01f, 0.2f, AppConstants::knobWidth, AppConstants::knobHeigth);

	randomPositionSlider.setBoundsRelative(0.08f, 0.575f, AppConstants::knobWidth, AppConstants::knobHeigth);

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

	randomPositionLabel.setBoundsRelative(0.135f, 0.50f, 0.2f, 0.1f);

	lengthLabel.setBoundsRelative(0.13f, 0.13f, 0.2f, 0.1f);

	cutoffLabel.setBoundsRelative(0.248f, 0.13f, 0.2f, 0.1f);

	qFactorLabel.setBoundsRelative(0.343f, 0.13f, 0.2f, 0.1f);

	gainLabel.setBoundsRelative(0.255f, 0.5f, 0.2f, 0.1f);

	filterTypeLabel.setBoundsRelative(0.351f, 0.5f, 0.2f, 0.1f);
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
		fftVisualizer.setEnabled(false);
		grainsVisualizer.setEnabled(true);
		break;

	case SynthState::Stopped:
		setButtonState(true, false, true, true);
		setSliderState(true);
		setFilterSliderState(false, false);
		fftVisualizer.setEnabled(false);
		grainsVisualizer.setEnabled(true);
		model.getWriteTime() = 0;
		model.getAudioState() = ModelAudioState::StopAudio;
		grainSelector.reset();
		model.getWriteGrainQueue().clear();
		break;

	case SynthState::Playing:
		setButtonState(false, true, false, true);
		setSliderState(true);
		setFilterSliderState(true, true);
		model.getAudioState() = ModelAudioState::grainPlay;
		grainSelector.start();
		model.getGrainCurrentIndex() = model.getGrainPosition();
		fftVisualizer.setEnabled(true);
		grainsVisualizer.setEnabled(true);
		break;
	case SynthState::Disable:
		setButtonState(false, false, true, false);
		setSliderState(false);
		setFilterSliderState(false, false);
		fftVisualizer.setEnabled(false);
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
	filterGainSlider.setEnabled(filterParam);
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
	model.getFilterGain() = filterGainSlider.getValue();
}

void GranularSynthComponent::grainSliderChanged()
{
	if (currentState != SynthState::Disable)
	{
		model.getGrainPosition() = windowPositionSlider.getValue();
		model.getGrainWindowLength() = windowLenghtSlider.getValue();
		model.getWriteDensity() = densitySlider.getValue();

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
