/*
  ==============================================================================

    SelectionComponent.cpp
    Created: 10 Mar 2021 10:04:24pm
    Author:  Francesco

  ==============================================================================
*/

#include "SelectionComponent.h"
#include "AppColors.h"

SelectionComponent::SelectionComponent(juce::AudioTransportSource& transportSourceRef, juce::File &sampleDir)
		: thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache),
        thumbnailComp(thumbnail),
	    specComp(),
        transportSource(transportSourceRef),
        positionComp(transportSource),
        displayGrain(thumbnail),
        sampleDir(sampleDir)
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

	//add and make visible internal components
	addAndMakeVisible(thumbnailComp);
	addAndMakeVisible(positionComp);
	addAndMakeVisible(displayGrain);
	addAndMakeVisible(specComp);

	//inizialize format for the format manager
	formatManager.registerBasicFormats();
}

void SelectionComponent::resized() 
{
	//buttons sizes
	openButton.setBoundsRelative(0.79f, 0.05f, 0.06f, 0.08f);
	playButton.setBoundsRelative(0.86f, 0.05f, 0.06f, 0.08f);
	stopButton.setBoundsRelative(0.93f, 0.05f, 0.06f, 0.08f);
	selectionButton.setBoundsRelative(0.79f, 0.18f, 0.200f, 0.08f);

	//waveform display 
	thumbnailComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
	positionComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.40f);
        
	//grain display
    displayGrain.setBoundsRelative(0.02f, 0.53f, 0.25f, 0.35f);

	//spec display 
	specComp.setBoundsRelative(0.30f, 0.53f, 0.25f, 0.35f);
}

void SelectionComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::boxText);
	g.setFont(juce::Font(13, 1));
	g.drawSingleLineText("Waveform", 22, 12);
	g.drawSingleLineText("Selected Grain", 22, 170);
	g.drawSingleLineText("Grain spectrogram", 350, 170);

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
	displayGrain.setTime(startTime, startTime + GRAINLENGTH);
	displayGrain.setPaintGrain(true);
	thumbnail.sendChangeMessage();
	
	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 
	transportSource.setPosition(startTime);
	transportSource.start();
	transportSource.getNextAudioBlock(audioChannelInfo);
	transportSource.stop();
	//reset the position to the previous one 
	transportSource.setPosition(currentTime);

	//save the grain as wav file
	saveWav(startTime, buffer);

	//display the spec
	specComp.setNextAudioBlock(audioChannelInfo);
}
    
void SelectionComponent::saveWav(float startTime, juce::AudioBuffer<float>& buffer)
{
	//saving the sample in the wav file 
	juce::WavAudioFormat format;
	std::unique_ptr<juce::AudioFormatWriter> writer;
	int fileNum (0);

	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator (juce::File(sampleDir.getFullPathName()), false))
		fileNum++;

	//path to the file
	juce::File file(sampleDir.getFullPathName() + "\\sample " + juce::String(fileNum)+".wav");

	//write the wav file with the buffer content
	writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
										sampleRate,
										buffer.getNumChannels(),
										24,
										{},
										0));
	if (writer != nullptr)
		writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
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

