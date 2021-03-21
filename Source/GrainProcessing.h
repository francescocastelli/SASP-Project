/*
  ==============================================================================

    GrainProcessingComponent.h
    Created: 11 Mar 2021 10:04:40pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Model.h"

class GrainProcessing
{
public:
    GrainProcessing(Model& model);

    //==============================================================================

    void applyWindow();
    
    void selectCurrentGrain();

    void computeWindowOutput();

    void saveGrain();


private:

    Model& model;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainProcessing);
};

