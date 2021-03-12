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
	windowsMenu.onChange = [this] {grainProcessing.windowMenuChanged(windowsMenu.getSelectedId());};

	//inizialize format for the format manager
	formatManager.registerBasicFormats();

	//register actions listeners
	addActionListener(&grainProcessing);
	addActionListener(&displayGrain);
}

void SelectionComponent::resized() 
{
	//buttons sizes
	openButton.setBoundsRelative(0.79f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	playButton.setBoundsRelative(0.86f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	stopButton.setBoundsRelative(0.93f, AppConstants::controlButtonsY, AppConstants::controlButtonWidth, AppConstants::controlButtonHeight);
	selectionButton.setBoundsRelative(0.79f, 0.18f, 0.20f, 0.08f);
	saveButton.setBoundsRelative(0.79f, 0.65f, 0.2f, 0.08f);

	//waveform display 
	thumbnailComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
	positionComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
        
	//grain display
    displayGrain.setBoundsRelative(0.02f, 0.53f, 0.25f, 0.35f);

	//grain processing display
	grainProcessing.setBoundsRelative(0.52f, 0.53f, 0.25f, 0.35f);

	//spec display 
	//specComp.setBoundsRelative(0.30f, 0.53f, 0.25f, 0.35f);

	//windows menu display 
	windowsMenu.setBoundsRelative(0.32f, 0.80f, 0.15f, 0.08f);;
}

void SelectionComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::boxText);
	g.setFont(juce::Font(13, 1));
	g.drawSingleLineText("Waveform", 22, 12);
	g.drawSingleLineText("Selected Grain", 22, 170);
	g.drawSingleLineText("Windowed Grain", 605, 170);

	//border 
    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}


void SelectionComponent::grainSelection(double start, double end, juce::Rectangle<int> bounds)
{
//thumbnailComp.grainSelection(start, end, bounds);
}

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
			playButton.setEnabled(true);          
			selectionButton.setEnabled(true);
			readerSource.reset(newSource.release());     
			state = SoundState::Stopped;
			sendChangeMessage();

			//also deactivate the grain component and relative buttons
			sendActionMessage("deactivateGrain");
			saveButton.setEnabled(false);
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
	juce::AudioBuffer<float> buffer(2, GRAINLENGTH*sampleRate);
	//clear the buffer
	buffer.clear();
	//create the audio channel info that is required from the getNextAudioBlock
	juce::AudioSourceChannelInfo audioChannelInfo(buffer);

    auto currentTime = transportSource.getCurrentPosition();
	auto startTime = (currentTime - GRAINLENGTH/2) < 0 ? 0 : currentTime - GRAINLENGTH/2;
	
	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 
	transportSource.setPosition(startTime);
	transportSource.start();
	transportSource.getNextAudioBlock(audioChannelInfo);
	transportSource.stop();
	//reset the position to the previous one 
	transportSource.setPosition(currentTime);

	//activate the grain components only when a grain is selected
	displayGrain.setTime(startTime, startTime + GRAINLENGTH);
	sendActionMessage("activateGrain");
	//and compute windowing operation
	grainProcessing.applyWindow(buffer);

	//activate the save grain button
	saveButton.setEnabled(true);
}
    
void SelectionComponent::saveButtonClicked()
{
	sendActionMessage("saveGrain");
}

SoundState SelectionComponent::getState()
{
	return state;
}

void SelectionComponent::setButtonsEnable(bool enablePlay, bool enableStop, bool enableSelect)
{
	playButton.setEnabled(enablePlay);
	stopButton.setEnabled(enableStop);
	selectionButton.setEnabled(enableSelect);
}

