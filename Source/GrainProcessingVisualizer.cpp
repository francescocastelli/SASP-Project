/*
  ==============================================================================

    GrainProcessing.cpp
    Created: 20 Mar 2021 3:32:26pm
    Author:  Francesco

  ==============================================================================
*/

#include "..\Include\GrainProcessingVisualizer.h"

GrainProcessingVisualizer::GrainProcessingVisualizer(Model& model)
    :model(model),
    enabled(false),
    currentChannel(0)
{
}

void GrainProcessingVisualizer::setEnabled(bool enabled)
{
    this->enabled = enabled;
    repaint();
}

void GrainProcessingVisualizer::setChannel(int channel)
{
    if (channel == 0 || channel == 1)
        currentChannel = channel;
    else currentChannel = 0;
}

void GrainProcessingVisualizer::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void GrainProcessingVisualizer::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(AppColours::buttonsText);
    g.drawFittedText("NO GRAIN SELECTED", getLocalBounds(), juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainProcessingVisualizer::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    drawFrame(g);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void GrainProcessingVisualizer::drawFrame(juce::Graphics& g)
{
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    auto windowLenght = juce::jmin(int(model.getWriteGrainLength() * model.getReadSamplerate()), model.getProcessedGrainBuffer().getNumSamples());
    //auto channel = (showLeft) ? 0 : 1;

    //create a new path
    juce::Path path = juce::Path();
    juce::PathStrokeType stroke = juce::PathStrokeType(0.9f);

    //starting point of the path
    path.startNewSubPath(0, height * 0.5f);

    for (int i = 0; i < windowLenght; ++i)
    {
        //add points to the path
        path.lineTo((float)juce::jmap(i, 0, windowLenght - 1, 0, width),
            juce::jmap(model.getProcessedGrainBuffer().getReadPointer(currentChannel)[i], 0.0f, 1.0f, (float)height, 0.0f) - height * 0.5f);
    }

    //draw the stroke
    g.setColour(AppColours::grainStroke);
    g.strokePath(path, stroke);
}

