#include "TrimPotComponent.h"

TrimPotComponent::TrimPotComponent(juce::AudioProcessorValueTreeState& apvts,
                                   const juce::String& paramId,
                                   const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setRotaryParameters(juce::degreesToRadians(225.0f),
                               juce::degreesToRadians(495.0f), true);
    slider.getProperties().set("trimPot", true);
    slider.setPopupMenuEnabled(false);
    tagHostParam(slider, paramId);
    addAndMakeVisible(slider);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramId, slider);

    caption.setText(labelText, juce::dontSendNotification);
    caption.setJustificationType(juce::Justification::centred);
    caption.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(caption);
}

void TrimPotComponent::resized()
{
    auto r = getLocalBounds();
    caption.setBounds(r.removeFromBottom(16));
    slider.setBounds(r.reduced(2));
}

void TrimPotComponent::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(palette.faceplate.withAlpha(0.35f));
    g.fillRoundedRectangle(r, 4.0f);
}
