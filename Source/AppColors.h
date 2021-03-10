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
    const juce::Colour background = juce::Colour(juce::uint8(41), juce::uint8(30), juce::uint8(64), 0.6f);

    //waveform display
    const juce::Colour waveformBackground = juce::Colour(juce::uint8(41), juce::uint8(30), juce::uint8(64), 0.7f);
    const juce::Colour waveformColor =  juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.7f);
    const juce::Colour waveformBorder = juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.7f);

    //position overlay over waveform
    const juce::Colour positionOverlay = juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.4f);
    const juce::Colour positionOverlayBar = juce::Colour(juce::uint8(255), juce::uint8(0), juce::uint8(0), 1.0f);

    //text
    const juce::Colour boxText = juce::Colour(juce::uint8(245), juce::uint8(245), juce::uint8(245), 0.7f);
};
