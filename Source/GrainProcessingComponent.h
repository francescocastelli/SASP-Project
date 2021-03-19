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

class GrainProcessingComponent: public juce::Component
{
public:
    GrainProcessingComponent(juce::File& sampleDir, const float& grainLenght, const float& fadeValue, const double& sampleRate);

    //==============================================================================
    void windowMenuChanged(int id);

    void applyWindow(juce::AudioBuffer<float>& buffer);

    void computeWindowOutput();

    void saveGrain();

    void setEnable(bool enable);

    //-------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void drawFrame(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);
    
    void paintIfNoFileLoaded(juce::Graphics& g);

private:
    //window used in the process
    juce::dsp::WindowingFunction<float> grainWindow;     
    juce::dsp::WindowingFunction<float>::WindowingMethod windowMethod;

    //copy of the input buffer
    juce::AudioBuffer<float> originalBuffer;
    //buffer containing the windowed signal
    juce::AudioBuffer<float> windowedBuffer;

    //flags
    bool nowindowing;

    //enable or disable the component
    bool enabled;

    //folder where to save files
    juce::File& sampleDir;

    //current sample rate
    const double& sampleRate;
    
    //window lenght in second [s]
    const float& windowLenght;

    //fade in and fade out [s]
    const float& fadeValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainProcessingComponent);
};

