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
      enabled(false)
{
}

void GrainSelector::setTime(double newStartTime, double newEndTime)
{
    startTime = newStartTime;
    endTime = newEndTime;
}

void GrainSelector::setEnable(bool enable)
{
    this->enabled = enable;
    repaint();
}

void GrainSelector::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainSelector::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

	g.setFont(juce::Font (12.0f, juce::Font::bold));
    g.setColour(AppColours::buttonsText);
    g.drawFittedText("NO GRAIN SELECTED", getLocalBounds() , juce::Justification::centred, 1);

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
