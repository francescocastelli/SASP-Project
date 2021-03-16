/*
  ==============================================================================

    SelectionComponent.cpp
    Created: 10 Mar 2021 10:04:24pm
    Author:  Francesco

  ==============================================================================
*/

#include "SelectionComponent.h"

SelectionComponent::SelectionComponent(juce::AudioTransportSource& transportSourceRef, juce::File &sampleDir)
		: thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache),
        thumbnailComp(thumbnail),
	    specComp(),
        transportSource(transportSourceRef),
        positionComp(transportSource),
        displayGrain(thumbnail),
        sampleDir(sampleDir),
	    sampleRate(0),
		currentGrainLenght (AppConstants::maxgrainlengthsec),
	    grainProcessing(sampleDir)
{
	//initialize buttons
	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open");
	openButton.onClick = [this] { openButtonClicked(); };
      
	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] { playButtonClicked(); };
	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playButton.setEnabled(false);
        
	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopButton.setEnabled(false);

	addAndMakeVisible(&selectionButton);
	selectionButton.setButtonText("Select grain");
	selectionButton.onClick = [this] { selectionButtonClicked(); };
	selectionButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	selectionButton.setEnabled(false);

	addAndMakeVisible(&saveButton);
	saveButton.setButtonText("Save grain");
	saveButton.onClick = [this] { saveButtonClicked(); };
	saveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	saveButton.setEnabled(false);

	addAndMakeVisible(&automaticSaveButton);
	automaticSaveButton.setButtonText("Automatic");
	automaticSaveButton.onClick = [this] { /*automaticGrainSelection(); */};
	automaticSaveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
	automaticSaveButton.setEnabled(false);

	//grain lenght slider
	addAndMakeVisible(grainLenghtSlider);
	grainLenghtSlider.setRange(1, 100, 0.5);
	grainLenghtSlider.setTextValueSuffix(" [ms]");
	grainLenghtSlider.setTextBoxIsEditable(false);
	grainLenghtSlider.setValue(100);
	grainLenghtSlider.onValueChange = [this] {	currentGrainLenght = grainLenghtSlider.getValue()/1000; positionComp.setWindowLenght(currentGrainLenght); };

	//fade in out slider
	addAndMakeVisible(fadeSlider);
	fadeSlider.setRange(0, 1000, 1);
	fadeSlider.setTextValueSuffix(" [samples]");
	fadeSlider.setTextBoxIsEditable(false);
	fadeSlider.setValue(0);
	fadeSlider.onValueChange = [this] { grainProcessing.setFadeValue(fadeSlider.getValue()); };
	
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
	windowsMenu.onChange = [this] {grainProcessing.setGrainLenght(currentGrainLenght * sampleRate); grainProcessing.windowMenuChanged(windowsMenu.getSelectedId()); };

	//inizialize format for the format manager
	formatManager.registerBasicFormats();

	//register actions listeners
	addActionListener(&grainProcessing);
	addActionListener(&displayGrain);
}

void SelectionComponent::setButtonsEnable(bool enablePlay, bool enableStop, bool enableSelect)
{
	playButton.setEnabled(enablePlay);
	stopButton.setEnabled(enableStop);
	selectionButton.setEnabled(enableSelect);
	saveButton.setEnabled(enableSelect);
	automaticSaveButton.setEnabled(enableSelect);
}

void SelectionComponent::resized() 
{
	//buttons sizes
	openButton.setBoundsRelative(0.79f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	playButton.setBoundsRelative(0.86f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	stopButton.setBoundsRelative(0.93f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	selectionButton.setBoundsRelative(0.79f, 0.35f, 0.20f, 0.08f);
	saveButton.setBoundsRelative(0.79f, 0.7f, 0.2f, 0.08f);
	automaticSaveButton.setBoundsRelative(0.79f, 0.85, 0.2f, 0.08f);

	//waveform display 
	thumbnailComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
	positionComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
        
	//grain display
    displayGrain.setBoundsRelative(0.02f, 0.6f, 0.25f, 0.35f);

	//slider
	grainLenghtSlider.setBoundsRelative(0.79f, 0.2f, 0.2f, 0.15f);
	fadeSlider.setBoundsRelative(0.32f, 0.65f, 0.15f, 0.15f);

	//grain processing display
	grainProcessing.setBoundsRelative(0.52f, 0.6f, 0.25f, 0.35f);

	//spec display 
	//specComp.setBoundsRelative(0.30f, 0.53f, 0.25f, 0.35f);

	//windows menu display 
	windowsMenu.setBoundsRelative(0.32f, 0.84f, 0.15f, 0.08f);;
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

void SelectionComponent::openButtonClicked()
{
    juce::FileChooser chooser("Select a Wave file to play...", {}, "*.wav");                                     
    
	if (chooser.browseForFileToOpen())                                          
	{
		auto file = chooser.getResult();                                        
		auto* reader = formatManager.createReaderFor(file);                    
		sampleRate = reader->sampleRate;
		//inform the listener that the sample rate is changed
		grainProcessing.setSampleRate(sampleRate);

		if (reader != nullptr)
		{
			std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true)); 
			transportSource.setSource(newSource.get(), 0, nullptr, sampleRate);                                
			thumbnail.setSource(new juce::FileInputSource(file));
						readerSource.reset(newSource.release());     
			state = SoundState::Stopped;
			sendChangeMessage();

			//also deactivate the grain component and relative buttons
			sendActionMessage("deactivateGrain");
			//save button is active when selection is active
			setButtonsEnable(true, false, true);
		}
    }
}
    
void SelectionComponent::playButtonClicked()
{
    state = SoundState::Starting;
	sendChangeMessage();
}
    
void SelectionComponent::stopButtonClicked()
{
    state = SoundState::Stopping;
	sendChangeMessage();
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
	
	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 
	transportSource.setPosition(startTime);
	transportSource.start();
	transportSource.getNextAudioBlock(audioChannelInfo);
	transportSource.stop();
	//reset the position to the previous one 
	transportSource.setPosition(currentTime+currentGrainLenght);

	//activate the grain components only when a grain is selected
	sendActionMessage("activateGrain");
	displayGrain.setTime(startTime, startTime + currentGrainLenght);
	//and compute windowing operation
	grainProcessing.applyWindow(buffer);

	//activate the save grain button
	setButtonsEnable(true, false, true);
}
    
void SelectionComponent::automaticGrainSelection()
{
	auto currentTime = 0.10f;

	for (int i = 0; i < 100; i++)
	{
		juce::AudioBuffer<float> buffer(2, currentGrainLenght*sampleRate);
		juce::AudioSourceChannelInfo audioChannelInfo(buffer);
		buffer.clear();
		auto startTime = (currentTime - currentGrainLenght/2) < 0 ? 0 : currentTime - currentGrainLenght/2;
	
		transportSource.setPosition(startTime);
		transportSource.start();
		transportSource.getNextAudioBlock(audioChannelInfo);
		transportSource.stop();
		transportSource.setPosition(currentTime+currentGrainLenght);

		//activate the grain components only when a grain is selected
		sendActionMessage("activateGrain");
		grainProcessing.applyWindow(buffer);
		currentTime += currentGrainLenght;
		saveButtonClicked();
	}
}

void SelectionComponent::saveButtonClicked()
{
	sendActionMessage("saveGrain");
}

SoundState SelectionComponent::getSoundState()
{
	return state;
}
