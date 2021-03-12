/*
  ==============================================================================

    GranularSynthComponent.cpp
    Created: 12 Mar 2021 9:28:14pm
    Author:  Francesco

  ==============================================================================
*/

#include "GranularSynthComponent.h"


GranularSynthComponent::GranularSynthComponent(juce::File& sampleDir, juce::Synthesiser& synth)
	:sampleDir (sampleDir),
	synth(synth)
{
	formatManager.registerBasicFormats();

	addAndMakeVisible(&loadGrain);
	loadGrain.setButtonText("Play");
	loadGrain.onClick = [this] { readGrains(); };
	loadGrain.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	loadGrain.setEnabled(true);
}

void GranularSynthComponent::readGrains() 
{
	int fileNum (0);

	juce::BigInteger range;
	range.setRange(0, 128, true);

	//get the number of files in the dir
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(sampleDir.getFullPathName()), false))
	{
		formatReader = formatManager.createReaderFor(entry.getFile());
		synth.addSound(new juce::SamplerSound(entry.getFile().getFileName(), *formatReader, range, 60, 0.1, 0.1, 0.1));
	}
}

void GranularSynthComponent::paint(juce::Graphics& g)
{
    g.setColour(AppColours::waveformBorder);
	g.drawRect(getLocalBounds());
}

void GranularSynthComponent::resized()
{
	loadGrain.setBoundsRelative(0.80f, 0.5f, 0.2f, 0.08f);
}

