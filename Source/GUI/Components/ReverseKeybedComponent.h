#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include "../LookAndFeel/VintageVoxLookAndFeel.h"
#include "../../Parameters/ParameterLayout.h"

class ReverseKeybedComponent : public juce::Component,
                               private juce::Timer
{
public:
    ReverseKeybedComponent();

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    void setPalette(const SkinPalette& p) noexcept { palette = p; repaint(); }
    void setManualLabel(const juce::String& s) { caption = s; repaint(); }
    void setNoteCallback(std::function<void(int, bool)> cb) { onNote = std::move(cb); }
    void setHeldQuery(std::function<bool(int)> q) { isHeld = std::move(q); }

private:
    void timerCallback() override { repaint(); }
    int noteAt(juce::Point<float> pos) const noexcept;
    void press(int note);
    void release(int note);

    SkinPalette palette;
    juce::String caption;
    std::function<void(int, bool)> onNote;
    std::function<bool(int)> isHeld;
    int mouseNote = -1;
    static constexpr int kStart = kLowestKey;
    static constexpr int kEnd = kHighestKey;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverseKeybedComponent)
};
