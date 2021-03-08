#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent(juce::AudioThumbnail& audioThumbnail) : thumbnail (audioThumbnail)
{
    thumbnail.addChangeListener(this);
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
    g.drawFittedText("No File Loaded", getLocalBounds() , juce::Justification::centred, 1);
}
 
void ThumbnailComponent::paintIfFileLoaded(juce::Graphics& g)
{
     g.fillAll(juce::Colours::lightgrey);
     g.setColour(juce::Colours::black);

     thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 6.0f);
}

void ThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
    if (source == &thumbnail)
        repaint();
}
