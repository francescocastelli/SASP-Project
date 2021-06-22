/*
  ==============================================================================

    AppColors.h
    Created: 10 Mar 2021 9:40:44pm
    Author:  Francesco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace AppColours
{
    //main background
    const juce::Colour background = juce::Colour(juce::uint8(19), juce::uint8(22), juce::uint8(34), 0.85f);

    //waveform display
    const juce::Colour waveformBackground = juce::Colour(juce::uint8(13), juce::uint8(15), juce::uint8(23), 0.7f);
    const juce::Colour waveformColor =  juce::Colour(juce::uint8(157), juce::uint8(6), juce::uint8(26), 0.9f);
    const juce::Colour waveformBorder = juce::Colour(juce::uint8(13), juce::uint8(15), juce::uint8(23), 0.95f);

    //position overlay over waveform
    const juce::Colour positionOverlay = juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.4f);
    const juce::Colour positionOverlayBar = juce::Colour(juce::uint8(255), juce::uint8(255), juce::uint8(255), 0.6f);

    //text
    const juce::Colour boxText = juce::Colour(juce::uint8(109), juce::uint8(159), juce::uint8(226), 0.7f);
    const juce::Colour buttonsText = juce::Colour(juce::uint8(255), juce::uint8(255), juce::uint8(255), 0.7f);

    //buttons
    const juce::Colour buttons = juce::Colour(juce::uint8(109), juce::uint8(159), juce::uint8(226), 0.7f);

    //spectrogram
    const juce::Colour specFill = juce::Colour(juce::uint8(157), juce::uint8(6), juce::uint8(26), 0.6f);
    const juce::Colour specStroke = juce::Colour(juce::uint8(157), juce::uint8(6), juce::uint8(26), 0.9f);

    //windowing
    const juce::Colour grainWindowedFill = juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.7f);
    const juce::Colour grainStroke = juce::Colour(juce::uint8(157), juce::uint8(6), juce::uint8(26), 0.9f);

    //knob 
    const juce::Colour knobBackground = juce::Colour(juce::uint8(13), juce::uint8(15), juce::uint8(23), 0.9f);
    const juce::Colour knobOutline = juce::Colour(juce::uint8(255), juce::uint8(255), juce::uint8(255), 0.9f);
    const juce::Colour knobText = juce::Colour(juce::uint8(255), juce::uint8(255), juce::uint8(255), 0.7f);
    const juce::Colour knobLabel = juce::Colour(juce::uint8(255), juce::uint8(255), juce::uint8(255), 0.9f);
};

namespace AppConstants
{
    //project name 
    const juce::String projectName = "Granular synth";
    //grain
    constexpr float maxgrainlengthsec = 0.1;

    //buttons position
    const float controlButtonsY = 0.05f;
    const float controlButtonWidth = 0.06f;
    const float controlButtonHeight = 0.08f;

    //know width and heigth
    const float knobWidth = 0.14f;
    const float knobHeigth = 0.25f;
    const float knobFadeWidth = 0.10f;
    const float knobFadeHeigth = 0.20f;

  };
