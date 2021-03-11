/*
  ==============================================================================

    SpectrogramComponent.h
    Created: 11 Mar 2021 11:21:52am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class SpectrogramComponent : public juce::Component,
                             private juce::Timer
{
public:
    SpectrogramComponent();

    ~SpectrogramComponent() override;

    //==============================================================================

    void setNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void paint(juce::Graphics& g) override;

    void timerCallback() override;

    void pushNextSampleIntoFifo(float sample) noexcept;

    void drawNextFrameOfSpectrum();

    void drawFrame(juce::Graphics& g);

    void paintIfFileLoaded(juce::Graphics& g);
    
    void paintIfNoFileLoaded(juce::Graphics& g);

    //fft parameters
    enum
    {
        fftOrder = 11,             
        fftSize = 1 << fftOrder,  
        scopeSize = 512               
    };

private:
    juce::dsp::FFT forwardFFT;                     
    juce::dsp::WindowingFunction<float> window;     

    float fifo[fftSize];                         
    float fftData[2 * fftSize];                   
    int fifoIndex = 0;                             
    bool nextFFTBlockReady = false;                
    float scopeData[scopeSize];                    

    bool paintComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent);
};
