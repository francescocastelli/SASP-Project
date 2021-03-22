/*
  ==============================================================================

    TimeFrequencyVisualizer.h
    Created: 16 Mar 2021 6:58:37pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <set>
#include "AppConstants.h"
#include "Model.h"

class TimeFreqVisualizer: public juce::Component, public juce::Timer
{
public:
    TimeFreqVisualizer(Model& model);

    ~TimeFreqVisualizer() override;

    //==============================================================================

    void setEnabled(bool active);

    void addCurrentIndex(int grainId);

    void setGrains();

    void timerCallback() override;

    void paint(juce::Graphics& g) override;

    void drawGrains(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);
    
    void paintIfNoFileLoaded(juce::Graphics& g);

private:

    Model& model;

    juce::Array<juce::Point<float>> grainArray;

    bool enabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeFreqVisualizer);
};

