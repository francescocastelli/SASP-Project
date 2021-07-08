/*
  ==============================================================================

    GrainProcessingcpp.cpp
    Created: 20 Mar 2021 6:12:52pm
    Author:  Francesco

  ==============================================================================
*/

#include "GrainProcessing.h"

GrainProcessing::GrainProcessing(Model& model)
    :model (model)
{
}

void GrainProcessing::selectCurrentGrain()
{
    auto& currentGrainLenght = model.getWriteGrainLength();
    auto totLength = model.getTransportSource().getLengthInSeconds();
	auto currentTime = model.getTransportSource().getCurrentPosition();
	auto startTime = (currentTime -  currentGrainLenght/ 2) < 0 ? 0 : currentTime - currentGrainLenght / 2;
	auto endTime = (startTime + currentGrainLenght >= totLength) ? totLength: startTime + currentGrainLenght;
    auto currentGain = model.getWriteGain().load();

	//temp buffer used for getting the sample to store in the wav
	//the lenght of the buffer is the grain lenght
	juce::AudioBuffer<float> buffer(2, (endTime - startTime) * model.getReadSamplerate());
	//clear the buffer
	buffer.clear();
	//create the audio channel info that is required from the getNextAudioBlock
	juce::AudioSourceChannelInfo audioChannelInfo(buffer);

	//fill the buffer for grainlength samples from the current position  
	//to get the buffer the transportSource must be in play 

    model.getAudioState() = ModelAudioState::selectionPlay;
    model.getTransportSource().setPosition(startTime);
	model.getTransportSource().start();
    model.getWriteGain() = 0.0f;
	model.getTransportSource().getNextAudioBlock(audioChannelInfo);
	model.getTransportSource().stop();
    model.getWriteGain() = currentGain;
    model.getAudioState() = ModelAudioState::StopAudio;
	//reset the position to the previous one 
	model.getTransportSource().setPosition(endTime);

    model.getGrainStartTime() = startTime;
    model.getGrainEndTime() = startTime + (endTime - startTime);

    //save the buffer into the model
    model.getSelectedGrainBuffer() = buffer;
}

void GrainProcessing::computeWindowOutput()
{
    //always check the maximum lenght
    auto fadeInSamples = juce::jmin(int(model.getWriteFade()* model.getReadSamplerate()), model.getSelectedGrainBuffer().getNumSamples());
    //auto windowLenght = juce::jmin(int(model.getWriteFade()* model.getReadSamplerate()), model.getSelectedGrainBuffer().getNumSamples());
    auto windowLenght = model.getSelectedGrainBuffer().getNumSamples();

    //get the buffer 
    auto currentBuffer = model.getSelectedGrainBuffer();

    //create the window 
    model.getCurrentWindow().fillWindowingTables(windowLenght, model.getCurrentWindowMethod() );

    //windowing
    for (int channel = 0; channel < model.getSelectedGrainBuffer().getNumChannels(); ++channel)
    {
        model.getCurrentWindow().multiplyWithWindowingTable(currentBuffer.getWritePointer(channel), windowLenght);
    }

    //min max normalization
    for (int channel = 0; channel < currentBuffer.getNumChannels(); ++channel)
    {
        juce::Range<float> rangeIn = currentBuffer.findMinMax(channel, 0, currentBuffer.getNumSamples());
        if (rangeIn.getStart() == rangeIn.getEnd()) rangeIn.setEnd(rangeIn.getEnd() + 0.001);
        juce::NormalisableRange<float> norm = juce::NormalisableRange<float>(rangeIn);

        // TODO: check level of output grain
        for (int i = 0; i < currentBuffer.getNumSamples(); ++i)
        {
           currentBuffer.getWritePointer(channel)[i] = (norm.convertTo0to1(currentBuffer.getReadPointer(channel)[i]) - 0.5f);
        }
    }

    //fade in
    currentBuffer.applyGainRamp(0, fadeInSamples, -0.5f, .5f);
    //fade out
    currentBuffer.applyGainRamp(currentBuffer.getNumSamples() - fadeInSamples, fadeInSamples, .5f, -.5f);

    //save the result into the processed buffer
    model.getProcessedGrainBuffer() = currentBuffer;
}

void GrainProcessing::saveGrain()
{
    //saving the sample in the wav file 
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    int fileNum(0);

    if (model.getGrainDirectory().getFullPathName() != ":Empty")
    {
        //get the number of files in the dir
        for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(model.getGrainDirectory().getFullPathName()), false))
            fileNum++;

        //path to the file
        juce::File file(model.getGrainDirectory().getFullPathName() + "\\grain" + juce::String(fileNum) + ".wav");

        //write the wav file with the buffer content
        writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
            model.getReadSamplerate(),
            2,
            16,
            {},
            0));

        if (writer != nullptr)
            writer->writeFromAudioSampleBuffer(model.getProcessedGrainBuffer(), 0, model.getProcessedGrainBuffer().getNumSamples());
    }else
        juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, "No folder selected!", "Please restart and select a folder");
}
