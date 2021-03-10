#include "PositionOverlayComponent.h"
#include "AppColors.h"

 PositionOverlayComponent::PositionOverlayComponent(juce::AudioTransportSource& transportSourceToUse )
    : transportSource(transportSourceToUse)
{
    startTimer(40);
}

void PositionOverlayComponent::paint(juce::Graphics& g) 
{
    auto duration = (float)transportSource.getLengthInSeconds();

    if (duration > 0.0)
    {
        auto audioPosition = (float)transportSource.getCurrentPosition();
        auto drawPosition = (audioPosition / duration) * (float)getWidth();
        auto rectWidth = (0.1 / duration) * (float)getWidth();

        g.setColour(AppColours::positionOverlayBar);
        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
        g.setColour(AppColours::positionOverlay);
        g.fillRoundedRectangle(drawPosition - rectWidth/2, 0.0f, rectWidth, getHeight(), 1.0f);
    }
}

void PositionOverlayComponent::mouseDrag(const juce::MouseEvent& event) 
{
    mouseSelection(event);
}

void PositionOverlayComponent::mouseDown(const juce::MouseEvent& event) 
{
    mouseSelection(event);
}

void PositionOverlayComponent::timerCallback() 
{
    repaint();
}

void PositionOverlayComponent::mouseSelection(const juce::MouseEvent& event)
{
    auto duration = transportSource.getLengthInSeconds();

    if (duration > 0.0)
    {
        auto clickPosition = event.position.x;
        auto audioPosition = (clickPosition / (float)getWidth()) * duration;

        transportSource.setPosition(audioPosition);
    }
}