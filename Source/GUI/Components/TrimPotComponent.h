#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"

class TrimPotComponent : public juce::Component
{
public:
    TrimPotComponent(juce::AudioProcessorValueTreeState& apvts,
                     const juce::String& paramId,
                     const juce::String& labelText);

    void resized() override;
    void paint(juce::Graphics&) override;
    void setPalette(const SkinPalette& p) noexcept { palette = p; caption.setColour(juce::Label::textColourId, p.text); repaint(); }

private:
    HostSlider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label caption;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    SkinPalette palette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrimPotComponent)
};
