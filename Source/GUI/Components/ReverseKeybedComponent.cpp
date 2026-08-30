#include "ReverseKeybedComponent.h"

namespace
{
    bool isNatural(int midi)
    {
        const int pc = midi % 12;
        return pc == 0 || pc == 2 || pc == 4 || pc == 5 || pc == 7 || pc == 9 || pc == 11;
    }

    int numNaturals()
    {
        int n = 0;
        for (int k = kLowestKey; k <= kHighestKey; ++k)
            if (isNatural(k)) ++n;
        return n;
    }
}

ReverseKeybedComponent::ReverseKeybedComponent()
{
    startTimerHz(30);
}

void ReverseKeybedComponent::resized() {}

int ReverseKeybedComponent::noteAt(juce::Point<float> pos) const noexcept
{
    const int nNat = numNaturals();
    const float natW = (float) getWidth() / (float) nNat;
    const float sharpW = natW * 0.62f;
    const float sharpH = (float) getHeight() * 0.58f;

    if (pos.y < sharpH)
    {
        int natI = 0;
        for (int n = kStart; n <= kEnd; ++n)
        {
            if (! isNatural(n))
            {
                const float left = (float) natI * natW - sharpW * 0.5f;
                if (pos.x >= left && pos.x < left + sharpW)
                    return n;
            }
            else
            {
                ++natI;
            }
        }
    }

    int idx = juce::jlimit(0, nNat - 1, (int) (pos.x / natW));
    int seen = 0;
    for (int n = kStart; n <= kEnd; ++n)
    {
        if (isNatural(n))
        {
            if (seen == idx) return n;
            ++seen;
        }
    }
    return kStart;
}

void ReverseKeybedComponent::press(int note)
{
    if (note == mouseNote) return;
    if (mouseNote >= 0 && onNote) onNote(mouseNote, false);
    mouseNote = note;
    if (onNote) onNote(note, true);
}

void ReverseKeybedComponent::release(int note)
{
    if (onNote) onNote(note, false);
    if (mouseNote == note) mouseNote = -1;
}

void ReverseKeybedComponent::mouseDown(const juce::MouseEvent& e) { press(noteAt(e.position)); }
void ReverseKeybedComponent::mouseDrag(const juce::MouseEvent& e) { press(noteAt(e.position)); }
void ReverseKeybedComponent::mouseUp(const juce::MouseEvent&)
{
    if (mouseNote >= 0) release(mouseNote);
}

void ReverseKeybedComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    if (caption.isNotEmpty())
    {
        g.setColour(palette.muted);
        g.setFont(juce::Font(juce::FontOptions(10.0f)).withStyle(juce::Font::bold));
        g.drawText(caption, area.removeFromTop(14), juce::Justification::centredLeft, false);
    }

    g.setColour(palette.keyBed);
    g.fillRoundedRectangle(area.toFloat(), 4.0f);

    const int nNat = numNaturals();
    const float natW = (float) area.getWidth() / (float) nNat;
    const float top = (float) area.getY();
    const float h = (float) area.getHeight();

    int natI = 0;
    for (int n = kStart; n <= kEnd; ++n)
    {
        if (! isNatural(n)) continue;
        auto r = juce::Rectangle<float>(area.getX() + natI * natW + 0.5f, top, natW - 1.0f, h - 1.0f);
        const bool held = isHeld ? isHeld(n) : false;
        const auto hi = palette.naturalKey.brighter(held ? 0.28f : 0.12f);
        g.setGradientFill(juce::ColourGradient(hi, r.getX(), r.getY(),
                                               palette.naturalKey, r.getX(), r.getBottom(), false));
        g.fillRoundedRectangle(r, 3.0f);
        g.setColour(juce::Colours::black.withAlpha(0.45f));
        g.drawRoundedRectangle(r, 3.0f, 0.6f);
        g.setColour(juce::Colours::white.withAlpha(0.07f));
        g.fillRect(r.removeFromTop(3.0f).reduced(1.0f, 0.0f));
        if (n % 12 == 0)
        {
            g.setColour(palette.muted.withAlpha(0.45f));
            g.setFont(juce::Font(juce::FontOptions(8.0f)));
            g.drawText("C" + juce::String((n / 12) - 1),
                       juce::Rectangle<float>(r.getX(), r.getBottom() - 14.0f, r.getWidth(), 14.0f),
                       juce::Justification::centred, false);
        }
        ++natI;
    }

    natI = 0;
    for (int n = kStart; n <= kEnd; ++n)
    {
        if (isNatural(n)) { ++natI; continue; }
        const float left = (float) area.getX() + (float) natI * natW - natW * 0.31f;
        auto r = juce::Rectangle<float>(left, top, natW * 0.62f, h * 0.58f);
        const bool held = isHeld ? isHeld(n) : false;
        g.setColour(juce::Colours::black.withAlpha(0.45f));
        g.fillRoundedRectangle(r.translated(0, 3.0f), 2.5f);
        g.setGradientFill(juce::ColourGradient(
            held ? palette.sharpKey.darker(0.08f) : palette.sharpKey.brighter(0.08f),
            r.getX(), r.getY(),
            palette.sharpKey.darker(0.22f), r.getX(), r.getBottom(), false));
        g.fillRoundedRectangle(r, 2.5f);
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.drawRoundedRectangle(r, 2.5f, 0.8f);
    }
}
