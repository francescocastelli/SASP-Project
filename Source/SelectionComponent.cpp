/*
  ==============================================================================

    SelectionComponent.cpp
    Created: 10 Mar 2021 10:04:24pm
    Author:  Francesco

  ==============================================================================
*/
#include "SelectionComponent.h"

SelectionComponent::SelectionComponent(juce::File &sampleDir)
		: thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache),
        thumbnailComp(thumbnail),
        positionComp(transportSource, currentGrainLenght),
        displayGrain(thumbnail),
        sampleDir(sampleDir),
	    sampleRate(0),
		currentFadeValue (0),
		automaticSelectionVal(0),
		currentGrainLenght (0.001),
	    grainProcessing(sampleDir, currentGrainLenght, currentFadeValue, sampleRate, showLeft)
{
	//labels
	addAndMakeVisible(waveformLabel);
	waveformLabel.setText("WAVEFORM", juce::NotificationType::dontSendNotification);
	waveformLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	waveformLabel.setFont(juce::Font (13.0f, juce::Font::bold));

	addAndMakeVisible(grainLabel);
	grainLabel.setText("GRAIN", juce::NotificationType::dontSendNotification);
	grainLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	grainLabel.setFont(juce::Font (13.0f, juce::Font::bold));

	addAndMakeVisible(windowLabel);
	windowLabel.setText("WINDOWED GRAIN", juce::NotificationType::dontSendNotification);
	windowLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	windowLabel.setFont(juce::Font (13.0f, juce::Font::bold));

	addAndMakeVisible(fadeLabel);
	fadeLabel.setText("PROCESSING", juce::NotificationType::dontSendNotification);
	fadeLabel.setColour(juce::Label::textColourId, AppColours::boxText);
	fadeLabel.setFont(juce::Font (13.0f, juce::Font::bold));

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
	playButton.onClick = [this] { transportSource.start(); changeCurrentState(SelectionState::Playing); };
	playButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	playButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	playButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	playButton.setLookAndFeel(&knobLookAndFeel);
	playButton.setEnabled(false);
        
	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("STOP");
	stopButton.onClick = [this] { transportSource.stop(); changeCurrentState(SelectionState::Stopped); };
	stopButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	stopButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	stopButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	stopButton.setLookAndFeel(&knobLookAndFeel);
	stopButton.setEnabled(false);

	addAndMakeVisible(&selectionButton);
	selectionButton.setButtonText("SELECT GRAIN");
	selectionButton.onClick = [this] { selectionButtonClicked(); changeCurrentState(SelectionState::SelectGrain); };
	selectionButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	selectionButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	selectionButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	selectionButton.setLookAndFeel(&knobLookAndFeel);
	selectionButton.setEnabled(false);

	addAndMakeVisible(&saveButton);
	saveButton.setButtonText("SAVE GRAIN");
	saveButton.onClick = [this] { grainProcessing.saveGrain(); };
	saveButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	saveButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	saveButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	saveButton.setLookAndFeel(&knobLookAndFeel);
	saveButton.setEnabled(false);

	addAndMakeVisible(&automaticSaveButton);
	automaticSaveButton.setButtonText("AUTOMATIC SAVE");
	automaticSaveButton.onClick = [this] { automaticGrainSelection(); };
	automaticSaveButton.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	automaticSaveButton.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	automaticSaveButton.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	automaticSaveButton.setLookAndFeel(&knobLookAndFeel);
	automaticSaveButton.setEnabled(false);

	addAndMakeVisible(&grainLeft);
	grainLeft.setButtonText("L");
	grainLeft.onClick = [this] { showLeft = true; grainProcessing.computeWindowOutput(); };
	grainLeft.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	grainLeft.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	grainLeft.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	grainLeft.setLookAndFeel(&knobLookAndFeel);
	grainLeft.setEnabled(false);

	addAndMakeVisible(&grainRight);
	grainRight.setButtonText("R");
	grainRight.onClick = [this] { showLeft = false; grainProcessing.computeWindowOutput(); };
	grainRight.setColour(juce::TextButton::buttonColourId, AppColours::buttons);
	grainRight.setColour(juce::TextButton::textColourOffId, AppColours::buttonsText);
	grainRight.setColour(juce::TextButton::textColourOnId, AppColours::buttonsText);
	grainRight.setLookAndFeel(&knobLookAndFeel);
	grainRight.setEnabled(false);

	//fade in out slider [s]
	addAndMakeVisible(fadeSlider);
	fadeSlider.setRange(0, 100, 1);
	fadeSlider.setLookAndFeel(&knobLookAndFeel);
	fadeSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	fadeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	fadeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 30, 20);
	fadeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	fadeSlider.setTextBoxIsEditable(false);
	fadeSlider.onValueChange = [this] { currentFadeValue = fadeSlider.getValue() / 1000.0f; grainProcessing.computeWindowOutput();};

	//grain lenght slider [s]
	addAndMakeVisible(grainLenghtSlider);
	grainLenghtSlider.setRange(1, 500, 0.5);
	grainLenghtSlider.setTextValueSuffix(" [ms]");
	grainLenghtSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	grainLenghtSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	grainLenghtSlider.setColour(juce::Slider::backgroundColourId, AppColours::waveformBackground);
	grainLenghtSlider.setColour(juce::Slider::trackColourId, AppColours::buttons);
	grainLenghtSlider.setTextBoxIsEditable(false);
	grainLenghtSlider.setValue(100);
	grainLenghtSlider.onValueChange = [this] {grainLenghtChanged();};
	
	//automatic selection slider [num grain]
	addAndMakeVisible(automaticSelectionSlider);
	automaticSelectionSlider.setRange(0, 100, 1);
	automaticSelectionSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
	automaticSelectionSlider.setColour(juce::Slider::textBoxTextColourId, AppColours::knobText);
	automaticSelectionSlider.setColour(juce::Slider::backgroundColourId, AppColours::waveformBackground);
	automaticSelectionSlider.setColour(juce::Slider::trackColourId, AppColours::buttons);
	automaticSelectionSlider.setTextBoxIsEditable(false);
	automaticSelectionSlider.onValueChange = [this] { automaticSelectionVal = automaticSelectionSlider.getValue();};

	//add and make visible internal components
	addAndMakeVisible(thumbnailComp);
	addAndMakeVisible(positionComp);
	addAndMakeVisible(displayGrain);
	addAndMakeVisible(windowsMenu);
	addAndMakeVisible(grainProcessing);

	//add items to the combo box 
	windowsMenu.addItem("none", 1);
	windowsMenu.addItem("rect", 2);
	windowsMenu.addItem("hann", 3);
	windowsMenu.addItem("hamming", 4);
	windowsMenu.addItem("triangular", 5);
	windowsMenu.addItem("blackman", 6);

	windowsMenu.setColour(juce::ComboBox::backgroundColourId, AppColours::waveformBackground);
	windowsMenu.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, AppColours::waveformBackground);
	windowsMenu.setSelectedId(1);
	windowsMenu.onChange = [this] { grainProcessing.windowMenuChanged(windowsMenu.getSelectedId()); };

	//inizialize format for the format manager
	formatManager.registerBasicFormats();

	//register as listener of the transport source 
	transportSource.addChangeListener(this);

	//set the current state at creation 
	changeCurrentState(SelectionState::Disabled);
}

