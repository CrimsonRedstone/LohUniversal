#include "OpenLidView.h"
#include "../../PluginProcessor.h"
#include "../../Utf8.h"

OpenLidView::OpenLidView(LohUniversalAudioProcessor& p)
    : proc(p)
{
    for (int i = 0; i < 12; ++i)
    {
        oscPots[(size_t) i] = std::make_unique<TrimPotComponent>(
            proc.apvts, kOscDetuneIds[i], kPitchClassNames[i]);
        addAndMakeVisible(*oscPots[(size_t) i]);
    }

    clickPot     = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::MICRO_CLICK,  "CLICK");
    bleedPot     = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::MICRO_BLEED,  "BLEED");
    driftPot     = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::MICRO_DRIFT,  "DRIFT");
    percDecayPot = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::PERC_DECAY,   "PERC DECAY");
    vibSpeedPot  = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::VIBRATO_SPEED,"VIB SPEED");
    vibDepthPot  = std::make_unique<TrimPotComponent>(proc.apvts, ParamIDs::VIBRATO_DEPTH,"VIB DEPTH");

    addAndMakeVisible(*clickPot);
    addAndMakeVisible(*bleedPot);
    addAndMakeVisible(*driftPot);
    addAndMakeVisible(*percDecayPot);
    addAndMakeVisible(*vibSpeedPot);
    addAndMakeVisible(*vibDepthPot);

    titleLabel.setText("TOP-OCTAVE OSCILLATOR BANK  " + u8MidDot() + "  C6" + u8EnDash() + "B6  "
                           + u8MidDot() + "  SERVICE TRIMMERS",
                       juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    hintLabel.setText("Junkyard skin adds " + u8PlusMinus() + "35" + u8Cent()
                          + " static offset, wow/flutter and note-on pitch jumps on top of these trimmers.",
                      juce::dontSendNotification);
    hintLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(hintLabel);

    splitLabel.setText("SPLIT POINT (MIDI)", juce::dontSendNotification);
    splitLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(splitLabel);
    splitSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    splitSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    splitSlider.setPopupMenuEnabled(false);
    tagHostParam(splitSlider, ParamIDs::SPLIT_POINT);
    addAndMakeVisible(splitSlider);
    splitAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc.apvts, ParamIDs::SPLIT_POINT, splitSlider);

    percHarmLabel.setText("PERC HARMONIC", juce::dontSendNotification);
    percHarmLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(percHarmLabel);
    percHarmBox.addItemList({ "4'", "2-2/3'", "2'" }, 1);
    tagHostParam(percHarmBox, ParamIDs::PERC_HARMONIC);
    addAndMakeVisible(percHarmBox);
    percHarmAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc.apvts, ParamIDs::PERC_HARMONIC, percHarmBox);
}

void OpenLidView::applySkin(const SkinPalette& p)
{
    palette = p;
    for (auto& pot : oscPots)
        pot->setPalette(p);
    clickPot->setPalette(p);
    bleedPot->setPalette(p);
    driftPot->setPalette(p);
    percDecayPot->setPalette(p);
    vibSpeedPot->setPalette(p);
    vibDepthPot->setPalette(p);
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc8f0c0));
    hintLabel.setColour(juce::Label::textColourId, juce::Colour(0xff80a878));
    splitLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc8f0c0));
    percHarmLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc8f0c0));
    repaint();
}

void OpenLidView::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced(8.0f);

    // PCB
    juce::ColourGradient pcb(juce::Colour(0xff1b4d2e), r.getX(), r.getY(),
                             juce::Colour(0xff0e2a1a), r.getRight(), r.getBottom(), false);
    g.setGradientFill(pcb);
    g.fillRoundedRectangle(r, 10.0f);

    g.setColour(juce::Colour(0xff0a1f12).withAlpha(0.5f));
    for (int i = 0; i < 24; ++i)
        g.drawHorizontalLine((int) r.getY() + 18 + i * 22, r.getX() + 12.0f, r.getRight() - 12.0f);

    // Copper traces
    g.setColour(juce::Colour(0xffb87333).withAlpha(0.55f));
    auto oscRow = r.removeFromTop(220.0f).reduced(20.0f, 50.0f);
    g.drawRoundedRectangle(oscRow.expanded(12.0f, 20.0f), 6.0f, 1.5f);
    for (int i = 0; i < 12; ++i)
    {
        const float x = oscRow.getX() + oscRow.getWidth() * ((float) i + 0.5f) / 12.0f;
        g.fillRect(x - 1.0f, oscRow.getBottom(), 2.0f, 40.0f);
    }

    g.setColour(juce::Colour(0xffe0c070));
    g.setFont(juce::Font(juce::FontOptions(13.0f)).withStyle(juce::Font::bold));
    g.drawText("DIVIDER TREE  " + u8MidDot() + "  A=440  " + u8MidDot() + "  TOP OCTAVE C6" + u8EnDash() + "B6",
               getLocalBounds().removeFromBottom(28).reduced(24, 4),
               juce::Justification::centredRight, false);

    // Fiducials
    auto fid = [](juce::Graphics& gg, juce::Point<float> c)
    {
        gg.setColour(juce::Colour(0xffd8d0c0));
        gg.drawEllipse(c.x - 5.0f, c.y - 5.0f, 10.0f, 10.0f, 1.2f);
        gg.fillEllipse(c.x - 1.5f, c.y - 1.5f, 3.0f, 3.0f);
    };
    fid(g, { r.getX() + 16.0f, 24.0f });
    fid(g, { (float) getWidth() - 24.0f, 24.0f });
    fid(g, { r.getX() + 16.0f, (float) getHeight() - 24.0f });
    fid(g, { (float) getWidth() - 24.0f, (float) getHeight() - 24.0f });
}

void OpenLidView::resized()
{
    auto r = getLocalBounds().reduced(20);
    titleLabel.setBounds(r.removeFromTop(28));
    hintLabel.setBounds(r.removeFromTop(22));
    r.removeFromTop(8);

    auto oscRow = r.removeFromTop(120);
    const int potW = oscRow.getWidth() / 12;
    for (int i = 0; i < 12; ++i)
        oscPots[(size_t) i]->setBounds(oscRow.removeFromLeft(potW).reduced(6, 4));

    r.removeFromTop(16);
    auto intern = r.removeFromTop(110);
    const int iw = intern.getWidth() / 6;
    auto place = [] (juce::Rectangle<int> cell, juce::Component& c)
    {
        c.setBounds(cell.withSizeKeepingCentre(juce::jmin(88, cell.getWidth()), juce::jmin(100, cell.getHeight())));
    };
    place(intern.removeFromLeft(iw).reduced(4, 0), *clickPot);
    place(intern.removeFromLeft(iw).reduced(4, 0), *bleedPot);
    place(intern.removeFromLeft(iw).reduced(4, 0), *driftPot);
    place(intern.removeFromLeft(iw).reduced(4, 0), *percDecayPot);
    place(intern.removeFromLeft(iw).reduced(4, 0), *vibSpeedPot);
    place(intern.removeFromLeft(iw).reduced(4, 0), *vibDepthPot);

    r.removeFromTop(12);
    auto bottom = r.removeFromTop(48).reduced(20, 0);
    auto split = bottom.removeFromLeft(bottom.getWidth() * 2 / 3).reduced(20, 0);
    splitLabel.setBounds(split.removeFromTop(18));
    splitSlider.setBounds(split);
    percHarmLabel.setBounds(bottom.removeFromTop(18));
    percHarmBox.setBounds(bottom.removeFromTop(24).reduced(8, 0));
}
