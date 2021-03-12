/*
  ==============================================================================

    GrainSelector.cpp
    Created: 15 Sep 2020 5:58:48pm
    Author:  Francesco

  ==============================================================================
*/
#pragma once
#include "GrainSelector.h"

GrainSelector::GrainSelector(juce::AudioThumbnail& audioThumbnail)
    : thumbnail (audioThumbnail),
      startTime (0.0),
      endTime (0.0),
      active(false)
{
}

void GrainSelector::setTime(double newStartTime, double newEndTime)
{
    startTime = newStartTime;
    endTime = newEndTime;
}

void GrainSelector::paint(juce::Graphics& g)
{
    if (!active)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainSelector::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    g.drawFittedText("No grain selected", getLocalBounds() , juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void GrainSelector::paintIfFileLoaded(juce::Graphics& g)
{
    //background
    g.fillAll(AppColours::waveformBackground);
    //waveform color
    g.setColour(AppColours::waveformColor);

    thumbnail.drawChannels(g, getLocalBounds(), startTime, endTime, 1.3f);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainSelector::actionListenerCallback(const juce::String &message) 
{
    if (message == "activateGrain")
    {
        active = true;
        repaint();
    }

    if (message == "deactivateGrain") {
        active = false;
        repaint();
    }
}
