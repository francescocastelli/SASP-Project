/*
  ==============================================================================

    Model.h
    Created: 20 Mar 2021 1:09:12pm
    Author:  Francesco

  ==============================================================================

	Model is a singleton -> non copyable and movable.

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

    Model(const Model&) = delete;
    Model(Model&&) = delete;

    Model& operator=(const Model&) = delete;
    Model&& operator=(Model&&) = delete;
    
    ~Model() = default;
 
    //------------------------------ GENERAL INFO --------------------------

    double getReadSamplerate();
    double& getWriteSamplerate();

    juce::File& getGrainDirectory();

    ModelAudioState& getAudioState();

    juce::AudioTransportSource& getTransportSource();

    juce::AudioThumbnail& getAudioThumbnail();

    std::atomic<float>& getWriteFade();

    //----------------------------- GRAIN SELECTION ------------------------------

    juce::AudioBuffer<float>& getSelectedGrainBuffer();

    juce::AudioBuffer<float>& getProcessedGrainBuffer();

    std::atomic<float>& getWriteGrainLength();

    juce::dsp::WindowingFunction<float>& getCurrentWindow();

    juce::dsp::WindowingFunction<float>::WindowingMethod& getCurrentWindowMethod();

    juce::AudioFormatManager& getAudioFormatManager();

    std::atomic<float>& getGrainStartTime();
    
    std::atomic<float>& getGrainEndTime();

    std::atomic<int>& getAutomaticSelValue();

    //------------------------------- FILTER ---------------------------------

    void updateFilterCoeff();

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>& getWriteFilter();

    juce::Reverb& getWriteReverb();

    std::atomic<float>& getFilterCutoff();

    std::atomic<float>& getFilterResonance();

    std::atomic<int>& getFilterType();

    std::atomic<bool>& getNoFiltering();

    //----------------------- GRAIN PLAYBACK ---------------------------------------

    const std::vector<Grain>& getReadGrainQueue();
    std::vector<Grain>& getWriteGrainQueue();

    std::atomic<int>& getWritePos();
    std::atomic<int>& getReadPos();

    juce::Array<juce::File>& getWriteGrainstack();

    std::atomic<long long>& getReadTime();
    std::atomic<long long>& getWriteTime();

    std::atomic<float>& getWriteGain();

    std::atomic<float>& getWriteDensity();

    std::atomic<float>& getRandomPosition();

    bool getRandomSelection();

    void setRandomSelection(bool randomSel);

    bool getReverse();

    void setReverse(bool reverse);

    std::atomic<int>& getGrainPosition();

    std::atomic<int>& getGrainWindowLength();

    std::atomic<int>& getGrainCurrentIndex();

private:
    //------------------------------ GENERAL INFO --------------------------

    //the state of the model
    ModelAudioState currentAudioState;

    //sample rate 
    double sampleRate;

    //grain directory 
    juce::File grainDirectory;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    //audio thubnail to keep the opened file
    juce::AudioThumbnail audioThumbnail;

    //tranport source used to play the input audio
    juce::AudioTransportSource transportSource;

    //---------------------- GRAIN SELECTION ------------------------------------

    //lenght of the grain [s]
    std::atomic<float> grainLength;

    //fade value on the selected grain
    std::atomic<float> fadeValue;

    //start and end pos of the grain
    std::atomic<float> startTime;
    std::atomic<float> endTime;

    std::atomic<int> automaticSelectionValue;

    // grain selection flags
    std::atomic<bool> randomSelection;
    std::atomic<bool> playReverse;

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

    // filter parameters 
    std::atomic<float> cutoffFreq;
    std::atomic<float> resonance;
    std::atomic<int> filterType;
    std::atomic<bool> noFiltering;

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
    std::atomic<float> currentGain;

    //grain density
    std::atomic<float> grainDensity;

    //grain position
    std::atomic<int> grainPosition;

    //random position
    std::atomic<float> randomPosition;

    //grain window
    std::atomic<int> grainWindowLength;

    //grain current index
    std::atomic<int> grainCurrentIndex;
};
