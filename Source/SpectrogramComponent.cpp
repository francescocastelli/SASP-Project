/*
  ==============================================================================

    SpectrogramComponent.cpp
    Created: 11 Mar 2021 11:21:52am
    Author:  Francesco

  ==============================================================================
*/

#include <JuceHeader.h>
#include "..\Include\SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent()
    : forwardFFT(fftOrder),
    enabled(false),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    computeFreqAxis();
    computedBLevel();
    startTimerHz(30);
}

SpectrogramComponent::~SpectrogramComponent() {}

void SpectrogramComponent::setNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo(channelData[i]);
    }
}

void SpectrogramComponent::setEnabled(bool active, double sampleRate)
{
    this->enabled = active;
    repaint();
}


void SpectrogramComponent::pushNextSampleIntoFifo(float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next frame should now be rendered..
    if (fifoIndex == fftSize)               
    {
        if (!nextFFTBlockReady)           
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;             
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    if (!enabled)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void SpectrogramComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);

    for (int i = 0; i < freqAxisSize - 12; ++i)
    {
        g.setColour(juce::Colours::grey);
        g.setOpacity(0.6);
        g.drawVerticalLine((float)juce::jmap(freqAxis[i], 1.f, (float)log10(20000), 0.f, (float)width), 0.f, (float)height);
    }

    for (int i = 0; i < 9; ++i)
    {
        g.setColour(juce::Colours::grey);
        g.setOpacity(0.6);
        g.drawHorizontalLine(juce::jmap(dbLevel[i], 0.f, 1.f, 0.f, (float)height), 0.f, (float)width);
    }
}
 
void SpectrogramComponent::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    drawFrame(g);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
}

void SpectrogramComponent::timerCallback() 
{
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrogramComponent::drawNextFrameOfSpectrum()
{
    // first apply a windowing function to our data
    window.multiplyWithWindowingTable(fftData, fftSize);       

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);  

    for (int i = 0; i < scopeSize; ++i)                         
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.1f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(AppConstants::mindB, AppConstants::maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)), AppConstants::mindB, AppConstants::maxdB, 0.0f, 1.5f);

        scopeData[i] = level;                                   
    }

}

void SpectrogramComponent::drawFrame(juce::Graphics& g)
{
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    
    // path for the fft amplitude 
    juce::Path path = juce::Path();
    juce::PathStrokeType specStroke = juce::PathStrokeType(0.6f);

    // starting point of the path
    path.startNewSubPath(getLocalBounds().getBottomLeft().getX(), getLocalBounds().getBottomLeft().getY());

    // fft of the signal
    for (int i = 0; i < scopeSize; ++i)
    {
        path.lineTo((float)juce::jmap(i, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f));
    }

    // bode frequency axes
    for (int i = 0; i < freqAxisSize - 12; ++i)
    {
        g.setColour(juce::Colours::grey);
        g.setOpacity(0.6f);
        g.drawVerticalLine((float)juce::jmap(freqAxis[i], 1.f, (float)log10(20000), 0.f, (float)width), 0.f, (float)height);
    }
    
    for (int i = 0; i < 9; ++i)
    {
        g.setColour(juce::Colours::grey);
        g.setOpacity(0.6f);
        g.drawHorizontalLine(juce::jmap(dbLevel[i], 0.f, 1.f, 0.f, (float)height), 0.f, (float)width);
    }

    // add the last point
    path.lineTo(getLocalBounds().getBottomRight().getX(), getLocalBounds().getBottomRight().getY());

    // close the path and fill it 
    path.closeSubPath();
    g.setColour(AppColours::specFill);
    g.fillPath(path);
}

void SpectrogramComponent::computeFreqAxis()
{
    int pos = 0;
    for (auto i = 1; i < (int)(freqAxisSize / 10); ++i)
    {
        for (auto j = 1; j < 10; ++j)
        {
            freqAxis[pos] = log10(j * pow(10, i));
            ++pos;
        }
    }
    
    freqAxis[pos] = log10(pow(10, 4));
    freqAxis[pos+1] = log10(2 * pow(10, 4));
}

void SpectrogramComponent::computedBLevel()
{
    for (int i=0; i<9; i+=2)
        dbLevel[i] = juce::jmap(-i * 10.f, AppConstants::mindB, AppConstants::maxdB, 0.0f, 1.0f);
}
