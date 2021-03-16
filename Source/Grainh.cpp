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
      dataBufferIndex(0),
      active (false)
{
}

Grain::~Grain()
{
}

Grain::Grain(juce::AudioBuffer<float>& buf) 
    : dataBuffer (buf),
      dataBufferIndex (0),
      active(false)
{
    
}

juce::AudioBuffer<float>& Grain::getBufferPointer()
{
    return dataBuffer;
}

int Grain::getSamplesLeft()
{
    return dataBuffer.getNumSamples() - dataBufferIndex;
}

void Grain::processBlock(const juce::AudioSourceChannelInfo& bufferToFill, long currentTimeIndex)
{
    if (active) 
    {
        int startPoint;
        //fill the two channel of the buffer with the grain buffer
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            startPoint = (currentTimeIndex % bufferToFill.numSamples);
            bufferToFill.buffer->addFrom(channel, startPoint, dataBuffer, channel, dataBufferIndex, 
										juce::jmin(bufferToFill.numSamples - startPoint, getSamplesLeft()));
	    }

        dataBufferIndex += bufferToFill.numSamples;
        //if the grain has been copyed all then deactive the grain and reset the index
        if (dataBufferIndex >= dataBuffer.getNumSamples())
        {
            dataBufferIndex = 0;
            active = false;
        }
    }
}

void Grain::setStartIndex(int startIndex)
{
    this->startIndex = startIndex;
}

int Grain::getStartIndex()
{
    return startIndex;
}

bool Grain::isActive()
{
    return active;
}

void Grain::activeGrain()
{
    active = true;
}
