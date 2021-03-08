/*
  ==============================================================================

    GrainSelector.cpp
    Created: 15 Sep 2020 5:58:48pm
    Author:  Francesco

  ==============================================================================
*/

#include "GrainSelector.h"

GrainSelector::GrainSelector(juce::AudioThumbnail& audioThumbnail)
    : thumbnail (audioThumbnail),
      startTime (0.0),
      endTime (0.0),
      paintGrain (false)
{
    thumbnail.addChangeListener(this);
}

void GrainSelector::setPaintGrain(bool newPaintGrain)
{
    paintGrain = newPaintGrain;
}

void GrainSelector::setTime(double newStartTime, double newEndTime)
{
    startTime = newStartTime;
    endTime = newEndTime;
}

void GrainSelector::paint(juce::Graphics& g)
{
    if (thumbnail.getNumChannels() == 0 || !paintGrain)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainSelector::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.drawFittedText("No File Loaded", getLocalBounds() , juce::Justification::centred, 1);
}
 
void GrainSelector::paintIfFileLoaded(juce::Graphics& g)
{
     g.fillAll(juce::Colours::lightgrey);
     g.setColour(juce::Colours::black);

     thumbnail.drawChannels(g, getLocalBounds(), startTime, endTime, 12.0f);
}

void GrainSelector::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
    if (source == &thumbnail)
        repaint();
}
