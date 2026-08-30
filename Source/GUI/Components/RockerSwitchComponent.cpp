#include "RockerSwitchComponent.h"

RockerSwitchComponent::RockerSwitchComponent(juce::AudioProcessorValueTreeState& apvts,
                                             const juce::String& paramId,
                                             const juce::String& labelText)
{
    button.setButtonText({});
    button.setClickingTogglesState(true);
    button.setAlpha(0.0f);
    button.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(button);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramId, button);

    caption.setText(labelText, juce::dontSendNotification);
    caption.setJustificationType(juce::Justification::centred);
    caption.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(caption);
    tagHostParam(*this, paramId);
    tagHostParam(button, paramId);

    button.onStateChange = [this] { repaint(); };
}

void RockerSwitchComponent::resized()
{
    auto r = getLocalBounds();
    caption.setBounds(r.removeFromBottom(16));
    button.setBounds(r.reduced(4, 2));
}

void RockerSwitchComponent::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu() && showHostParamMenu(*this, e.getScreenPosition()))
        return;
    pressed = true;
    button.setToggleState(! button.getToggleState(), juce::sendNotification);
    repaint();
}

void RockerSwitchComponent::mouseUp(const juce::MouseEvent&)
{
    pressed = false;
    repaint();
}

void RockerSwitchComponent::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    auto body = r;
    body.removeFromBottom(16.0f);
    body = body.reduced(6.0f, 3.0f);

    const bool on = button.getToggleState();

    // Recess
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.fillRoundedRectangle(body.expanded(3.0f), 4.0f);

    const float tilt = on ? 0.18f : -0.18f;
    juce::Path rocker;
    auto top = body.removeFromTop(body.getHeight() * (0.5f + tilt));
    auto bot = body;

    juce::Colour upCol   = palette.metal.brighter(on ? 0.05f : 0.25f);
    juce::Colour downCol = palette.metal.darker(on ? 0.35f : 0.1f);
    if (pressed)
    {
        upCol = upCol.darker(0.1f);
        downCol = downCol.darker(0.1f);
    }

    g.setColour(on ? downCol : upCol);
    g.fillRoundedRectangle(top, 3.0f);
    g.setColour(on ? upCol : downCol);
    g.fillRoundedRectangle(bot, 3.0f);

    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.drawLine(top.getX() + 2.0f, top.getBottom(), top.getRight() - 2.0f, top.getBottom(), 1.2f);

    // Indicator lamp
    auto lamp = juce::Rectangle<float>(10.0f, 10.0f).withCentre({ r.getCentreX(), r.getY() + 9.0f });
    g.setColour(on ? palette.lampOn : palette.lampOff);
    g.fillEllipse(lamp);
    if (on)
    {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillEllipse(lamp.reduced(3.0f));
    }

    g.setColour(palette.text);
}
