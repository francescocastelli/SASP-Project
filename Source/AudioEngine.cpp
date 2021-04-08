/*
  ==============================================================================

    AudioEngine.cpp
    Created: 20 Mar 2021 1:08:58pm
    Author:  Francesco

  ==============================================================================
*/

#include "AudioEngine.h"

AudioEngine::AudioEngine(Model& model)
	:model (model),
	fftVisualizer(nullptr)
{

}

//-------------------------------------------------------------
void AudioEngine::setfftVisualizer(SpectrogramComponent* fftVisualizer)
{
	this->fftVisualizer = fftVisualizer;
}

void AudioEngine::releaseResources()
{
	delete(fftVisualizer);
	fftVisualizer = nullptr;
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	model.getTransportSource().prepareToPlay(samplesPerBlockExpected, sampleRate);

	//prepare filter and reverb
	//set up reverb
	juce::Reverb::Parameters param = { 0.3f, 0.1f, 0.33f, 0.4f, 1.5f, 0.f };
	model.getWriteReverb().setParameters(param);
	model.getWriteReverb().setSampleRate(sampleRate);

	//set up the filter
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlockExpected;
	spec.numChannels = 2;

	model.getWriteFilter().prepare(spec);
	model.getWriteFilter().reset();
}

void AudioEngine::processNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (model.getAudioState() == ModelAudioState::grainPlay)
	{
		std::atomic<long long>& time = model.getWriteTime();
		int currentWrite = model.getWritePos();

		if (model.getReadPos() >= currentWrite)
			currentWrite = model.getWriteGrainQueue().size();

		for (int i = 0; i < bufferToFill.numSamples; ++i)
		{
			for (int j = 0; j < currentWrite-model.getReadPos(); ++j)
				if (model.getWriteGrainQueue()[model.getReadPos()+j].canPlay(time))
					model.getWriteGrainQueue()[model.getReadPos()+j].processBlock(bufferToFill, time);

			++time;

			if (model.getWriteGrainQueue()[model.getReadPos()].isFinished())
				model.getReadPos() = (model.getReadPos() + 1) % model.getWriteGrainQueue().size();
		}

		//filtering 
		model.updateFilterCoeff();

		if (!model.getNoFiltering())
		{
			juce::dsp::AudioBlock<float> currentBlock(*bufferToFill.buffer);
			model.getWriteFilter().process(juce::dsp::ProcessContextReplacing<float>(currentBlock));
		}

		//apply some reverb to the output
		model.getWriteReverb().processStereo(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1), bufferToFill.numSamples);

		//apply gain at the output sound
		bufferToFill.buffer->applyGain(model.getWriteGain());

		//fft visualizer
		fftVisualizer->setNextAudioBlock(bufferToFill);
	}
	else if (model.getAudioState() == ModelAudioState::selectionPlay)
	{
		model.getTransportSource().getNextAudioBlock(bufferToFill);
		bufferToFill.buffer->applyGain(model.getWriteGain());
	}
	else bufferToFill.clearActiveBufferRegion();

}


