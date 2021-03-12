/*
  ==============================================================================

    GrainSelector.h
    Created: 15 Sep 2020 5:58:48pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"

class GrainSelector: public juce::Component , public juce::ActionListener
{
public:

    GrainSelector(juce::AudioThumbnail& audioThumbnail);

    void setTime(double newStartTime, double newEndTime);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

    void actionListenerCallback(const juce::String &message) override;

private:
    
    //audioThumbnail reference
    juce::AudioThumbnail& thumbnail; 

    double startTime;
    double endTime;
    bool active;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainSelector)
};
