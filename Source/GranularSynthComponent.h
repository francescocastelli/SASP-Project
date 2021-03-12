/*
  ==============================================================================

    GranularSynthComponent.h
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"

class GranularSynthComponent: public juce::Component
{
public:
    GranularSynthComponent(juce::File& sampleDir, juce::Synthesiser& synth);

    //-------------------------------------------------------------
    void readGrains();
    //-------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void resized() override;

private:

    juce::File& sampleDir;

    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader;
    juce::Synthesiser& synth;

    //buttons
    juce::TextButton loadGrain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthComponent);
};

