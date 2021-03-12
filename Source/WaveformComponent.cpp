#include "WaveformComponent.h"

WaveformComponent::WaveformComponent(juce::AudioThumbnail& audioThumbnail) : thumbnail (audioThumbnail)
{
    thumbnail.addChangeListener(this);
}

void WaveformComponent::paint(juce::Graphics& g)
{
    if (thumbnail.getNumChannels() == 0)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void WaveformComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    g.drawFittedText("No file loaded", getLocalBounds() , juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void WaveformComponent::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void WaveformComponent::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
    if (source == &thumbnail)
        repaint();
}
