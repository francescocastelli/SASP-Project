/*
  ==============================================================================

    SpectrogramComponent.h
    Created: 11 Mar 2021 11:21:52am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "Model.h"

class SpectrogramComponent : public juce::Component,
                             private juce::Timer
{
public:
    SpectrogramComponent();

    ~SpectrogramComponent() override;

    //==============================================================================

    void setNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void setEnabled(bool active, double sampleRate);

    void computeFreqAxis(double sampleRate);

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
        scopeSize = 512,
        freqAxisSize = 40,
    };

private:

    juce::dsp::FFT forwardFFT;                     
    juce::dsp::WindowingFunction<float> window;     

    float fifo[fftSize];                         
    float fftData[2 * fftSize];                   
    int fifoIndex = 0;                             
    bool nextFFTBlockReady = false;                
    float scopeData[scopeSize];                    
    float freqAxis[freqAxisSize-12];                    
    double sampleRate;

    bool enabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent);
};
