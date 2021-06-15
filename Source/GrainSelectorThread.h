/*
  ==============================================================================

    GrainSelectorThread.h
    Created: 20 Mar 2021 1:08:58pm
    Author:  Francesco

  ==============================================================================

  This represent a thread (juce::Thread) that is used to read grains from the grain 
  stack at the correct position. Grain stack is a circular buffer so we need to check
  boundaries and go back to the beginning when we reach the end.

  After reading the current grain it also should schedule it, so it computes starting
  time, apply fade in if needed (if it overlap with the previous scheduled grain) and 
  apply fade out.

  The scheduling is done in this way: at step i we set nextgrainstart, which is the 
  starting time of the next grain (processed at step i+1, we are scheduling ahed of 
  the current step). For i = 0 nextgrainstart is set to the current time of the model 
  (model has a time varible that keep the global time).
  This thread is always reading grains from the grain stack ahed wrt the audioengine
  and schedule them to be played at a time instant greater than the current time of 
  the model.

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

    ~GrainSelector();
    //-------------------------------------------------------------

    void run() override;

    void reset();

    void start();

private:

    Model& model;
   
    long long nextGrainStart;

    //--------------------- private methods ---------------------------

    void selectNextGrain();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainSelector);
};

