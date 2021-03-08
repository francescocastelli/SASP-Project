/*
  ==============================================================================

    InputSceneComponent.h
    Created: 28 Aug 2020 10:55:51am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ThumbnailComponent.h"
#include "PositionOverlayComponent.h"
#include "GrainSelector.h"

#define GRAINLENGTH 0.1    //grain length in seconds

enum class SoundState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};


class WaveFormComponent : public juce::Component, public juce::ChangeBroadcaster 
{
public:
    WaveFormComponent(juce::AudioTransportSource& transportSourceRef, juce::File &sampleDir)
        : thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache),
        thumbnailComp(thumbnail),
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

        addAndMakeVisible(thumbnailComp);
        addAndMakeVisible(positionComp);
        addAndMakeVisible(displayGrain);

        formatManager.registerBasicFormats();
    }

    void resized() override
    {
        openButton.setBoundsRelative(0.80f, 0.05f, 0.06f, 0.08f);
        playButton.setBoundsRelative(0.87f, 0.05f, 0.06f, 0.08f);
        stopButton.setBoundsRelative(0.94f, 0.05f, 0.06f, 0.08f);
        selectionButton.setBoundsRelative(0.80f, 0.18f, 0.200f, 0.08f);

        thumbnailComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.3f);
        positionComp.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.3f);
        displayGrain.setBoundsRelative(0.02f, 0.45f, 0.5f, 0.55f);
    }

    void grainSelection(double start, double end, juce::Rectangle<int> bounds)
    {
        //thumbnailComp.grainSelection(start, end, bounds);
    }

    void openButtonClicked()
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
    
    void playButtonClicked()
    {
        state = SoundState::Starting;
        sendChangeMessage();
    }
    
    void stopButtonClicked()
    {
        state = SoundState::Stopping;
        sendChangeMessage();
    }
    
    void selectionButtonClicked()
    {
        auto currentTime = transportSource.getCurrentPosition();
        auto startTime = (currentTime - GRAINLENGTH/2) < 0 ? 0 : currentTime - GRAINLENGTH/2;
        displayGrain.setTime(startTime, startTime + GRAINLENGTH);
        displayGrain.setPaintGrain(true);
        thumbnail.sendChangeMessage();

        //save the grain as wav file
        saveWav();
    }
    
    void saveWav()
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

        //temp buffer used for getting the sample to store in the wav
        //the lenght of the buffer is the grain lenght
        juce::AudioBuffer<float> buffer(2, GRAINLENGTH*sampleRate);
        //clear the buffer
        buffer.clear();
        //create the audio channel info that is required from the getNextAudioBlock
        juce::AudioSourceChannelInfo audioChannelInfo(buffer);

        //to get the buffer the transportSource must be in play 
        transportSource.start();
        transportSource.getNextAudioBlock(audioChannelInfo);
        transportSource.stop();

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

    SoundState getState()
    {
        return state;
    }

    void setButtonsEnable(bool enablePlay, bool enableStop, bool enableSelect)
    {
        playButton.setEnabled(enablePlay);
        stopButton.setEnabled(enableStop);
        selectionButton.setEnabled(enableSelect);
    }

private:
    
    SoundState state;
    double sampleRate;

    juce::AudioFormatManager formatManager;    //manager for the different formats
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache;                 
    juce::AudioThumbnail thumbnail;
    juce::AudioTransportSource& transportSource;

    //top-buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;

    ThumbnailComponent thumbnailComp;
    PositionOverlayComponent positionComp;
    GrainSelector displayGrain;

    //sample dir ref
    juce::File& sampleDir;
};

