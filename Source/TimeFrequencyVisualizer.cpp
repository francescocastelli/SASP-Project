/*
  ==============================================================================

    TimeFrequencyVisualizer.cpp
    Created: 16 Mar 2021 6:58:37pm
    Author:  Francesco

  ==============================================================================
*/

#include "TimeFrequencyVisualizer.h"

GrainVisualizer::GrainVisualizer()
    :active(false)
{
    startTimerHz(300);
}

GrainVisualizer::~GrainVisualizer() {}

void GrainVisualizer::setActive(bool active)
{
    this->active = active;
    repaint();
}


void GrainVisualizer::setGrains(const juce::Array<juce::File>& grainFileArray)
{
    float xOffset =  float(getLocalBounds().getWidth()) / float(grainFileArray.size());
    auto x = 0.0f;

    for (int i = 0; i < grainFileArray.size(); ++i)
    {
        x += xOffset;
        auto y = juce::Random::getSystemRandom().nextInt(juce::Range<int>(10, getHeight()-10));
        grainArray.add(juce::Point<float>(x, y));
    }
}

void GrainVisualizer::addCurrentIndex(int grainId)
{
    currentIndex =  grainId;
}


void GrainVisualizer::timerCallback()
{
    repaint();
}

void GrainVisualizer::paint(juce::Graphics& g)
{
    if (!active)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainVisualizer::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void GrainVisualizer::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    drawGrains(g);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainVisualizer::drawGrains(juce::Graphics& g)
{
    for (int i = 0; i < grainArray.size(); ++i)
    {
        g.setColour(AppColours::specStroke);
        if ( i == currentIndex )
            g.fillEllipse(grainArray[i].getX(), grainArray[i].getY(), 8, 8);
        else
            g.fillEllipse(grainArray[i].getX(), grainArray[i].getY(), 3, 3);
    }
}

