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

class GrainProcessingComponent: public juce::Component, public juce::ActionListener
{
public:
    GrainProcessingComponent(juce::File& sampleDir);

    //==============================================================================
    void windowMenuChanged(int id);

    void applyWindow(juce::AudioBuffer<float>& buffer);

    void computeWindowOutput();

    void saveGrain();

    void setSampleRate(double sampleRate);

    void setGrainLenght(int grainLenght);

    void setFadeValue(int fadeValue);
    //-------------------------------------------------------------

    void actionListenerCallback(const juce::String& message) override;

    //-------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void drawFrame(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);
    
    void paintIfNoFileLoaded(juce::Graphics& g);

private:
    //window used in the process
    juce::dsp::WindowingFunction<float> grainWindow;     
    //copy of the input buffer
    juce::AudioBuffer<float> originalBuffer;
    //buffer containing the windowed signal
    juce::AudioBuffer<float> windowedBuffer;

    //flags
    bool nowindowing;

    bool fadeInAndOut;

    int fadeSamples;
    
    //enable or disable the component
    bool active;

    //folder where to save files
    juce::File& sampleDir;

    //current sample rate
    double sampleRate;
    
    int windowLenght;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainProcessingComponent);
};

