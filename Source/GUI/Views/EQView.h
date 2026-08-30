#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"
#include <array>
#include <memory>

class LohUniversalAudioProcessor;

class EQView : public juce::Component
{
public:
    EQView(LohUniversalAudioProcessor& processor);
    void paint(juce::Graphics&) override;
    void resized() override;
    void applySkin(const SkinPalette& palette);

private:
    LohUniversalAudioProcessor& proc;
    juce::Label title;
    HostToggle eqOn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqOnAtt;
    std::array<HostSlider, 5> bands;
    std::array<juce::Label, 5> freqLabs;
    std::array<juce::Label, 5> dbLabs;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 5> bandAtt;
    SkinPalette palette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQView)
};
