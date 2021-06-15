/*
  ==============================================================================

    ControllerTop.cpp
    Created: 20 Mar 2021 4:09:36pm
    Author:  Francesco

  ==============================================================================
*/

#include "ControllerTop.h"

SelectionComponent::SelectionComponent(Model& model, AudioLoader& audioLoader, GrainProcessing& grainProcessor)
	:model(model),
	audioLoader(audioLoader),
	grainProcessor(grainProcessor),
	grainVisualizer(model),
	waveformVisualizer(model),
	positionVisualizer(model),
	windowedGrainVisualizer(model)
{
	addAndMakeVisible(grainVisualizer);
	addAndMakeVisible(waveformVisualizer);
	addAndMakeVisible(positionVisualizer);
	addAndMakeVisible(windowedGrainVisualizer);
	
	changeCurrentState(SelectionState::Disabled);

	//disable all the visualizer
	waveformVisualizer.setEnabled(false);
	positionVisualizer.setEnabled(false);
	grainVisualizer.setEnabled(false);
	windowedGrainVisualizer.setEnabled(false);

	//labels
	addAndMakeVisible(waveformLabel);
	waveformLabel.setText("WAVEFORM", juce::NotificationType::dontSendNotification);
	waveformLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	waveformLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	addAndMakeVisible(grainLabel);
	grainLabel.setText("GRAIN", juce::NotificationType::dontSendNotification);
	grainLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	grainLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	addAndMakeVisible(windowedLabel);
	windowedLabel.setText("WINDOWED GRAIN", juce::NotificationType::dontSendNotification);
	windowedLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	windowedLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	addAndMakeVisible(processingLabel);
	processingLabel.setText("PROCESSING", juce::NotificationType::dontSendNotification);
	processingLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	processingLabel.setFont(juce::Font(13.0f, juce::Font::bold));

	//initialize buttons
	addAndMakeVisible(&openButton);
	openButton.setButtonText("OPEN");
	openButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	openButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	openButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	openButton.setLookAndFeel(&knobLookAndFeel);
	openButton.onClick = [this] { openButtonClicked(); };

	addAndMakeVisible(&playButton);
	playButton.setButtonText("PLAY");
	playButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	playButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	playButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	playButton.setLookAndFeel(&knobLookAndFeel);
	playButton.onClick = [this] { playButtonClicked(); };

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("STOP");
	stopButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	stopButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	stopButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	stopButton.setLookAndFeel(&knobLookAndFeel);
	stopButton.onClick = [this] { stopButtonClicked(); };

	addAndMakeVisible(&selectionButton);
	selectionButton.setButtonText("SELECT GRAIN");
	selectionButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	selectionButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	selectionButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	selectionButton.setLookAndFeel(&knobLookAndFeel);
	selectionButton.onClick = [this] { selectionButtonClicked(); };

	addAndMakeVisible(&saveButton);
	saveButton.setButtonText("SAVE GRAIN");
	saveButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	saveButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	saveButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	saveButton.setLookAndFeel(&knobLookAndFeel);
	saveButton.onClick = [this] { saveButtonClicked(); };

	addAndMakeVisible(&automaticSaveButton);
	automaticSaveButton.setButtonText("AUTOMATIC SAVE");
	automaticSaveButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	automaticSaveButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	automaticSaveButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	automaticSaveButton.setLookAndFeel(&knobLookAndFeel);
	automaticSaveButton.onClick = [this] { automaticGrainSelection(); };

	addAndMakeVisible(&grainLeft);
	grainLeft.setButtonText("L");
	grainLeft.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	grainLeft.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	grainLeft.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	grainLeft.setLookAndFeel(&knobLookAndFeel);
	grainLeft.onClick = [this] { changeWindowedChannel(0); };

	addAndMakeVisible(&grainRight);
	grainRight.setButtonText("R");
	grainRight.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	grainRight.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	grainRight.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	grainRight.setLookAndFeel(&knobLookAndFeel);
	grainRight.onClick = [this] {changeWindowedChannel(1); };

	//fade in out slider [s]
	addAndMakeVisible(fadeSlider);
	fadeSlider.setRange(0, 100, 1);
	fadeSlider.setLookAndFeel(&knobLookAndFeel);
	fadeSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	fadeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	fadeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	fadeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	fadeSlider.setTextBoxIsEditable(false);
	fadeSlider.onValueChange = [this] {fadeValueChanged(); };

	//grain lenght slider [s]
	addAndMakeVisible(grainLenghtSlider);
	grainLenghtSlider.setRange(1, 500, 0.5);
	grainLenghtSlider.setLookAndFeel(&knobLookAndFeel);
	grainLenghtSlider.setTextValueSuffix(" [ms]");
	grainLenghtSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	grainLenghtSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	grainLenghtSlider.setColour(juce::Slider::backgroundColourId, AppColours::waveformBackground);
	grainLenghtSlider.setColour(juce::Slider::trackColourId, AppColours::buttons);
	grainLenghtSlider.setTextBoxIsEditable(false);
	grainLenghtSlider.setValue(100);
	grainLenghtSlider.onValueChange = [this] { grainLenghtChanged(); };

	//automatic selection slider [num grain]
	addAndMakeVisible(automaticSelectionSlider);
	automaticSelectionSlider.setRange(0, 100, 1);
	automaticSelectionSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	automaticSelectionSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	automaticSelectionSlider.setColour(juce::Slider::backgroundColourId, AppColours::waveformBackground);
	automaticSelectionSlider.setColour(juce::Slider::trackColourId, AppColours::buttons);
	automaticSelectionSlider.setTextBoxIsEditable(false);
	automaticSelectionSlider.onValueChange = [this] { automaticValueChanged(); };

	//add items to the combo box 
	addAndMakeVisible(windowsMenu);
	windowsMenu.addItem("rect", 1);
	windowsMenu.addItem("hann", 2);
	windowsMenu.addItem("hamming", 3);
	windowsMenu.addItem("triangular", 4);
	windowsMenu.addItem("blackman", 5);

	windowsMenu.setColour(juce::ComboBox::backgroundColourId, AppColours::waveformBackground);
	windowsMenu.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, AppColours::waveformBackground);
	windowsMenu.onChange = [this] {windowMenuChanged(); };
	windowsMenu.setSelectedId(1);

}

