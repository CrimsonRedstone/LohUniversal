#include "EQView.h"
#include "../../PluginProcessor.h"
#include "../../Utf8.h"

EQView::EQView(LohUniversalAudioProcessor& p)
    : proc(p)
{
    title.setText("5-BAND EQUALIZER  " + u8MidDot() + "  POST AMP / FX", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    eqOn.setButtonText("EQ IN");
    eqOn.setClickingTogglesState(true);
    tagHostParam(eqOn, ParamIDs::EQ_ON);
    addAndMakeVisible(eqOn);
    eqOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        proc.apvts, ParamIDs::EQ_ON, eqOn);
    eqOn.onStateChange = [this] { repaint(); };

    const char* freq[5] = { "80", "250", "800", "2.5k", "8k" };
    for (int i = 0; i < 5; ++i)
    {
        bands[(size_t) i].setSliderStyle(juce::Slider::LinearVertical);
        bands[(size_t) i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52, 18);
        bands[(size_t) i].setPopupMenuEnabled(false);
        tagHostParam(bands[(size_t) i], kEqGainIds[i]);
        addAndMakeVisible(bands[(size_t) i]);
        bandAtt[(size_t) i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            proc.apvts, kEqGainIds[i], bands[(size_t) i]);

        freqLabs[(size_t) i].setText(freq[i], juce::dontSendNotification);
        freqLabs[(size_t) i].setJustificationType(juce::Justification::centred);
        freqLabs[(size_t) i].setInterceptsMouseClicks(false, false);
        addAndMakeVisible(freqLabs[(size_t) i]);

        dbLabs[(size_t) i].setText("dB", juce::dontSendNotification);
        dbLabs[(size_t) i].setJustificationType(juce::Justification::centred);
        dbLabs[(size_t) i].setInterceptsMouseClicks(false, false);
        addAndMakeVisible(dbLabs[(size_t) i]);
    }
}

void EQView::applySkin(const SkinPalette& p)
{
    palette = p;
    title.setColour(juce::Label::textColourId, p.text);
    for (int i = 0; i < 5; ++i)
    {
        freqLabs[(size_t) i].setColour(juce::Label::textColourId, p.text);
        dbLabs[(size_t) i].setColour(juce::Label::textColourId, p.muted);
    }
    repaint();
}

void EQView::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour(palette.chassisDark.interpolatedWith(juce::Colours::black, 0.45f));
    g.fillRect(r);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(0, 36, getWidth(), 12);
    g.fillRect(0, getHeight() - 18, getWidth(), 12);
    g.setColour(juce::Colour(0xff8a8680));
    for (int x = 24; x < getWidth(); x += 48)
    {
        g.fillEllipse((float) x, 38.0f, 8.0f, 8.0f);
        g.fillEllipse((float) x, (float) getHeight() - 16.0f, 8.0f, 8.0f);
    }

    auto panel = getLocalBounds().reduced(28, 58).toFloat();
    g.setColour(palette.faceplate);
    g.fillRoundedRectangle(panel, 10.0f);
    g.setColour(palette.accent.withAlpha(0.35f));
    g.drawRoundedRectangle(panel, 10.0f, 1.2f);

    const bool on = eqOn.getToggleState();
    g.setColour(on ? juce::Colour(0xffff3333) : juce::Colour(0xff3a1010));
    g.fillEllipse(panel.getX() + 22.0f, panel.getY() + 18.0f, 14.0f, 14.0f);
    if (on)
    {
        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.fillEllipse(panel.getX() + 25.0f, panel.getY() + 20.0f, 8.0f, 8.0f);
    }
}

void EQView::resized()
{
    auto r = getLocalBounds().reduced(16);
    title.setBounds(r.removeFromTop(28));
    r.removeFromTop(16);
    r.removeFromBottom(10);

    auto body = r.reduced(20, 8);
    auto left = body.removeFromLeft(120);
    eqOn.setBounds(left.removeFromTop(48).reduced(8, 4));

    const int colW = body.getWidth() / 5;
    for (int i = 0; i < 5; ++i)
    {
        auto col = body.removeFromLeft(colW).reduced(10, 4);
        freqLabs[(size_t) i].setBounds(col.removeFromTop(22));
        dbLabs[(size_t) i].setBounds(col.removeFromBottom(16));
        bands[(size_t) i].setBounds(col.reduced(8, 4));
    }
}
