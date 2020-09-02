/*
  ==============================================================================

    InputSceneComponent.h
    Created: 28 Aug 2020 10:55:51am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ThumbnailComponent.h"
#include "PositionOverlayComponent.h"

class WaveFormComponent : public juce::Component
{
public:
    WaveFormComponent(int sourceSamplesPerThumbnailSample,
        juce::AudioFormatManager& formatManagerComp,
        juce::AudioThumbnailCache& cache,
        juce::AudioTransportSource& transportSourceToUse) 
        : thumbnailComp(sourceSamplesPerThumbnailSample, formatManagerComp, cache), 
        positionComp(transportSourceToUse)
    {
        addAndMakeVisible(thumbnailComp);
        addAndMakeVisible(positionComp);
    }

    void resized() override
    {
        thumbnailComp.setBounds(getLocalBounds());
        positionComp.setBounds(getLocalBounds());
    }

    void setFile(const juce::File& file)
    {
        thumbnailComp.setFile(file);
    }

    void grainSelection(double start, double end, juce::Rectangle<int> bounds)
    {
        thumbnailComp.grainSelection(start, end, bounds);
    }

private:
    ThumbnailComponent thumbnailComp;
    PositionOverlayComponent positionComp;
};

