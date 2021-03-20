/*
  ==============================================================================

    GrainProcessingComponent.cpp
    Created: 11 Mar 2021 10:04:40pm
    Author:  Francesco

  ==============================================================================
*/

#include "GrainProcessingComponent.h"

GrainProcessingComponent::GrainProcessingComponent(juce::File& saveDir, const float& grainLenght, const float& fadeValue, const double& sampleRate, const bool& showLeft)
                            :nowindowing(false),
                             enabled(false),
                             windowLenght(grainLenght),
                             fadeValue(fadeValue),
                             sampleRate (sampleRate),
                             showLeft (showLeft),
		                     grainWindow(4410, juce::dsp::WindowingFunction<float>::rectangular),
                             originalBuffer(),
                             windowedBuffer(),
                             sampleDir(saveDir)
{
}

void GrainProcessingComponent::setEnable(bool enable)
{
    enabled = enable;
    repaint();
}

void GrainProcessingComponent::windowMenuChanged(int id)
{

	switch (id)
	{
    case 1: nowindowing = true; break;
    case 2: windowMethod = juce::dsp::WindowingFunction<float>::rectangular; break;//grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::rectangular); break;
	case 3: windowMethod = juce::dsp::WindowingFunction<float>::hann; break;//grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::hann); break;
	case 4: windowMethod = juce::dsp::WindowingFunction<float>::hamming; break;//grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::hamming); break;
	case 5: windowMethod = juce::dsp::WindowingFunction<float>::triangular; break;//grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::triangular); break;
	case 6: windowMethod = juce::dsp::WindowingFunction<float>::blackman; break;//grainWindow.fillWindowingTables(windowLenght, juce::dsp::WindowingFunction<float>::blackman); break;
	default:
		break;
	}

    if (id != 1) nowindowing = false;
	if ( enabled ) computeWindowOutput();
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
    //always check the maximum lenght
    auto fadeInSamples = juce::jmin(int(fadeValue * sampleRate), originalBuffer.getNumSamples());
    auto windowLenght = juce::jmin(int(this->windowLenght * sampleRate), originalBuffer.getNumSamples());

    //every time re-use the original buffer
    windowedBuffer = originalBuffer;
    //create the window 
    if ( !nowindowing) grainWindow.fillWindowingTables(windowLenght, windowMethod);
    
    //windowing
    for (int channel = 0; channel < windowedBuffer.getNumChannels(); ++channel) 
    {
        /*
        juce::Range<float> rangeIn = windowedBuffer.findMinMax(channel, 0, windowedBuffer.getNumSamples());
        if (rangeIn.getStart() == rangeIn.getEnd()) rangeIn.setEnd(rangeIn.getEnd() + 0.001);
        juce::NormalisableRange<float> norm = juce::NormalisableRange<float>(rangeIn);

        for (int i = 0; i < windowedBuffer.getNumSamples(); ++i)
        {
            windowedBuffer.getWritePointer(channel)[i] = (norm.convertTo0to1(windowedBuffer.getReadPointer(channel)[i]) - 0.5) * 0.8;
        }
        */
        if (!nowindowing) grainWindow.multiplyWithWindowingTable(windowedBuffer.getWritePointer(channel), windowLenght);
    }

    //min max normalization
    for (int channel = 0; channel < windowedBuffer.getNumChannels(); ++channel)
    {
        juce::Range<float> rangeIn = windowedBuffer.findMinMax(channel, 0, windowedBuffer.getNumSamples());
        if (rangeIn.getStart() == rangeIn.getEnd()) rangeIn.setEnd(rangeIn.getEnd() + 0.001);
        juce::NormalisableRange<float> norm = juce::NormalisableRange<float>(rangeIn);

        for (int i = 0; i < windowedBuffer.getNumSamples(); ++i)
        {
            windowedBuffer.getWritePointer(channel)[i] = (norm.convertTo0to1(windowedBuffer.getReadPointer(channel)[i]) - 0.5) ;
        }
    }

    //fade in
    windowedBuffer.applyGainRamp(0, fadeInSamples, 0.0f, 1.0f);
    //fade out
    windowedBuffer.applyGainRamp(windowedBuffer.getNumSamples()- fadeInSamples, fadeInSamples, 1.0f, 0.0f);

    //paint the windowed grain
    repaint();
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
										2,
										16,
										{},
										0));

	if (writer != nullptr)
    	writer->writeFromAudioSampleBuffer(windowedBuffer, 0, windowedBuffer.getNumSamples());
}

void GrainProcessingComponent::paint(juce::Graphics& g)
{
    if (!enabled )  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainProcessingComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

	g.setFont(juce::Font (12.0f, juce::Font::bold));
    g.setColour(AppColours::buttonsText);
    g.drawFittedText("NO GRAIN SELECTED", getLocalBounds() , juce::Justification::centred, 1);

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
    auto windowLenght = juce::jmin(int(this->windowLenght * sampleRate), windowedBuffer.getNumSamples());
    auto channel = (showLeft) ? 0 : 1;

    //create a new path
    juce::Path path = juce::Path();
    juce::PathStrokeType stroke = juce::PathStrokeType(0.9f);

    //starting point of the path
    path.startNewSubPath(0, height/2);

    for (int i = 0; i < windowLenght; ++i)
    {
        //add points to the path
        path.lineTo((float)juce::jmap(i, 0, windowLenght- 1, 0, width),
            juce::jmap(windowedBuffer.getReadPointer(channel)[i], 0.0f, 1.0f, (float)height, 0.0f) - height/2 );
    }

    //draw the stroke
    g.setColour(AppColours::grainStroke);
    g.strokePath(path, stroke);
}




