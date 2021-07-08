#include "..\Include\PositionOverlayComponent.h"

 PositionOverlayComponent::PositionOverlayComponent(Model& model)
     :model(model),
     enabled(false)
{
   startTimer(40);
}

void PositionOverlayComponent::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void PositionOverlayComponent::paint(juce::Graphics& g) 
{
    if (enabled)
    {
        auto duration = (float)model.getTransportSource().getLengthInSeconds();

        if (duration > 0.0)
        {
            auto audioPosition = (float)model.getTransportSource().getCurrentPosition();
            auto drawPosition = (audioPosition / duration) * (float)getWidth();
            auto rectWidth = float((model.getWriteGrainLength() / duration)) * (float)getWidth();

            g.setColour(AppColours::positionOverlayBar);
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
            g.setColour(AppColours::positionOverlay);
            g.fillRoundedRectangle(drawPosition - rectWidth / 2, 0.0f, rectWidth, getHeight(), 1.0f);
        }
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
    if (enabled)
    {
        auto duration = model.getTransportSource().getLengthInSeconds();

        if (duration > 0.0)
        {
            auto clickPosition = event.position.x;
            auto audioPosition = (clickPosition / (float)getWidth()) * duration;

            if (audioPosition >= duration)
                audioPosition = duration;
            else if (audioPosition < 0)
                audioPosition = 0;

            model.getTransportSource().setPosition(audioPosition);
        }
    }
}

