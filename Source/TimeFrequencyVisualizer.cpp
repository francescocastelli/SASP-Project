/*
  ==============================================================================

    TimeFrequencyVisualizer.cpp
    Created: 16 Mar 2021 6:58:37pm
    Author:  Francesco

  ==============================================================================
*/

#include "..\Include\TimeFrequencyVisualizer.h"

TimeFreqVisualizer::TimeFreqVisualizer(Model& model)
    :enabled(false),
    model(model)
{
    startTimerHz(300);
}

TimeFreqVisualizer::~TimeFreqVisualizer() {}

void TimeFreqVisualizer::setEnabled(bool active)
{
    this->enabled= active;
    repaint();
}

void TimeFreqVisualizer::setGrains()
{
    auto grainFileArray = model.getWriteGrainstack();
    float xOffset =  float(getLocalBounds().getWidth()) / float(grainFileArray.size());
    auto x = 0.0f;

    for (int i = 0; i < grainFileArray.size(); ++i)
    {
        x += xOffset;
        auto y = juce::Random::getSystemRandom().nextInt(juce::Range<int>(10, getHeight()-10));
        grainArray.add(juce::Point<float>(x, y));
    }
}

void TimeFreqVisualizer::timerCallback()
{
    repaint();
}

void TimeFreqVisualizer::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void TimeFreqVisualizer::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void TimeFreqVisualizer::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    drawGrains(g);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void TimeFreqVisualizer::drawGrains(juce::Graphics& g)
{
    for (int i = 0; i < grainArray.size(); ++i)
    {
        g.setColour(AppColours::specStroke);
        if ( i == model.getGrainCurrentIndex())
            g.fillEllipse(grainArray[i].getX(), grainArray[i].getY(), 8, 8);
        else
            g.fillEllipse(grainArray[i].getX(), grainArray[i].getY(), 3, 3);
        
    }

    float size = model.getWriteGrainstack().size();

    float drawPosition = (float)(model.getGrainPosition()/size)*getWidth();
    float rectWidth = (float)(model.getGrainWindowLength() / size)* getWidth();

    g.setColour(AppColours::positionOverlayBar);
    g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
    g.setColour(AppColours::positionOverlay);
    g.setOpacity(0.3f);
    g.fillRoundedRectangle(drawPosition, 0.0f, rectWidth, getHeight(), 1.0f);
}

void TimeFreqVisualizer::reset()
{
    grainArray.clear();
}

