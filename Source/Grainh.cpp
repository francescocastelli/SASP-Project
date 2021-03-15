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
      smoother(0),
      dataBufferIndex(0)
{
    smoother.reset(200);
}

Grain::~Grain()
{
}

Grain::Grain(juce::AudioBuffer<float>& buf) 
    : dataBuffer (buf),
      dataBufferIndex (0)
{
    
}

juce::AudioBuffer<float>& Grain::getBufferPointer()
{
    return dataBuffer;
}

int Grain::getCurrentIndex()
{
    return dataBufferIndex;
}

int Grain::getTotSamples()
{
    return dataBuffer.getNumSamples();
}

int Grain::getSamplesLeft()
{
    return dataBuffer.getNumSamples() - dataBufferIndex;
}

void Grain::incIndex(int numSamples)
{
    dataBufferIndex += numSamples;
}

void Grain::processBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto bufRead = bufferToFill.buffer->getReadPointer(channel);
        auto bufWrite = bufferToFill.buffer->getWritePointer(channel);

        bufferToFill.buffer->addFrom(channel, 0, dataBuffer, channel, dataBufferIndex, 
										juce::jmin(bufferToFill.numSamples, getSamplesLeft()));
	}

    dataBufferIndex += bufferToFill.numSamples;
    if (dataBufferIndex >= dataBuffer.getNumSamples())
        dataBufferIndex = 0;
}

bool Grain::isPlaying()
{
    return dataBufferIndex != 0;
}
