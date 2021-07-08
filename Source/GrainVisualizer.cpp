/*
  ==============================================================================

    GrainVisualizer.cpp
    Created: 20 Mar 2021 4:29:33pm
    Author:  Francesco

  ==============================================================================
*/

#include "..\Include\GrainVisualizer.h"

GrainVisualizer::GrainVisualizer(Model& model)
    :model(model),
    enabled(false)
{
}

void GrainVisualizer::setEnabled(bool enabled)
{
    this->enabled = enabled;
    repaint();
}

void GrainVisualizer::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainVisualizer::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(AppColours::buttonsText);
    g.drawFittedText("NO GRAIN SELECTED", getLocalBounds(), juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainVisualizer::paintIfFileLoaded(juce::Graphics& g)
{
    //background
    g.fillAll(AppColours::waveformBackground);
    //waveform color
    g.setColour(AppColours::waveformColor);

    model.getAudioThumbnail().drawChannels(g, getLocalBounds(), model.getGrainStartTime(), model.getGrainEndTime(), 1.3f);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

