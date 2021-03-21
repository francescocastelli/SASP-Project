/*
  ==============================================================================

    GrainProcessingVisualizer.h
    Created: 20 Mar 2021 6:13:44pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Model.h"

class GrainProcessingVisualizer: public juce::Component
{
public:
    GrainProcessingVisualizer(Model& model);

    void setEnabled(bool enabled);

    void setChannel(int channel);

    void paint(juce::Graphics& g) override;

    void paintIfNoFileLoaded(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);

    void drawFrame(juce::Graphics& g);

private:

    Model& model;

    bool enabled;

    int currentChannel;
};
