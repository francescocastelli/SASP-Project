/*
  ==============================================================================

    TimeFrequencyVisualizer.h
    Created: 16 Mar 2021 6:58:37pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"


class GrainVisualizer: public juce::Component, public juce::Timer
{
public:
    GrainVisualizer();

    ~GrainVisualizer() override;

    //==============================================================================

    void setActive(bool active);

    void setGrains(const juce::Array<juce::File>& grainFileArray);

    void timerCallback() override;

    void paint(juce::Graphics& g) override;

    void drawGrains(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);
    
    void paintIfNoFileLoaded(juce::Graphics& g);

private:

    juce::Array<juce::Point<float>> grainArray;

    bool active;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer);
};

