/*
  ==============================================================================

    Model.h
    Created: 20 Mar 2021 1:09:12pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Grainh.h"

enum class ModelAudioState
{
    StopAudio,
    selectionPlay,
    grainPlay,
};

class Model 
{
public:
    Model();
    
    ~Model();
    
    //----------------------- grain playback---------------------------------------
    //get the reference to the grain queue, read or write
    const std::vector<Grain>& getReadGrainQueue();
    std::vector<Grain>& getWriteGrainQueue();

    //get read and write positions
    std::atomic<int>& getWritePos();
    std::atomic<int>& getReadPos();

    //get the ref to the file stack of grains
    juce::Array<juce::File>& getWriteGrainstack();

    //get reference to the current time
    std::atomic<long long>& getReadTime();
    std::atomic<long long>& getWriteTime();

    //get the current gain
    float getReadGain();
    float& getWriteGain();

    //get the current density 
    float getReadDensity();
    float& getWriteDensity();

    float& getRandomPosition();
    bool getRandomSelection();
    void setRandomSelection(bool randomSel);

    bool getReverse();
    void setReverse(bool reverse);

    int& getGrainPosition();

    int& getGrainWindowLength();

    int& getGrainCurrentIndex();

    //------------------------------- filter ------------------------------------------------
    //use to update the current filter coeff
    void updateFilterCoeff();

    //get reference to the current filter
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>& getWriteFilter();

    //get reg to reverb
    juce::Reverb& getWriteReverb();

    float& getFilterCutoff();

    float& getFilterResonance();

    float& getFilterGain();

    int& getFilterType();

    bool getNoFiltering();

    //------------------------------ general info ---------------------------
    //get the current sample rate
    double getReadSamplerate();
    double& getWriteSamplerate();

    juce::File& getGrainDirectory();

    //get the current state
    ModelAudioState& getAudioState();

    //get the tranpost source
    juce::AudioTransportSource& getTransportSource();

    //get the audio thumbnail 
    juce::AudioThumbnail& getAudioThumbnail();

    //get the fade
    float getReadFade();
    float& getWriteFade();

    //--------------------------------- grain selection ------------------------------
    //get the current selected grain buffer
    juce::AudioBuffer<float>& getSelectedGrainBuffer();

    //get the current processed buffer
    juce::AudioBuffer<float>& getProcessedGrainBuffer();

    //get the current grain length
    float getReadGrainLength();
    float& getWriteGrainLength();

    //get the current window
    juce::dsp::WindowingFunction<float>& getCurrentWindow();

    juce::dsp::WindowingFunction<float>::WindowingMethod& getCurrentWindowMethod();

    juce::AudioFormatManager& getAudioFormatManager();

    float& getGrainStartTime();
    
    float& getGrainEndTime();

    int& getAutomaticSelValue();

private:

    //the state of the model
    ModelAudioState currentAudioState;

    //sample rate 
    double sampleRate;

    //grain directory 
    juce::File grainDirectory;

    //tranport source used to play the input audio
    juce::AudioTransportSource transportSource;

    //audio thubnail to keep the opened file
    juce::AudioThumbnail audioThumbnail;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    //---------------------- GRAIN SELECTION ------------------------------------

    //lenght of the grain [s]
    float grainLength;

    //fade value on the selected grain
    float fadeValue;

    //start and end pos of the grain
    float startTime;

    float endTime;

    int automaticSelectionValue;

    bool randomSelection;

    bool playReverse;

    //buffer of the current selected grain
    juce::AudioBuffer<float> currentGrainBuffer;

    //buffer after processing
    juce::AudioBuffer<float> currentProcessedBuffer;

    //window 
    juce::dsp::WindowingFunction<float> currentWindow;

    //window method
    juce::dsp::WindowingFunction<float>::WindowingMethod currentWindowMethod;

    //------------------ FILTERING -----------------------

    //current filter
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;

    //current reverb 
    juce::Reverb reverb;

    float cutoffFreq;

    float resonance;

    int filterType;

    float filterGain;

    bool noFiltering;

    //---------------------- GRAIN PLAYBACK ----------------------

    //queue of current grains to be played
    std::vector<Grain> grainQueue;

    //stack of all the grains
    juce::Array<juce::File> grainFileStack;

    //current time 
    std::atomic<long long> time;

    std::atomic<int> writePos;

    std::atomic<int> readPos;

    //gain
    float currentGain;

    //grain density
    float grainDensity;

    //grain position
    int grainPosition;

    //random position
    float randomPosition;

    //grain window
    int grainWindowLength;

    //grain current index
    int grainCurrentIndex;
};
