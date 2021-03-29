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
    grainPosition(1),
    grainWindowLength(1),
    grainDensity(1),
    noFiltering(true),
    randomPosition (0.0f),
    randomSelection(false),
    readPos (0), 
    writePos(0)
{
    //format inizialization
    formatManager.registerBasicFormats();

    //create the directory
    //grainDirectory.createDirectory();
}

Model::~Model()
{
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
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, cutoffFreq, resonance, filterGain);
        break;
    case 2:
        noFiltering = false;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, cutoffFreq, resonance);
        break;
    case 3:
        noFiltering = false;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, cutoffFreq, resonance, filterGain);
        break;
    default:
        break;
    }
}

juce::Reverb& Model::getWriteReverb()
{
    return reverb;
}

float Model::getReadGain()
{
    return currentGain;
}

float& Model::getWriteGain()
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

float Model::getReadDensity()
{
    return grainDensity;
}

float& Model::getWriteDensity()
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

float Model::getReadFade()
{
    return fadeValue;
}

float& Model::getWriteFade()
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

float Model::getReadGrainLength()
{
    return grainLength;
}

float& Model::getWriteGrainLength()
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

float& Model::getGrainStartTime()
{
    return startTime;
}
    
float& Model::getGrainEndTime()
{
    return endTime;
}

juce::dsp::WindowingFunction<float>::WindowingMethod& Model::getCurrentWindowMethod()
{
    return currentWindowMethod;
}

int& Model::getAutomaticSelValue()
{
    return automaticSelectionValue;
}

float& Model::getFilterCutoff()
{
    return cutoffFreq;
}

float& Model::getFilterResonance()
{
    return resonance;
}

float& Model::getFilterGain()
{
    return filterGain;
}

int& Model::getFilterType()
{
    return filterType;
}

int& Model::getGrainPosition()
{
    return grainPosition;
}

int& Model::getGrainWindowLength()
{
    return grainWindowLength;
}

int& Model::getGrainCurrentIndex()
{
    return grainCurrentIndex;
}

bool Model::getNoFiltering()
{
    return noFiltering;
}

float& Model::getRandomPosition()
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
