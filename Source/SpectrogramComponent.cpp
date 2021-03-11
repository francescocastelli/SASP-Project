/*
  ==============================================================================

    SpectrogramComponent.cpp
    Created: 11 Mar 2021 11:21:52am
    Author:  Francesco

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrogramComponent.h"
#include "AppColors.h"

SpectrogramComponent::SpectrogramComponent()
    : forwardFFT(fftOrder),
    paintComp(false),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
}

SpectrogramComponent::~SpectrogramComponent() {}

void SpectrogramComponent::setNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo(channelData[i]);

        paintComp = true;
        timerCallback();
    }
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
    if (!paintComp)  paintIfNoFileLoaded(g);
    else  paintIfFileLoaded(g);
}

void SpectrogramComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.fillAll(AppColours::waveformBackground);

    g.setColour(AppColours::waveformColor);
    g.drawFittedText("No file loaded", getLocalBounds() , juce::Justification::centred, 1);

    g.setColour(AppColours::waveformBorder);
    g.drawRect(getLocalBounds(), 1);
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

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)                         
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);

        scopeData[i] = level;                                   
    }
}

void SpectrogramComponent::drawFrame(juce::Graphics& g)
{
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    
    //create a new path
    juce::Path path = juce::Path();
    juce::PathStrokeType specStroke = juce::PathStrokeType(0.6f);

    //starting point of the path
    path.startNewSubPath(getLocalBounds().getBottomLeft().getX(), getLocalBounds().getBottomLeft().getY());

    for (int i = 0; i < scopeSize; ++i)
    {
        //add points to the path
        path.lineTo((float)juce::jmap(i, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f));
    }

    //add the last point
    path.lineTo(getLocalBounds().getBottomRight().getX(), getLocalBounds().getBottomRight().getY());

    //close the path and fill it 
    path.closeSubPath();
    g.setColour(AppColours::specFill);
    g.fillPath(path);

    //draw the stroke
    g.setColour(AppColours::specStroke);
    g.strokePath(path, specStroke);
}