/*
  ==============================================================================

    GrainSelector.h
    Created: 15 Sep 2020 5:58:48pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class GrainSelector: public juce::Component , private juce::ChangeListener
{
public:

    GrainSelector(juce::AudioThumbnail& audioThumbnail);

    void setPaintGrain(bool newPaintGrain);

    void setTime(double newStartTime, double newEndTime);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    
    //audioThumbnail reference
    juce::AudioThumbnail& thumbnail; 

    double startTime;
    double endTime;
    bool paintGrain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainSelector)
};
