#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"

class PositionOverlayComponent : public juce::Component, private juce::Timer
{
public:

    PositionOverlayComponent(juce::AudioTransportSource& transportSourceToUse, const float& grainLenght);

    void paint(juce::Graphics& g) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseDown(const juce::MouseEvent& event) override;

private:
    
    //transportSource
    juce::AudioTransportSource& transportSource;

    const float& currentGrainLenght;

    //private functions
    void timerCallback() override;

    void mouseSelection(const juce::MouseEvent& event);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlayComponent)
};

