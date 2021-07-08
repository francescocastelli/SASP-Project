# GRANULAR SYNTHESIZER

Stand-alone application for granulary synthesis written in C++ using the juce framework. This is part of a project for the SASP course at Polimi.

## What is granular synthesis? 

**Granulation is a process in which an audio sample is broken down into tiny segments of audio.** 

These segments are called grains, brief microacoustic events with a duration near the threshold of human auditory perception. Each grain contains a waveform shaped by an amplitude envelope and serves as a building block for creating a more complex sound object.

With granular synthesis we can capture the two perceptual dimension of sounds: time-domain information and frequency-domain information. 

There are different possible ways of organize these grains globally: pitch-synchronous overlapping streams, synchronous organization, physical or abstract model, etc... . Here we concentrate more on asynchronous grains organization.

 *[Microsound, Curtis Road]*

### Asynchronous granular synthesis

It scatters the grains over a specified duration within regions inscribed on the time-frequency plane, which are called clouds. The scattering of grains in time is irregular, and the composer can control a cloud by specifing some parameters: 
 - Grain duration
 - Amplitude envelope 
 - Density of the grains per second
 - Reverse grain
 
## Implementation

The project is written in C++ with the help of the [juce framework](https://juce.com/).  It follows a MVC paradigm for the organization of the code, where all the data are store in the model, modify by the controller and read by the view for the display.

### Grain selection 

The selection of grains is implemented using a thread that loads ahead of time the next grains. Grains are not pre-loaded for saving memory, so we keep a stack of reference to the grain files and load them on the fly when needed. Grains that are loaded are organized into a circular buffer, where the grain selector thread writes them ahead of time.

### Grain scheduling 

### Audio Engine

The audio engine writes the audio in output using the method processNextAudioBlock. This method receives a buffer of a certain length to be filled with the audio sample, and it's called basically at the audio sample rate. So it's  really important that every operation permformed inside here is fast, since we can't miss a block in the ouput stream. 

For this reason all the sensible data accessed here are implemented using std::atomic, no mutex or lock are used and only simple operation are performed. 

The audio engine is responsible for reading the grains from the  circular buffer after the grain selector thread placed them. Then the content of the grain is copyed on the current output block. Since grains can play simultaneously, overlap and add is used to adjust the amplitude of the overlapping region.

## GUI

