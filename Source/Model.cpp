/*
  ==============================================================================

    Model.cpp
    Created: 20 Mar 2021 1:09:12pm
    Author:  Francesco

  ==============================================================================
*/

#include "Model.h"

Model::Model()
    :
    grainDirectory(":Empty"),
    time(0),
    thumbnailCache(5),
    audioThumbnail(512, formatManager, thumbnailCache),
    currentWindow(4410, juce::dsp::WindowingFunction<float>::rectangular),
    grainLength(0.1f),
    currentGain(0.8f),
    fadeValue(0.0f),
    grainQueue (200),
    grainPosition(1),
    grainWindowLength(1),
    grainDensity(1),
    noFiltering(true),
    randomPosition (0.0f),
    randomSelection(false),
    readPos (0), 
    writePos(0),
    currentAudioState (ModelAudioState::StopAudio),
    sampleRate(0.0)
{
    //format inizialization
    formatManager.registerBasicFormats();

    //create the directory
    //grainDirectory.createDirectory();
}

//----------------------------------------------------------

const std::vector<Grain>& Model::getReadGrainQueue()
{
    return grainQueue;
}

std::vector<Grain>& Model::getWriteGrainQueue()
{
    return grainQueue;
}

std::atomic<long long>& Model::getReadTime()
{
    return time;
}

std::atomic<long long>& Model::getWriteTime()
{
    return time;
}

juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>& Model::getWriteFilter()
{
    return filter;
}

void Model::updateFilterCoeff()
{
    switch (filterType)
    {
    case 0:
        noFiltering = true;
        break;
    case 1:
        noFiltering = false;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFreq, resonance);
        break;
    case 2:
        noFiltering = false;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, cutoffFreq, resonance);
        break;
    case 3:
        noFiltering = false;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoffFreq, resonance);
        break;
    default:
        break;
    }
}

juce::Reverb& Model::getWriteReverb()
{
    return reverb;
}

std::atomic<float>& Model::getWriteGain()
{
    return currentGain;
}

double Model::getReadSamplerate()
{
    return sampleRate;
}

double& Model::getWriteSamplerate()
{
    return sampleRate;
}

std::atomic<float>& Model::getWriteDensity()
{
    return grainDensity;
}

ModelAudioState& Model::getAudioState()
{
    return currentAudioState;
}

juce::AudioTransportSource& Model::getTransportSource()
{
    return transportSource;
}

juce::AudioThumbnail& Model::getAudioThumbnail()
{
    return audioThumbnail;
}

juce::Array<juce::File>& Model::getWriteGrainstack()
{
    return grainFileStack;
}

juce::File& Model::getGrainDirectory() 
{
    return grainDirectory;
}

std::atomic<float>& Model::getWriteFade()
{
    return fadeValue;
}

juce::AudioBuffer<float>& Model::getSelectedGrainBuffer()
{
    return currentGrainBuffer;
}

juce::AudioBuffer<float>& Model::getProcessedGrainBuffer()
{
    return currentProcessedBuffer;
}

std::atomic<float>& Model::getWriteGrainLength()
{
    return grainLength;
}

juce::dsp::WindowingFunction<float>& Model::getCurrentWindow()
{
    return currentWindow;
}

juce::AudioFormatManager& Model::getAudioFormatManager()
{
    return formatManager;
}

std::atomic<float>& Model::getGrainStartTime()
{
    return startTime;
}
    
std::atomic<float>& Model::getGrainEndTime()
{
    return endTime;
}

juce::dsp::WindowingFunction<float>::WindowingMethod& Model::getCurrentWindowMethod()
{
    return currentWindowMethod;
}

std::atomic<int>& Model::getAutomaticSelValue()
{
    return automaticSelectionValue;
}

std::atomic<float>& Model::getFilterCutoff()
{
    return cutoffFreq;
}

std::atomic<float>& Model::getFilterResonance()
{
    return resonance;
}

std::atomic<int>& Model::getFilterType()
{
    return filterType;
}

std::atomic<int>& Model::getGrainPosition()
{
    return grainPosition;
}

std::atomic<int>& Model::getGrainWindowLength()
{
    return grainWindowLength;
}

std::atomic<int>& Model::getGrainCurrentIndex()
{
    return grainCurrentIndex;
}

std::atomic<bool>& Model::getNoFiltering()
{
    return noFiltering;
}

std::atomic<float>& Model::getRandomPosition()
{
    return randomPosition;
}

bool Model::getRandomSelection()
{
    return randomSelection;
}

void Model::setRandomSelection(bool randomSel)
{
    randomSelection = randomSel;
}

bool Model::getReverse()
{
    return playReverse;
}

void Model::setReverse(bool reverse)
{
    playReverse = reverse;
}

std::atomic<int>& Model::getWritePos()
{
    return writePos;
}

std::atomic<int>& Model::getReadPos()
{
    return readPos;
}
