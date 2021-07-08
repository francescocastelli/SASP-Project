#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Model.h"

class WaveformVisualizer: public juce::Component 
{
public:

    WaveformVisualizer(Model& model);

    void setEnabled(bool enabled);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

private:
    Model& model;

    bool enabled;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformVisualizer)
};
