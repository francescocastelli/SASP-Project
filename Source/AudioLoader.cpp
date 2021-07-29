/*
  ==============================================================================

    AudioLoader.cpp
    Created: 20 Mar 2021 3:09:28pm
    Author:  Francesco

  ==============================================================================
*/

#include "..\Include\AudioLoader.h"

AudioLoader::AudioLoader(Model& model)
	:model (model)
{
}

void AudioLoader::loadInputFile()
{
	juce::FileChooser chooser("Select an audio file...", {}, "*.wav;*.mp3;*.flac");                                     
    
	if (chooser.browseForFileToOpen())                                          
	{
		auto file = chooser.getResult();                                        
		auto* reader = model.getAudioFormatManager().createReaderFor(file);                    

		//set the sample rate on the model
		model.getWriteSamplerate() = reader->sampleRate;

		if (reader != nullptr)
		{
			std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true)); 
			model.getTransportSource().setSource(newSource.get(), 0, nullptr, model.getReadSamplerate());                                
			model.getAudioThumbnail().setSource(new juce::FileInputSource(file));
			readerSource.reset(newSource.release());     
		}
    }
}

void AudioLoader::loadGrainFile()
{
	//TODO delete the grains already present

	model.getWriteGrainstack().clear();
	//get the number of files in the dir 
	for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(juce::File(model.getGrainDirectory().getFullPathName()), false))
	{
		model.getWriteGrainstack().add(entry.getFile());
	}
}
