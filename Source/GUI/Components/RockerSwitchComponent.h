#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"

class RockerSwitchComponent : public juce::Component
{
public:
    RockerSwitchComponent(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& paramId,
                          const juce::String& labelText);

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    void setPalette(const SkinPalette& p) noexcept { palette = p; repaint(); }

private:
    juce::ToggleButton button;
    juce::Label caption;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    SkinPalette palette;
    bool pressed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RockerSwitchComponent)
};