//--------------------------- component state -------------------------------------------

void SelectionComponent::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
	if (source == &transportSource && transportSource.hasStreamFinished())
		changeCurrentState(SelectionState::Loaded);
}

//used to change the state of the component
void SelectionComponent::changeCurrentState(SelectionState newState)
{
	if ( currentState != SelectionState::Disabled || newState == SelectionState::Loaded) 
		currentState = newState;

	switch (currentState)
	{
	case SelectionState::Disabled:
		setButtonsEnable(true, false, false, false, false);
		setSliderEnable(false, false, false, false);
		displayGrain.setEnable(false);
		grainProcessing.setEnable(false);
		sendChangeMessage();
		break;
	case SelectionState::Loaded:
		setButtonsEnable(true, true, false, true, false);
		setSliderEnable(true, false, false, true);
		displayGrain.setEnable(false);
		grainProcessing.setEnable(false);
		sendChangeMessage();
		break;
	case SelectionState::Stopped:
		setButtonsEnable(true, true, false, true, false);
		setSliderEnable(true, false, false, true);
		displayGrain.setEnable(false);
		grainProcessing.setEnable(false);
		sendChangeMessage();
		break;
	case SelectionState::Playing:
		setButtonsEnable(false, false, true, false, false);
		setSliderEnable(false, false, false, false);
		displayGrain.setEnable(false);
		grainProcessing.setEnable(false);
		sendChangeMessage();
		break;
	case SelectionState::SelectGrain:
		setButtonsEnable(true, true, false, true, true);
		setSliderEnable(true, true, true, true);
		displayGrain.setEnable(true);
		grainProcessing.setEnable(true);
		sendChangeMessage();
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

SelectionState SelectionComponent::getCurrentState()
{
	return currentState;
}
//---------------------------------------- audio play -------------------------------------------------------------

void SelectionComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SelectionComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	transportSource.getNextAudioBlock(bufferToFill);
}

void SelectionComponent::releaseResources()
{
	transportSource.releaseResources();
}

//--------------------------------------- buttons ------------------------------------------------------

void SelectionComponent::openButtonClicked()
{
    juce::FileChooser chooser("Select a Wave file to play...", {}, "*.wav");                                     
    
	if (chooser.browseForFileToOpen())                                          
	{
		auto file = chooser.getResult();                                        
		auto* reader = formatManager.createReaderFor(file);                    
		sampleRate = reader->sampleRate;

		if (reader != nullptr)
		{
			std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true)); 
			transportSource.setSource(newSource.get(), 0, nullptr, sampleRate);                                
			thumbnail.setSource(new juce::FileInputSource(file));
			readerSource.reset(newSource.release());     

			changeCurrentState(SelectionState::Loaded);
		}
    }
}
    
