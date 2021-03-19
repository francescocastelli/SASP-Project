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
	    grainProcessing(sampleDir, currentGrainLenght, currentFadeValue, sampleRate)
{
	//initialize buttons
	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open");
	openButton.onClick = [this] { openButtonClicked(); };
      
	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] { transportSource.start(); changeCurrentState(SelectionState::Playing); };
	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playButton.setEnabled(false);
        
	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { transportSource.stop(); changeCurrentState(SelectionState::Stopped); };
	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopButton.setEnabled(false);

	addAndMakeVisible(&selectionButton);
	selectionButton.setButtonText("Select grain");
	selectionButton.onClick = [this] { selectionButtonClicked(); changeCurrentState(SelectionState::SelectGrain); };
	selectionButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	selectionButton.setEnabled(false);

	addAndMakeVisible(&saveButton);
	saveButton.setButtonText("Save grain");
	saveButton.onClick = [this] { grainProcessing.saveGrain(); };
	saveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	saveButton.setEnabled(false);

	addAndMakeVisible(&automaticSaveButton);
	automaticSaveButton.setButtonText("Automatic");
	automaticSaveButton.onClick = [this] { automaticGrainSelection(); };
	automaticSaveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	automaticSaveButton.setEnabled(false);

	//fade in out slider [s]
	addAndMakeVisible(fadeSlider);
	fadeSlider.setRange(0, 100, 1);
	fadeSlider.setTextValueSuffix(" [ms]");
	fadeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 15);
	fadeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	fadeSlider.setTextBoxIsEditable(false);
	fadeSlider.onValueChange = [this] { currentFadeValue = fadeSlider.getValue() / 1000.0f; grainProcessing.computeWindowOutput();};

	//grain lenght slider [s]
	addAndMakeVisible(grainLenghtSlider);
	grainLenghtSlider.setRange(1, 500, 0.5);
	grainLenghtSlider.setTextValueSuffix(" [ms]");
	grainLenghtSlider.setTextBoxIsEditable(false);
	grainLenghtSlider.setValue(100);
	grainLenghtSlider.onValueChange = [this] {grainLenghtChanged();};
	
	//automatic selection slider [num grain]
	addAndMakeVisible(automaticSelectionSlider);
	automaticSelectionSlider.setRange(0, 100, 1);
	automaticSelectionSlider.setTextBoxIsEditable(false);
	automaticSelectionSlider.onValueChange = [this] { automaticSelectionVal = automaticSelectionSlider.getValue();};

	//add and make visible internal components
	addAndMakeVisible(thumbnailComp);
	addAndMakeVisible(positionComp);
	addAndMakeVisible(displayGrain);
	//addAndMakeVisible(specComp);
	addAndMakeVisible(windowsMenu);
	addAndMakeVisible(grainProcessing);

	//add items to the combo box 
	windowsMenu.addItem("none", 1);
	windowsMenu.addItem("rect", 2);
	windowsMenu.addItem("hann", 3);
	windowsMenu.addItem("hamming", 4);
	windowsMenu.addItem("triangular", 5);
	windowsMenu.addItem("blackman", 6);

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
	//temp buffer used for getting the sample to store in the wav
	//the lenght of the buffer is the grain lenght
	juce::AudioBuffer<float> buffer(2, currentGrainLenght*sampleRate);
	//clear the buffer
	buffer.clear();
	//create the audio channel info that is required from the getNextAudioBlock
	juce::AudioSourceChannelInfo audioChannelInfo(buffer);

    auto currentTime = transportSource.getCurrentPosition();
	auto startTime = (currentTime - currentGrainLenght/2) < 0 ? 0 : currentTime - currentGrainLenght/2;
	auto currentGain = transportSource.getGain();
	
	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 
	transportSource.setPosition(startTime);
	transportSource.setGain(0);
	transportSource.start();
	transportSource.getNextAudioBlock(audioChannelInfo);
	transportSource.stop();
	transportSource.setGain(currentGain);
	//reset the position to the previous one 
	transportSource.setPosition(currentTime+currentGrainLenght);

	displayGrain.setTime(startTime, startTime + currentGrainLenght);
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
	fadeSlider.setBoundsRelative(0.20f, 0.55f, 0.3f, 0.3f);

	//grain processing display
	grainProcessing.setBoundsRelative(0.52f, 0.6f, 0.25f, 0.35f);

	//windows menu display 
	windowsMenu.setBoundsRelative(0.32f, 0.87f, 0.15f, 0.08f);;

	automaticSelectionSlider.setBoundsRelative(0.79f, 0.75f, 0.2f, 0.08f);
}

void SelectionComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::boxText);
	g.setFont(juce::Font(13, 1));
	g.drawSingleLineText("Waveform", 22, 12);
	g.drawSingleLineText("Selected Grain", 22, 168);
	g.drawSingleLineText("Windowed Grain", 605, 168);

	//border 
    g.setColour(AppColours::waveformBorder);
	g.drawLine(getLocalBounds().getBottomLeft().getX(), getLocalBounds().getBottomLeft().getY(),
		getLocalBounds().getBottomRight().getX(), getLocalBounds().getBottomRight().getY(), 3);
}

