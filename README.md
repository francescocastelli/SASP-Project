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

Scheduling of the grains is based on a global timer that is incremented at the rate of the playback time and used as a global time reference. The grain selector thread read the next grain from the stack of grains and loads it into the circular buffer. When a grain is loaded, we also set its starting time with reference to the global timer. In this way we can schedule grains ahed of time and also control the amount of scatters, density of grains and overlap (since grains can play simultaneously, overlap and add is used to adjust the amplitude of the overlapping region). 

Then is the audio engine that has the resposibility of play back grains from the circular buffer of loaded grains based on their starting time.

### Audio Engine

The audio engine writes the audio in output using the method processNextAudioBlock. This method receives a buffer of a certain length to be filled with the audio sample, and it's called basically at the audio sample rate. So it's  really important that every operation permformed inside here is fast, since we can't miss a block in the ouput stream. 

For this reason all the sensible data accessed here are implemented using std::atomic, no mutex or lock are used and only simple operation are performed. 

The audio engine is responsible for reading the grains from the  circular buffer after the grain selector thread placed them. Then the content of the grain is copyed on the current output block.

## GUI

<img width="897" alt="full-app" src="https://user-images.githubusercontent.com/12759012/126700928-d428d75e-fbd6-4ae5-9f20-bf485d3f4841.png">

The GUI can be splitted into two main regions

### Top part

<img width="876" alt="top" src="https://user-images.githubusercontent.com/12759012/126701116-bf7048a4-fc47-4141-9cb2-bf5e501fbb8f.png">

The top part of the app is used for loading an audio file and selecting grains from it. It allows the used to play and stop the audio, select a grain with a specified length and apply some modest pre-processing like windowing and fade in/out. Once a grain is selected and preprocessed the user can save it into the grain folder in order to play back it later. There is also the possibility of selecting grain automatically in sequential order with the pre-processing currently selected.

### Bottom part 

<img width="872" alt="bottom" src="https://user-images.githubusercontent.com/12759012/126701241-988d2024-9a57-47b3-8320-a293a79ddabd.png">

The bottom part instead is used for the play back of the saved grains. It allows the user to load grains from the folder and play around with the various parameters. It also gives the possibility of applying some filtering effect to the output sound. 

