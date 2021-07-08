/*
  ==============================================================================

    GrainProcessingComponent.h
    Created: 11 Mar 2021 10:04:40pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Model.h"

class GrainVisualizer: public juce::Component
{
public:
    GrainVisualizer(Model& model);

    void setEnabled(bool enabled);

    void paint(juce::Graphics& g) override;

    void paintIfFileLoaded(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);

private:
    Model& model;

    bool enabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer);
};

