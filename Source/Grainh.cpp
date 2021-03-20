/*
  ==============================================================================

    Grainh.cpp
    Created: 14 Mar 2021 12:23:36am
    Author:  Francesco

  ==============================================================================
*/

#include "Grainh.h"

Grain::Grain() 
    : dataBuffer(),
      startIndex (0),
      dataBufferIndex(0)
{
}

Grain::Grain(juce::AudioBuffer<float>& buf, int startTime, int id) 
    : dataBuffer (),
      startIndex (startTime),
      id (id),
      dataBufferIndex (0)
{
    //make a copy of the buffer
    dataBuffer = buf;
}

Grain::~Grain()
{
}

void Grain::processBlock(const juce::AudioSourceChannelInfo& bufferToFill, int timeIndex)
{
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto outputChannel = bufferToFill.buffer->getWritePointer(channel);
        auto dataChannel = dataBuffer.getReadPointer(channel);

        outputChannel[timeIndex % bufferToFill.numSamples] += dataChannel[timeIndex - startIndex];
    }
}

void Grain::fadeOut(int startSample)
{
    dataBuffer.applyGainRamp(startSample, dataBuffer.getNumSamples() - startSample, 1.0f, 0.0f);
}

void Grain::fadeIn(int endSample)
{
    dataBuffer.applyGainRamp(0, endSample, 0.0f, 1.0f);
}

bool Grain::canPlay(int timeIndex)
{
    return (startIndex < timeIndex) && (timeIndex < startIndex + dataBuffer.getNumSamples());
}

int Grain::getStartIndex()
{
    return startIndex;
}

bool Grain::hasEnded(int timeIndex)
{
    return (timeIndex > startIndex + dataBuffer.getNumSamples());
}

int Grain::getId()
{
    return id;
}
