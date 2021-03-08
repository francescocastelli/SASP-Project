#pragma once
#include <JuceHeader.h>

class ThumbnailComponent: public juce::Component , private juce::ChangeListener
{
public:

    ThumbnailComponent(juce::AudioThumbnail& audioThumbnail);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    
    //audioThumbnail reference
    juce::AudioThumbnail& thumbnail; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThumbnailComponent)
};