void SelectionComponent::paint(juce::Graphics& g)
{
	//border 
    g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX(), getLocalBounds().getBottomLeft().getY(),
		getLocalBounds().getBottomRight().getX(), getLocalBounds().getBottomRight().getY(), 3);
}

void SelectionComponent::resized()
{
	waveformVisualizer.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
	positionVisualizer.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);

	//grain display
    grainVisualizer.setBoundsRelative(0.02f, 0.6f, 0.25f, 0.35f);

	//windowed grain display
	windowedGrainVisualizer.setBoundsRelative(0.52f, 0.6f, 0.25f, 0.35f);

	//buttons sizes
	openButton.setBoundsRelative(0.79f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	playButton.setBoundsRelative(0.86f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	stopButton.setBoundsRelative(0.93f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	selectionButton.setBoundsRelative(0.79f, 0.35f, 0.20f, 0.08f);
	saveButton.setBoundsRelative(0.79f, 0.60f, 0.2f, 0.08f);
	automaticSaveButton.setBoundsRelative(0.79f, 0.85, 0.2f, 0.08f);

	//slider
	grainLenghtSlider.setBoundsRelative(0.79f, 0.2f, 0.2f, 0.15f);
	fadeSlider.setBoundsRelative(0.39f, 0.62f, AppConstants::knobFadeWidth, AppConstants::knobFadeHeigth);

	//windows menu display 
	windowsMenu.setBoundsRelative(0.32f, 0.87f, 0.15f, 0.08f);;

	//automatic selection
	automaticSelectionSlider.setBoundsRelative(0.79f, 0.75f, 0.2f, 0.08f);

	//show left or right windowed grain
	grainLeft.setBoundsRelative(0.34f, 0.67f, 0.02f, 0.05f);
	grainRight.setBoundsRelative(0.37f, 0.67f, 0.02f, 0.05f);

	//labels 
	waveformLabel.setBoundsRelative(0.02f, -0.03f, 0.1f, 0.1f);
	grainLabel.setBoundsRelative(0.02f, 0.52f, 0.1f, 0.1f);
	windowedLabel.setBoundsRelative(0.52f, 0.52f, 0.1f, 0.1f);
	processingLabel.setBoundsRelative(0.36f, 0.52f, 0.1f, 0.1f);
}

//used to change the state of the component
void SelectionComponent::changeCurrentState(SelectionState newState)
{
	if (currentState != SelectionState::Disabled || newState == SelectionState::Loaded)
		currentState = newState;

	switch (currentState)
	{
	case SelectionState::Disabled:
		setButtonsEnable(true, false, false, false, false);
		setSliderEnable(false, false, false, false);
		grainVisualizer.setEnabled(false);
		windowedGrainVisualizer.setEnabled(false);
		break;
	case SelectionState::Loaded:
		setButtonsEnable(true, true, false, true, false);
		setSliderEnable(true, false, false, true);
		grainVisualizer.setEnabled(false);
		windowedGrainVisualizer.setEnabled(false);
		break;
	case SelectionState::Stopped:
		setButtonsEnable(true, true, false, true, false);
		setSliderEnable(true, false, false, true);
		grainVisualizer.setEnabled(false);
		windowedGrainVisualizer.setEnabled(false);
		break;
	case SelectionState::Playing:
		setButtonsEnable(false, false, true, false, false);
		setSliderEnable(false, false, false, false);
		grainVisualizer.setEnabled(false);
		windowedGrainVisualizer.setEnabled(false);
		break;
	case SelectionState::SelectGrain:
		setButtonsEnable(true, true, false, true, true);
		setSliderEnable(true, true, true, true);
		grainVisualizer.setEnabled(true);
		windowedGrainVisualizer.setEnabled(true);
		break;
	default:
		break;
	}
}

void SelectionComponent::setButtonsEnable(bool enableOpen, bool enablePlay, bool enableStop, bool enableSelect, bool enableSave)
{
	openButton.setEnabled(enableOpen);
	playButton.setEnabled(enablePlay);
	stopButton.setEnabled(enableStop);
	selectionButton.setEnabled(enableSelect);
	saveButton.setEnabled(enableSave);
	automaticSaveButton.setEnabled(enableSave);
	grainLeft.setEnabled(enableSave);
	grainRight.setEnabled(enableSave);
}

void SelectionComponent::setSliderEnable(bool enableWlenght, bool enableFade, bool enableWselection, bool enableAuto)
{
	grainLenghtSlider.setEnabled(enableWlenght);
	fadeSlider.setEnabled(enableFade);
	windowsMenu.setEnabled(enableWselection);
	automaticSelectionSlider.setEnabled(enableAuto);
}

//-------------------------- buttons actions -----------------------------------

void SelectionComponent::openButtonClicked()
{
	audioLoader.loadInputFile();
	waveformVisualizer.setEnabled(true);
	positionVisualizer.setEnabled(true);

	changeCurrentState(SelectionState::Loaded);
}

void SelectionComponent::playButtonClicked()
{
	model.getTransportSource().start();
	model.getAudioState() = ModelAudioState::selectionPlay;

	changeCurrentState(SelectionState::Playing);
}

void SelectionComponent::stopButtonClicked()
{
	model.getTransportSource().stop();
	model.getAudioState() = ModelAudioState::StopAudio;

	changeCurrentState(SelectionState::Stopped);
}

void SelectionComponent::saveButtonClicked()
{
	grainProcessor.saveGrain();
}

void SelectionComponent::selectionButtonClicked()
{
	grainProcessor.selectCurrentGrain();
	grainVisualizer.setEnabled(true);

	grainProcessor.computeWindowOutput();
	windowedGrainVisualizer.setEnabled(true);

	changeCurrentState(SelectionState::SelectGrain);
}

void SelectionComponent::grainLenghtChanged()
{
	model.getWriteGrainLength() = grainLenghtSlider.getValue()/ 1000.0f;
	fadeSlider.setRange(0, int(model.getWriteGrainLength() * 1000), 1);
}

void SelectionComponent::windowMenuChanged()
{
	switch (windowsMenu.getSelectedId())
	{
	case 1: model.getCurrentWindowMethod() = juce::dsp::WindowingFunction<float>::rectangular; break;
	case 2: model.getCurrentWindowMethod() = juce::dsp::WindowingFunction<float>::hann; break;
	case 3: model.getCurrentWindowMethod() = juce::dsp::WindowingFunction<float>::hamming; break;
	case 4: model.getCurrentWindowMethod() = juce::dsp::WindowingFunction<float>::triangular; break;
	case 5: model.getCurrentWindowMethod() = juce::dsp::WindowingFunction<float>::blackman; break;
	default:
		break;
	}
	
	if (currentState == SelectionState::SelectGrain)
	{
		grainProcessor.computeWindowOutput();
		windowedGrainVisualizer.setEnabled(true);
	}
}

void SelectionComponent::changeWindowedChannel(int channel)
{
	windowedGrainVisualizer.setChannel(channel);
	windowedGrainVisualizer.setEnabled(true);
}

void SelectionComponent::automaticValueChanged()
{
	model.getAutomaticSelValue() = automaticSelectionSlider.getValue();
}

void SelectionComponent::automaticGrainSelection() 
{
	auto maxLenght = int((model.getTransportSource().getLengthInSeconds() - model.getTransportSource().getCurrentPosition() - model.getWriteGrainLength()) / model.getWriteGrainLength());

	for (int i = 0; i < juce::jmin(model.getAutomaticSelValue().load(), maxLenght); i++)
	{
		selectionButtonClicked();
		grainProcessor.saveGrain();
	}
}

void SelectionComponent::fadeValueChanged()
{
	model.getWriteFade() = fadeSlider.getValue() / 1000.0f;
	grainProcessor.computeWindowOutput();
	windowedGrainVisualizer.setEnabled(true);
}
