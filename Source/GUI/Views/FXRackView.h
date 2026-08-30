#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"
#include <array>
#include <memory>

class LohUniversalAudioProcessor;

class FXRackView : public juce::Component, private juce::Timer
{
public:
    FXRackView(LohUniversalAudioProcessor& processor);
    ~FXRackView() override { stopTimer(); }
    void paint(juce::Graphics&) override;
    void resized() override;
    void applySkin(const SkinPalette& palette);

private:
    void timerCallback() override;

    struct Stomp
    {
        juce::String name;
        juce::Colour colour;
        HostSlider mix { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        HostSlider extraA { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        HostSlider extraB { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        juce::Label extraALab, extraBLab, mixLab;
        HostToggle on;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAtt, extraAAtt, extraBAtt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onAtt;
        juce::Rectangle<int> bounds;
        int extras = 0;
        bool lastOn = false;
    };

    void setupKnob(HostSlider& sl, const char* paramId);

    LohUniversalAudioProcessor& proc;
    std::array<std::unique_ptr<Stomp>, 6> stomps;

    juce::Label rackTitle, ampTitle, leslieTitle;
    HostComboBox ampBox, rotaryBox;
    HostSlider ampDrive { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    HostSlider ampBass  { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    HostSlider ampTreble{ juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    HostSlider ampCut   { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    HostSlider rotBal   { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label driveLab, bassLab, trebleLab, cutLab, balLab;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampAtt, rotaryAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        driveAtt, bassAtt, trebleAtt, cutAtt, balAtt;

    juce::Rectangle<int> ampPanel, lesliePanel;
    SkinPalette palette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXRackView)
};
