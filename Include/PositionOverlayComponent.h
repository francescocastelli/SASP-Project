#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Model.h"

class PositionOverlayComponent : public juce::Component, private juce::Timer
{
public:

    PositionOverlayComponent(Model& model);

    void setEnabled(bool enabled);

    void paint(juce::Graphics& g) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseDown(const juce::MouseEvent& event) override;

private:
    
    Model& model;

    bool enabled;

    // -------------------- private functions -------------------------

    void timerCallback() override;

    void mouseSelection(const juce::MouseEvent& event);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlayComponent)
};

