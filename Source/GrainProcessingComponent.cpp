/*
  ==============================================================================

    GrainProcessingComponent.cpp
    Created: 11 Mar 2021 10:04:40pm
    Author:  Francesco

  ==============================================================================
*/

#include "GrainProcessingComponent.h"

GrainProcessingComponent::GrainProcessingComponent(juce::File& saveDir)
                            :nowindowing(false),
                             active(false),
                             sampleRate (44100),
		                     grainWindow(4410, juce::dsp::WindowingFunction<float>::rectangular),
                             originalBuffer(),
                             windowedBuffer(),
                             fadeSamples(0),
                             sampleDir(saveDir)
{
}

void GrainProcessingComponent::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void GrainProcessingComponent::setGrainLenght(int grainLenght)
{
    windowLenght = grainLenght;
}

void GrainProcessingComponent::setFadeValue(int fadeValue)
{
    fadeSamples = fadeValue;;
    if ( active ) computeWindowOutput();
}

void GrainProcessingComponent::windowMenuChanged(int id)
{
	switch (id)
	{
    case 1: nowindowing = true; break;
	case 2: grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::rectangular); break;
	case 3: grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::hann); break;
	case 4: grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::hamming); break;
	case 5: grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::triangular); break;
	case 6: grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::blackman); break;
	default:
		break;
	}

    if (id != 1) nowindowing = false;
	if ( active ) computeWindowOutput();
}

void GrainProcessingComponent::applyWindow(juce::AudioBuffer<float>& buffer)
{
     if (buffer.getNumChannels() > 0)
     {
        //take a copy of the buffer
        originalBuffer = buffer;
        //compute the windowing
        computeWindowOutput();
     }
}

void GrainProcessingComponent::computeWindowOutput()
{
    //every time re-use the original buffer
    windowedBuffer = originalBuffer;

    //min max normalization
    juce::Range<float> rangeIn = windowedBuffer.findMinMax(0, 0, windowedBuffer.getNumSamples());
    juce::NormalisableRange<float> norm = juce::NormalisableRange<float>(rangeIn);

    for (int i = 0; i < windowedBuffer.getNumSamples(); ++i)
    {
        windowedBuffer.getWritePointer(0)[i] = (norm.convertTo0to1(windowedBuffer.getReadPointer(0)[i]) - 0.5) * 0.8;
    }

    //apply the selected window
    if( !nowindowing ) grainWindow.multiplyWithWindowingTable(windowedBuffer.getWritePointer(0), windowLenght);

    //fade in
    windowedBuffer.applyGainRamp(0, fadeSamples, 0.0f, 1.0f);
    //fade out
    windowedBuffer.applyGainRamp(windowedBuffer.getNumSamples()-fadeSamples, fadeSamples, 1.0f, 0.0f);

    //paint the windowed grain
    repaint();
}

void GrainProcessingComponent::actionListenerCallback(const juce::String &message)
{
    if (message == "activateGrain") active = true;

    if (message == "saveGrain" && active) saveGrain();

    if (message == "deactivateGrain") {
        active = false;
        repaint();
    }
}

void GrainProcessingComponent::saveGrain()
{
   	//saving the sample in the wav file 
	juce::WavAudioFormat format;
	std::unique_ptr<juce::AudioFormatWriter> writer;
	int fileNum (0);

	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator (juce::File(sampleDir.getFullPathName()), false))
		fileNum++;

	//path to the file
	juce::File file(sampleDir.getFullPathName() + "\\grain" + juce::String(fileNum)+".wav");

	//write the wav file with the buffer content
	writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
										sampleRate,
										1,
										16,
										{},
										0));

	if (writer != nullptr)
    	writer->writeFromAudioSampleBuffer(windowedBuffer, 0, windowedBuffer.getNumSamples());
}

void GrainProcessingComponent::paint(juce::Graphics& g)
{
    if (!active)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainProcessingComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    g.drawFittedText("No grain selected", getLocalBounds() , juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void GrainProcessingComponent::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    drawFrame(g);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainProcessingComponent::drawFrame(juce::Graphics& g)
{
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    
    //create a new path
    juce::Path path = juce::Path();
    juce::PathStrokeType stroke = juce::PathStrokeType(0.6f);

    for (int i = 0; i < windowLenght; ++i)
    {
        //add points to the path
        path.lineTo((float)juce::jmap(i, 0, windowLenght- 1, 0, width),
            juce::jmap(windowedBuffer.getReadPointer(0)[i], 0.0f, 1.0f, (float)height, 0.0f) - height/2);
    }

    //draw the stroke
    g.setColour(AppColours::grainStroke);
    g.strokePath(path, stroke);
}




