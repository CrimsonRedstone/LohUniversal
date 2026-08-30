#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Components/TrimPotComponent.h"
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"
#include <array>
#include <memory>

class LohUniversalAudioProcessor;

class OpenLidView : public juce::Component
{
public:
    OpenLidView(LohUniversalAudioProcessor& processor);
    void paint(juce::Graphics&) override;
    void resized() override;
    void applySkin(const SkinPalette& palette);

private:
    LohUniversalAudioProcessor& proc;
    std::array<std::unique_ptr<TrimPotComponent>, 12> oscPots;
    std::unique_ptr<TrimPotComponent> clickPot, bleedPot, driftPot;
    std::unique_ptr<TrimPotComponent> percDecayPot, vibSpeedPot, vibDepthPot;
    HostSlider splitSlider;
    HostComboBox percHarmBox;
    juce::Label splitLabel, titleLabel, hintLabel, percHarmLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> splitAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> percHarmAtt;
    SkinPalette palette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenLidView)
};
