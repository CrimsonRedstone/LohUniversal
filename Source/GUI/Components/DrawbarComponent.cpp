#include "DrawbarComponent.h"
#include "../../Utf8.h"

namespace
{
    juce::Colour capColourForLabel(const juce::String& label)
    {
        const auto l = label.toLowerCase();
        if (l == "m" || l.contains("reed"))
            return juce::Colour(0xffc0392b); // red
        if (l.contains("iv") || l.contains("mixture"))
            return juce::Colour(0xfff0d000); // yellow
        if (l.contains("16") || l == "~" || l.contains("flute"))
            return juce::Colour(0xff8a4a1a); // brown
        if (l.contains("5") || l.contains("2-2") || l.contains("2/3") || l.contains("1-3") || l.contains("1/5")
            || l.contains(u8OneThird()) || l.contains(u8TwoThirds()) || l.contains(u8OneFifth()))
            return juce::Colour(0xff1a1a1a); // black mutations
        return juce::Colour(0xfff2efe6); // ivory 8' 4' 2' 1'
    }
}

DrawbarComponent::DrawbarComponent(juce::AudioProcessorValueTreeState& state,
                                   const juce::StringArray& paramIds,
                                   const juce::StringArray& labels,
                                   bool upperBank)
    : apvts(state), isUpper(upperBank)
{
    jassert(paramIds.size() == labels.size());

    for (int i = 0; i < paramIds.size(); ++i)
    {
        auto bar = std::make_unique<Bar>();
        bar->label = labels[i];
        bar->cap = capColourForLabel(labels[i]);
        bar->slider.setSliderStyle(juce::Slider::LinearVertical);
        bar->slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        bar->slider.setRange(0.0, 1.0, 0.0);
        bar->slider.setSkewFactor(1.0);
        bar->slider.setColour(juce::Slider::thumbColourId, bar->cap);
        bar->slider.setOpaque(false);
        bar->slider.setPopupMenuEnabled(false);
        tagHostParam(bar->slider, paramIds[i]);
        bar->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramIds[i], bar->slider);
        addAndMakeVisible(bar->slider);
        bars.push_back(std::move(bar));
    }

    startTimerHz(24);
    bankTitle = isUpper ? "UPPER" : "LOWER";
}

void DrawbarComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    if (paintBackground)
    {
        g.setColour(palette.faceplate);
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(palette.faceplateHi.withAlpha(0.4f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);
    }

    g.setFont(juce::Font(juce::FontOptions(10.0f)).withStyle(juce::Font::bold));
    g.setColour(palette.muted);
    g.drawText(bankTitle, getLocalBounds().removeFromTop(16),
               juce::Justification::centred, false);

    const int n = (int) bars.size();
    if (n == 0)
        return;

    auto area = getLocalBounds().reduced(6, 18);
    const int colW = area.getWidth() / n;

    for (int i = 0; i < n; ++i)
    {
        auto col = area.removeFromLeft(colW);
        auto labelArea = col.removeFromBottom(18);
        auto slot = col.reduced(col.getWidth() / 3, 2).toFloat();

        const float v = (float) bars[(size_t) i]->slider.getValue();

        if (uiStyle == DrawbarUiStyle::KorgLed)
        {
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(slot, 3.0f);
            const int segs = 9;
            const float segH = slot.getHeight() / (float) segs;
            const int lit = juce::jlimit(0, segs, juce::roundToInt(v * (float) segs));
            for (int s = 0; s < segs; ++s)
            {
                auto cell = juce::Rectangle<float>(slot.getX() + 2.0f,
                                                   slot.getBottom() - (float) (s + 1) * segH + 1.0f,
                                                   slot.getWidth() - 4.0f, segH - 2.0f);
                const bool on = s < lit;
                g.setColour(on ? (s >= 7 ? juce::Colour(0xffff5533) : palette.ledOn)
                               : palette.ledOff);
                g.fillRoundedRectangle(cell, 1.5f);
                if (on)
                {
                    g.setColour(juce::Colours::white.withAlpha(0.25f));
                    g.fillRoundedRectangle(cell.removeFromTop(cell.getHeight() * 0.35f), 1.0f);
                }
            }
        }
        else
        {
            g.setColour(palette.muted.withAlpha(0.5f));
            for (int t = 0; t <= 8; ++t)
            {
                const float y = slot.getY() + slot.getHeight() * (1.0f - (float) t / 8.0f);
                g.drawHorizontalLine((int) y, slot.getRight() + 2.0f, slot.getRight() + 6.0f);
            }
        }

        g.setColour(palette.text);
        g.setFont(juce::Font(juce::FontOptions(10.0f)).withStyle(juce::Font::bold));
        g.drawText(bars[(size_t) i]->label, labelArea, juce::Justification::centred, false);
    }
}

void DrawbarComponent::resized()
{
    const int n = (int) bars.size();
    if (n == 0)
        return;

    auto area = getLocalBounds().reduced(6, 18);
    area.removeFromBottom(18);
    const int colW = area.getWidth() / n;
    for (int i = 0; i < n; ++i)
    {
        auto col = area.removeFromLeft(colW);
        bars[(size_t) i]->slider.setBounds(col.reduced(2, 0));
        if (uiStyle == DrawbarUiStyle::KorgLed)
            bars[(size_t) i]->slider.setAlpha(0.0f);
        else
            bars[(size_t) i]->slider.setAlpha(1.0f);
    }
}
