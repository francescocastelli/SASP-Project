#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent(int sourceSamplesPerThumbnailSample,
                   juce::AudioFormatManager& formatManager,
                   juce::AudioThumbnailCache& cache, const juce::Rectangle<float> grainBounds, const juce::Rectangle<float> waveFormBound) 
                    : thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache),
                      waveFormBound(waveFormBound),
                      grainBound(grainBounds),
                      startPos {0.0f},
                      endPos {0.0f}
{
    thumbnail.addChangeListener(this);
}

void ThumbnailComponent::setFile(const juce::File& file)
{
    thumbnail.setSource(new juce::FileInputSource(file));
}

void ThumbnailComponent::grainSelection(double startPos, double endPos) 
{
    ThumbnailComponent::startPos = startPos;
    ThumbnailComponent::endPos = endPos;
    repaint();
}

void ThumbnailComponent::paint(juce::Graphics& g)
{
    if (thumbnail.getNumChannels() == 0)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void ThumbnailComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.drawFittedText("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
}
 
void ThumbnailComponent::paintIfFileLoaded(juce::Graphics& g)
{
     g.fillAll(juce::Colours::lightgrey);
     g.setColour(juce::Colours::black);
 
     thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 6.0f);
     thumbnail.drawChannels(g, juce::Rectangle<int>(10,50, 100, 100), startPos, endPos, 6.0f);
}

void ThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
    if (source == &thumbnail)
        repaint();
}
