/*
  ==============================================================================

    InputSceneComponent.h
    Created: 28 Aug 2020 10:55:51am
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GrainProcessingVisualizer.h"
#include "GrainVisualizer.h"
#include "WaveformComponent.h"
#include "PositionOverlayComponent.h"
#include "KnobLookAndFeel.h"
#include "AudioLoader.h"
#include "GrainProcessing.h"
#include "Model.h"

enum class SelectionState
{
    Disabled,
    Loaded,
    Stopped,
    Playing,
    SelectGrain
};

class SelectionComponent : public juce::Component
{
public:
    SelectionComponent(Model&  model, AudioLoader& audioLoader, GrainProcessing& grainProcessor);

    //-------------------------------------------------------------------------------------

    void paint(juce::Graphics& g) override;

    void resized() override;

private:

    //model ref
    Model& model;

    //selection state
    SelectionState currentState;

    //audio loader ref
    AudioLoader& audioLoader;

    //grain processing module ref
    GrainProcessing& grainProcessor;

    KnobLookAndFeel knobLookAndFeel;

    GrainVisualizer grainVisualizer;
    GrainProcessingVisualizer windowedGrainVisualizer;
    WaveformVisualizer waveformVisualizer;
    PositionOverlayComponent positionVisualizer;

    //buttons
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton selectionButton;
    juce::TextButton saveButton;
    juce::TextButton automaticSaveButton;
    juce::TextButton grainLeft;
    juce::TextButton grainRight;

    //text labels
    juce::Label waveformLabel;
    juce::Label grainLabel;
    juce::Label windowLabel;
    juce::Label fadeLabel;

    //sliders
    juce::Slider grainLenghtSlider;
    juce::Slider fadeSlider;
    juce::Slider automaticSelectionSlider;

    //grain processing buttons
    juce::ComboBox windowsMenu;

    //------------------------------------- private methods ----------------------------

    void changeCurrentState(SelectionState newState);
    
    void setButtonsEnable(bool enableOpen, bool enablePlay, bool enableStop, bool enableSelect, bool enableSave);
    
    void setSliderEnable(bool enableWlenght, bool enableFade, bool enableWselection, bool enableAuto);

    void openButtonClicked();

    void playButtonClicked();

    void stopButtonClicked();

    void saveButtonClicked();

    void selectionButtonClicked();

    void grainLenghtChanged();

    void windowMenuChanged();

    void changeWindowedChannel(int channel);

    void automaticGrainSelection();

    void automaticValueChanged();

    void fadeValueChanged();
};
