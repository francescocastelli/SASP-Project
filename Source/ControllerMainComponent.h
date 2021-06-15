/*
  ==============================================================================

    ControllerMainComponent.cpp
    Created: 20 Mar 2021 4:04:30pm
    Author:  Francesco

  ==============================================================================

	Main controller, used to instantiate the model, audio loader, audio engine and 
	the two controllers (top and bottom).

    Is also responsible for creating the directory where grains will be saved.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AppConstants.h"
#include "ControllerTop.h"
#include "ControllerBottom.h"
#include "AudioEngine.h"
#include "AudioLoader.h"
#include "GrainProcessing.h"
#include "GrainSelectorThread.h"
#include "Model.h"

class ControllerMainComponent: public juce::AudioAppComponent
{
public:

    ControllerMainComponent();

    ~ControllerMainComponent() override;
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    //model instance
    Model model;

    //audio engine
    AudioEngine audioEngine;

    //audio loader
    AudioLoader audioLoader;

    //grain processing
    GrainProcessing grainProcessor;

    //grain selector
    GrainSelector grainSelector;

    //selected directory 
    juce::File dir;

    //top part of the window, contains the waveform and the grain selection (also buttons)
    // ControllerTop
    SelectionComponent selectionComponent;

    //bottom half of the window, contains the granular synth
    // ControllerBottom
    GranularSynthComponent granularSynthComponent;

    //--------------------------- private method ------------------------------------

    static void selectGrainFolder(int modalResult, ControllerMainComponent* param)
    {
        juce::FileChooser chooser("Select a directory for storing grains", {});

        if (chooser.browseForDirectory())
        {
            param->dir = chooser.getResult();
            param->model.getGrainDirectory() = param->dir;
        }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerMainComponent)
};