void SelectionComponent::selectionButtonClicked()
{
    auto currentTime = transportSource.getCurrentPosition();
	auto startTime = (currentTime - currentGrainLenght/2) < 0 ? 0 : currentTime - currentGrainLenght/2;
	auto endTime = (startTime + currentGrainLenght >= transportSource.getLengthInSeconds()) ? transportSource.getLengthInSeconds() : startTime + currentGrainLenght;

	//temp buffer used for getting the sample to store in the wav
	//the lenght of the buffer is the grain lenght
	juce::AudioBuffer<float> buffer(2, (endTime - startTime)*sampleRate);
	//clear the buffer
	buffer.clear();
	//create the audio channel info that is required from the getNextAudioBlock
	juce::AudioSourceChannelInfo audioChannelInfo(buffer);

	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 
	transportSource.setPosition(startTime);
	transportSource.start();
	transportSource.getNextAudioBlock(audioChannelInfo);
	transportSource.stop();
	//reset the position to the previous one 
	transportSource.setPosition(endTime);

	displayGrain.setTime(startTime, startTime + (endTime-startTime));
	//and compute windowing operation
	grainProcessing.applyWindow(buffer);
}
    
void SelectionComponent::grainLenghtChanged()
{
	currentGrainLenght = grainLenghtSlider.getValue() / 1000.0f; 
	fadeSlider.setRange(0, int(currentGrainLenght * 1000), 1); 
	changeCurrentState(SelectionState::Stopped); 
}

void SelectionComponent::automaticGrainSelection()
{
	auto maxLenght = int((transportSource.getLengthInSeconds() - transportSource.getCurrentPosition() - currentGrainLenght) / currentGrainLenght );

	for (int i = 0; i < juce::jmin(automaticSelectionVal, maxLenght); i++)
	{
		selectionButtonClicked();
		grainProcessing.saveGrain();
	}
}

//---------------------------------------- gui --------------------------------------------

void SelectionComponent::resized() 
{
	//buttons sizes
	openButton.setBoundsRelative(0.79f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	playButton.setBoundsRelative(0.86f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	stopButton.setBoundsRelative(0.93f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	selectionButton.setBoundsRelative(0.79f, 0.35f, 0.20f, 0.08f);
	saveButton.setBoundsRelative(0.79f, 0.60f, 0.2f, 0.08f);
	automaticSaveButton.setBoundsRelative(0.79f, 0.85, 0.2f, 0.08f);

	//waveform display 
	thumbnailComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
	positionComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
        
	//grain display
    displayGrain.setBoundsRelative(0.02f, 0.6f, 0.25f, 0.35f);

	//slider
	grainLenghtSlider.setBoundsRelative(0.79f, 0.2f, 0.2f, 0.15f);
	fadeSlider.setBoundsRelative(0.26f, 0.55f, AppConstants::knobWidth, AppConstants::knobHeigth);

	//grain processing display
	grainProcessing.setBoundsRelative(0.52f, 0.6f, 0.25f, 0.35f);

	//windows menu display 
	windowsMenu.setBoundsRelative(0.32f, 0.87f, 0.15f, 0.08f);;

	//automatic selection
	automaticSelectionSlider.setBoundsRelative(0.79f, 0.75f, 0.2f, 0.08f);

	//show left or right windowed grain
	grainLeft.setBoundsRelative(0.495f, 0.71f, 0.02f, 0.05f);
	grainRight.setBoundsRelative(0.495f, 0.78f, 0.02f, 0.05f);

	//labels 
	waveformLabel.setBoundsRelative(0.02f, -0.03f, 0.1f, 0.1f);
	grainLabel.setBoundsRelative(0.02f, 0.52f, 0.1f, 0.1f);
	windowLabel.setBoundsRelative(0.52f, 0.52f, 0.1f, 0.1f);
	fadeLabel.setBoundsRelative(0.37f, 0.52f, 0.1f, 0.1f);
}

void SelectionComponent::paint(juce::Graphics& g)
{
	//border 
    g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX(), getLocalBounds().getBottomLeft().getY(),
		getLocalBounds().getBottomRight().getX(), getLocalBounds().getBottomRight().getY(), 3);
}

