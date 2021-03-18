/*
  ==============================================================================

    TimeFrequencyVisualizer.cpp
    Created: 16 Mar 2021 6:58:37pm
    Author:  Francesco

  ==============================================================================
*/

#include "TimeFrequencyVisualizer.h"

GrainVisualizer::GrainVisualizer()
{
    startTimerHz(20);
}

GrainVisualizer::~GrainVisualizer() {}

void GrainVisualizer::setActive(bool active)
{
    this->active = active;
    repaint();
}

void GrainVisualizer::setGrains(const juce::Array<juce::File>& grainFileArray)
{
    float xOffset = float(grainFileArray.size()) / float(getLocalBounds().getWidth());
    auto x = 0.0f;

    for (int i = 0; i < grainFileArray.size(); ++i)
    {
        x += xOffset+1;
        auto y = juce::Random::getSystemRandom().nextInt(getHeight());
        grainArray.add(juce::Point<float>(x, y));
    }
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
        g.setColour(juce::Colours::red);
        g.fillEllipse(grainArray[i].getX(), grainArray[i].getY(), 1, 1);
    }
}

