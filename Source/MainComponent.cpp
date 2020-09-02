#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : inputAudioState(Stopped),
      thumbnailCache(5),
      inputWaveform (512, formatManager, thumbnailCache, transportSource), thumb(512, formatManager, thumbnailCache)
{

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels (0, 2);
    }

    addAndMakeVisible(inputWaveform);
    
    //initialize buttons
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open");
    openButton.onClick = [this] { openButtonClicked(); };
  
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);
    
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    addAndMakeVisible(&selectionButton);
    selectionButton.setButtonText("Select grain");
    selectionButton.onClick = [this] { selectionButtonClicked(); };
    selectionButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightsalmon);
    selectionButton.setEnabled(false);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);

    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource) changeState(transportSource.isPlaying() ? Playing : Stopped);
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //juce::Rectangle<int> thumbnailBounds(10, 150, getWidth() - 250, 10);

    //g.fillAll(juce::Colours::lightgrey);
    //g.setColour(juce::Colours::black);

    //thumb.drawChannels(g, thumbnailBounds, 0.0, thumb.getTotalLength(), 6.0f);
}

void MainComponent::resized()
{
    openButton.setBoundsRelative(0.82f, 0.05f, 0.05f, 0.03f);
    playButton.setBoundsRelative(0.88f, 0.05f, 0.05f, 0.03f);
    stopButton.setBoundsRelative(0.94f, 0.05f, 0.05f, 0.03f);
    selectionButton.setBoundsRelative(0.82f, 0.1f, 0.125f, 0.03f);

    inputWaveform.setBoundsRelative(0.02f, 0.05f, 0.75f, 0.1f);
}

//==============================================================================

void MainComponent::changeState(SoundState newState)
{

    if (inputAudioState != newState)
    {
        inputAudioState = newState;
        
        switch (inputAudioState)
        {
        case Stopped:
            stopButton.setEnabled(false);
            selectionButton.setEnabled(true);
            playButton.setEnabled(true);
            transportSource.setPosition(0.0);
            break;

        case Starting:
            playButton.setEnabled(false);
            selectionButton.setEnabled(false);
            transportSource.start();
            break;

        case Playing:
            stopButton.setEnabled(true);
            break;

        case Stopping:
            transportSource.stop();
            break;
        }

    }
}

void MainComponent::openButtonClicked()
{
    juce::FileChooser chooser("Select a Wave file to play...", {}, "*.wav");                                     

    if (chooser.browseForFileToOpen())                                          
    {
        auto file = chooser.getResult();                                        
        auto* reader = formatManager.createReaderFor(file);                    

        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true)); 
            sampleRate = reader->sampleRate;
            grainBuffer = juce::AudioBuffer<float>(2, sampleRate * GRAINLENGTH);
            transportSource.setSource(newSource.get(), 0, nullptr, sampleRate);                                
            playButton.setEnabled(true);          
            selectionButton.setEnabled(true);
            inputWaveform.setFile(file);
            readerSource.reset(newSource.release());     
            changeState(Stopped);
        }
    }
}

void MainComponent::playButtonClicked()
{
    changeState(Starting);
}

void MainComponent::stopButtonClicked()
{
    changeState(Stopping);
}

void MainComponent::selectionButtonClicked()
{
    auto currentPos = transportSource.getCurrentPosition();
    juce::Rectangle<int> bound(1, 100, 300, 150);

    inputWaveform.grainSelection(currentPos - 0.1, currentPos + 0.1,bound );
}
