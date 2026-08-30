#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Components/DrawbarComponent.h"
#include "../Components/ReverseKeybedComponent.h"
#include "../Components/RockerSwitchComponent.h"
#include "../Components/TrimPotComponent.h"
#include "../LookAndFeel/VintageVoxLookAndFeel.h"

class LohUniversalAudioProcessor;

class MainPanelView : public juce::Component
{
public:
    MainPanelView(LohUniversalAudioProcessor& processor);
    void paint(juce::Graphics&) override;
    void resized() override;
    void applySkin(const SkinPalette& palette, int drawbarStyle);

private:
    LohUniversalAudioProcessor& proc;

    DrawbarComponent upperBars;
    DrawbarComponent lowerBars;
    DrawbarComponent mixtureBar;
    DrawbarComponent fluteReed;

    ReverseKeybedComponent keys;

    RockerSwitchComponent mains;
    RockerSwitchComponent vibrato;
    RockerSwitchComponent perc;
    RockerSwitchComponent ledMode;

    juce::ComboBox percVolBox;
    juce::Label percVolLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> percVolAtt;

    SkinPalette palette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainPanelView)
};
