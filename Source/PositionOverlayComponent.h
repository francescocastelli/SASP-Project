#pragma once
#include <JuceHeader.h>

class PositionOverlayComponent : public juce::Component, private juce::Timer
{
public:

    PositionOverlayComponent(juce::AudioTransportSource& transportSourceToUse, const juce::Rectangle<float> waveFormBound);

    void paint(juce::Graphics& g) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseDown(const juce::MouseEvent& event) override;

private:
    
    //transportSource
    juce::AudioTransportSource& transportSource;

    //bounds
    juce::Rectangle<float> waveFormBound;

    //private functions
    void timerCallback() override;

    void mouseSelection(const juce::MouseEvent& event);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlayComponent)
};

