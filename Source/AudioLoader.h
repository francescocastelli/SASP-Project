/*
  ==============================================================================

    AudioLoader.h
    Created: 20 Mar 2021 3:09:28pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Model.h"

class AudioLoader
{
public:
    AudioLoader(Model& model);

    void loadInputFile();

    void loadGrainFile();

private:

    Model& model;

    //reader for loading audio files
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

};
