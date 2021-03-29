/*
  ==============================================================================

    GrainSelectorThread.cpp
    Created: 20 Mar 2021 2:33:39pm
    Author:  Francesco

  ==============================================================================
*/

#include "GrainSelectorThread.h"

GrainSelector::GrainSelector(Model& model)
    :model(model),
	Thread("Selector thread"),
	nextGrainStart(0)
{

}

GrainSelector::~GrainSelector()
{
	stopThread(100);
}

//-------------------------------------------------------------

void GrainSelector::reset() 
{
	stopThread(100);
	nextGrainStart = 0;
}

void GrainSelector::start()
{
	startThread();
}

void GrainSelector::selectNextGrain()
{
	auto currentIndex = model.getGrainCurrentIndex();

	if (model.getRandomSelection())
	{
		model.getGrainCurrentIndex() = juce::Random::getSystemRandom().nextInt(juce::Range<int>(model.getGrainPosition(), model.getGrainWindowLength() + model.getGrainPosition() - 1));
	}
	else {
		if (currentIndex + 1 >= model.getGrainWindowLength() + model.getGrainPosition())
			model.getGrainCurrentIndex() = model.getGrainPosition();
		else
			model.getGrainCurrentIndex() = currentIndex + 1;
	}
}

void GrainSelector::run()
{
	juce::AudioBuffer<float> tempBuf;
	juce::AudioFormatReader* formatReader;
	std::vector<Grain>& grainStack = model.getWriteGrainQueue();

	double waitTime = -1;
	long long fadein = 0;
	long long fadeout = 0;

	while (!threadShouldExit())
	{
		selectNextGrain();

		formatReader = model.getAudioFormatManager().createReaderFor(model.getWriteGrainstack()[model.getGrainCurrentIndex()]);
		if (formatReader)
		{
			tempBuf = juce::AudioBuffer<float> (2, formatReader->lengthInSamples);
			formatReader->read(&tempBuf, 0, formatReader->lengthInSamples, 0, true, true);	
			formatReader->~AudioFormatReader();

			if (nextGrainStart == 0) nextGrainStart = model.getReadTime();

			//set the start index for the current grain
			int scheduleDelay = 500;
			long long grainStart = nextGrainStart + scheduleDelay;

			//add the buffer to the grain stack
			if (model.getReverse())
				tempBuf.reverse(0, tempBuf.getNumSamples());

			Grain newgrain { tempBuf, int(grainStart), model.getGrainCurrentIndex() };

			if (fadein != 0) newgrain.fadeIn(juce::jmin(tempBuf.getNumSamples()-1, int(fadein)));

			//set the start index for the next grain 
			// next = start pos of this + its duration
			//nextGrainStart = grainStart + duration;
			int duration = (model.getReadSamplerate() / model.getReadDensity());
			nextGrainStart = grainStart + duration*( 1 + (model.getRandomPosition()*(juce::Random::getSystemRandom().nextFloat()*2 -1)));

			//overlap
			if (grainStart + tempBuf.getNumSamples() >= nextGrainStart)
			{
				auto fadeSamples = grainStart + tempBuf.getNumSamples() - nextGrainStart;
				newgrain.fadeOut(fadeSamples);
				fadein = fadeSamples;
			}
			else fadein = 0;

		
			if (model.getWritePos() < grainStack.size())
				grainStack[model.getWritePos()] = std::move(newgrain);
			else model.getWritePos() = 0;

			++model.getWritePos();

			//time to wait
			float grainDuration = float(tempBuf.getNumSamples()) / model.getReadSamplerate();
			waitTime = (grainDuration * 1000)/model.getReadSamplerate();
			//add the schedule error wrt to the current time
			double scheduleErr = float( ( (grainStart - scheduleDelay) - model.getReadTime()) * 1000) / model.getReadSamplerate();
			waitTime += scheduleErr;
		}
		wait(waitTime);
	}

}

