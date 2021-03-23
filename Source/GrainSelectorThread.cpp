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

	double waitTime = -1;
	long long fadein = 0;
	long long fadeout = 0;

	while (!threadShouldExit())
	{
		model.getMutex().lock();
		std::deque<Grain>& grainStack = model.getWriteGrainQueue();

		//remove the grains already played
		if (grainStack.size() > 0)
			grainStack.erase(std::remove_if(grainStack.begin(), grainStack.end(), [this](Grain x) {return x.hasEnded(model.getReadTime()); }), grainStack.end());

		model.getMutex().unlock();

		//read the buffer for the current grain
		//TODO implement next grain selection
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

			model.getMutex().lock();

			//add the buffer to the grain stack
			if (model.getReverse())
				tempBuf.reverse(0, tempBuf.getNumSamples());

			grainStack.push_back((Grain(tempBuf, grainStart, model.getGrainCurrentIndex())));

			if (fadein != 0) grainStack.back().fadeIn(fadein);
			model.getMutex().unlock();

			//set the start index for the next grain 
			// next = start pos of this + its duration
			//nextGrainStart = grainStart + duration;
			int duration = (model.getReadSamplerate() / model.getReadDensity());
			nextGrainStart = grainStart + duration*( 1 + (model.getRandomPosition()*(juce::Random::getSystemRandom().nextFloat()*2 -1)));

			//overlap
			if (grainStart + tempBuf.getNumSamples() >= nextGrainStart)
			{
				model.getMutex().lock();
				auto fadeSamples = grainStart + tempBuf.getNumSamples() - nextGrainStart;
				grainStack.back().fadeOut(fadeSamples);
				model.getMutex().unlock();
				fadein = fadeSamples;
			}
			else fadein = 0;

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

