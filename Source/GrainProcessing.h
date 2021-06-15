/*
  ==============================================================================

    GrainProcessingComponent.h
    Created: 11 Mar 2021 10:04:40pm
    Author:  Francesco

  ==============================================================================

	Class used to perform all the preprocessing after grain selection: allows to 
    apply to the grain a window and some fade in/out based on the value currently 
    selected in the model.

    Also used for saving the grain to disk after the preprocessing (2 channels, wav 
    format, 16bit, sample rate equal to the input sample rate).

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

    void selectCurrentGrain();

    void computeWindowOutput();

    void saveGrain();

private:

    Model& model;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainProcessing);
};

