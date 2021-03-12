#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"

class WaveformComponent: public juce::Component , private juce::ChangeListener
{
public:

    WaveformComponent(juce::AudioThumbnail& audioThumbnail);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    
    //audioThumbnail reference
    juce::AudioThumbnail& thumbnail; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformComponent)
};
