/*
  ==============================================================================

    AudioEngine.h
    Created: 20 Mar 2021 1:08:58pm
    Author:  Francesco

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "Grainh.h"
#include "Model.h"

class GrainSelector: public juce::Thread
{
public:
    GrainSelector(Model& model);

    //-------------------------------------------------------------

    void run() override;

    void reset();

    void start();

private:

    Model& model;
   
    long long nextGrainStart;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainSelector);
};

