#include "WaveformComponent.h"

WaveformVisualizer::WaveformVisualizer(Model& model) 
    :model(model),
    enabled(false)
{
}

void WaveformVisualizer::setEnabled(bool enabled)
{
    this->enabled = enabled;
    repaint();
}

void WaveformVisualizer::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void WaveformVisualizer::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

	g.setFont(juce::Font (12.0f, juce::Font::bold));
    g.setColour(AppColours::buttonsText);
    g.drawFittedText("NO FILE LOADED", getLocalBounds() , juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
 
void WaveformVisualizer::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    model.getAudioThumbnail().drawChannels(g, getLocalBounds(), 0.0, model.getAudioThumbnail().getTotalLength(), 1.0f);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}
