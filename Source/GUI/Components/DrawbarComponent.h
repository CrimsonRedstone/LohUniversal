#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Parameters/ParameterLayout.h"
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../HostAutomation.h"

class DrawbarComponent : public juce::Component,
                         private juce::Timer
{
public:
    DrawbarComponent(juce::AudioProcessorValueTreeState& apvts,
                     const juce::StringArray& paramIds,
                     const juce::StringArray& labels,
                     bool upperBank);

    void paint(juce::Graphics&) override;
    void resized() override;
    void setUiStyle(int style) noexcept { uiStyle = style; resized(); repaint(); }
    void setPalette(const SkinPalette& p) noexcept { palette = p; repaint(); }
    void setBankTitle(const juce::String& t) { bankTitle = t; repaint(); }
    void setPaintBackground(bool should) noexcept { paintBackground = should; repaint(); }

private:
    void timerCallback() override { repaint(); }

    struct Bar
    {
        HostSlider slider { juce::Slider::LinearVertical, juce::Slider::NoTextBox };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        juce::String label;
        juce::Colour cap;
    };

    juce::AudioProcessorValueTreeState& apvts;
    std::vector<std::unique_ptr<Bar>> bars;
    SkinPalette palette;
    int uiStyle = DrawbarUiStyle::VintageMetal;
    bool isUpper = true;
    bool paintBackground = true;
    juce::String bankTitle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrawbarComponent)
};
