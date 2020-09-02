#pragma once
#include <JuceHeader.h>

class ThumbnailComponent: public juce::Component , private juce::ChangeListener
{
public:

    ThumbnailComponent(int sourceSamplesPerThumbnailSample, juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache, const juce::Rectangle<float> grainBound, const juce::Rectangle<float> waveFormBound );

    void setFile(const juce::File& file);

    void grainSelection(double startPos, double endPos);

    void paint(juce::Graphics& g);

    void paintIfNoFileLoaded(juce::Graphics& g);
     
    void paintIfFileLoaded(juce::Graphics& g);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    
    //audioThumbnail
    juce::AudioThumbnail thumbnail; 

    //bounds
    juce::Rectangle<float> waveFormBound;
    juce::Rectangle<float> grainBound;

    //timing pos
    double startPos;
    double endPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThumbnailComponent)
};
