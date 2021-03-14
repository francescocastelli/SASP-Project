/*
  ==============================================================================

    Grainh.cpp
    Created: 14 Mar 2021 12:23:36am
    Author:  Francesco

  ==============================================================================
*/

#include "Grainh.h"

Grain::Grain() 
    : dataBuffer()
{
    
}


Grain::Grain(juce::AudioBuffer<float>& buf) 
    : dataBuffer (buf)
{
    
}


juce::AudioBuffer<float>& Grain::getBufferPointer()
{
    return dataBuffer;
}


