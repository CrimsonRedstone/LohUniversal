#include "FXRackView.h"
#include "../../PluginProcessor.h"
#include "../../Utf8.h"

void FXRackView::setupKnob(HostSlider& sl, const char* paramId)
{
    sl.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sl.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sl.setRotaryParameters(juce::degreesToRadians(225.0f), juce::degreesToRadians(495.0f), true);
    sl.setPopupMenuEnabled(false);
    tagHostParam(sl, paramId);
    addAndMakeVisible(sl);
}

FXRackView::FXRackView(LohUniversalAudioProcessor& p)
    : proc(p)
{
    struct Spec
    {
        const char* name;
        const char* mixId;
        const char* onId;
        juce::uint32 colour;
        const char* extraAId;
        const char* extraAName;
        const char* extraBId;
        const char* extraBName;
    };

    const Spec specs[6] = {
        { "OVERDRIVE", ParamIDs::OVERDRIVE, ParamIDs::FX_OVERDRIVE_ON, 0xffc0392b, nullptr, nullptr, nullptr, nullptr },
        { "WAH",       ParamIDs::WAH,       ParamIDs::FX_WAH_ON,       0xfff1c40f, nullptr, nullptr, nullptr, nullptr },
        { "CHORUS",    ParamIDs::CHORUS,    ParamIDs::FX_CHORUS_ON,    0xff27ae60, nullptr, nullptr, nullptr, nullptr },
        { "PHASER",    ParamIDs::PHASER,    ParamIDs::FX_PHASER_ON,    0xff8e44ad, nullptr, nullptr, nullptr, nullptr },
        { "DELAY",     ParamIDs::DELAY,     ParamIDs::FX_DELAY_ON,     0xff2980b9, ParamIDs::DELAY_TIME, "TIME", ParamIDs::DELAY_FEEDBACK, "FDBK" },
        { "REVERB",    ParamIDs::REVERB,    ParamIDs::FX_REVERB_ON,    0xff1abc9c, ParamIDs::REVERB_SIZE, "SIZE", nullptr, nullptr }
    };

    for (int i = 0; i < 6; ++i)
    {
        auto s = std::make_unique<Stomp>();
        s->name = specs[i].name;
        s->colour = juce::Colour(specs[i].colour);
        setupKnob(s->mix, specs[i].mixId);
        s->mixLab.setText("MIX", juce::dontSendNotification);
        s->mixLab.setJustificationType(juce::Justification::centred);
        s->mixLab.setInterceptsMouseClicks(false, false);
        addAndMakeVisible(s->mixLab);
        s->mixAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            proc.apvts, specs[i].mixId, s->mix);

        if (specs[i].extraAId != nullptr)
        {
            setupKnob(s->extraA, specs[i].extraAId);
            s->extraALab.setText(specs[i].extraAName, juce::dontSendNotification);
            s->extraALab.setJustificationType(juce::Justification::centred);
            s->extraALab.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(s->extraALab);
            s->extraAAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                proc.apvts, specs[i].extraAId, s->extraA);
            s->extras = 1;
        }
        if (specs[i].extraBId != nullptr)
        {
            setupKnob(s->extraB, specs[i].extraBId);
            s->extraBLab.setText(specs[i].extraBName, juce::dontSendNotification);
            s->extraBLab.setJustificationType(juce::Justification::centred);
            s->extraBLab.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(s->extraBLab);
            s->extraBAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                proc.apvts, specs[i].extraBId, s->extraB);
            s->extras = 2;
        }

        s->on.setButtonText({});
        s->on.setClickingTogglesState(true);
        s->on.getProperties().set("stomp", true);
        tagHostParam(s->on, specs[i].onId);
        addAndMakeVisible(s->on);
        s->onAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            proc.apvts, specs[i].onId, s->on);
        s->lastOn = s->on.getToggleState();
        stomps[(size_t) i] = std::move(s);
    }

    rackTitle.setText("FX RACK  " + u8MidDot() + "  STOMP  " + u8Arrow() + "  AMP  " + u8Arrow() + "  ROTARY",
                      juce::dontSendNotification);
    rackTitle.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(rackTitle);

    ampTitle.setText("AMP", juce::dontSendNotification);
    leslieTitle.setText("ROTARY", juce::dontSendNotification);
    addAndMakeVisible(ampTitle);
    addAndMakeVisible(leslieTitle);

    ampBox.addItemList({ "Direct", "Combo Cabinet", "Rotary Speaker" }, 1);
    rotaryBox.addItemList({ "Stop", "Slow", "Fast" }, 1);
    tagHostParam(ampBox, ParamIDs::AMP_TYPE);
    tagHostParam(rotaryBox, ParamIDs::ROTARY_SPEED);
    addAndMakeVisible(ampBox);
    addAndMakeVisible(rotaryBox);
    ampAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc.apvts, ParamIDs::AMP_TYPE, ampBox);
    rotaryAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc.apvts, ParamIDs::ROTARY_SPEED, rotaryBox);

    auto lab = [this](juce::Label& l, const juce::String& t)
    {
        l.setText(t, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setInterceptsMouseClicks(false, false);
        addAndMakeVisible(l);
    };
    lab(driveLab, "DRIVE");
    lab(bassLab, "BASS");
    lab(trebleLab, "TREBLE");
    lab(cutLab, "CUT");
    lab(balLab, "HORN");

    setupKnob(ampDrive, ParamIDs::AMP_DRIVE);
    setupKnob(ampBass, ParamIDs::AMP_BASS);
    setupKnob(ampTreble, ParamIDs::AMP_TREBLE);
    setupKnob(ampCut, ParamIDs::AMP_CUT);
    setupKnob(rotBal, ParamIDs::ROTARY_BAL);
    driveAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, ParamIDs::AMP_DRIVE, ampDrive);
    bassAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, ParamIDs::AMP_BASS, ampBass);
    trebleAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, ParamIDs::AMP_TREBLE, ampTreble);
    cutAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, ParamIDs::AMP_CUT, ampCut);
    balAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, ParamIDs::ROTARY_BAL, rotBal);

    startTimerHz(12);
}

void FXRackView::timerCallback()
{
    bool dirty = false;
    for (auto& s : stomps)
    {
        const bool on = s->on.getToggleState();
        if (on != s->lastOn)
        {
            s->lastOn = on;
            dirty = true;
        }
    }
    if (dirty)
        repaint();
}

void FXRackView::applySkin(const SkinPalette& p)
{
    palette = p;
    rackTitle.setColour(juce::Label::textColourId, p.text);
    ampTitle.setColour(juce::Label::textColourId, p.text);
    leslieTitle.setColour(juce::Label::textColourId, p.text);
    auto mute = [&](juce::Label& l) { l.setColour(juce::Label::textColourId, p.muted); };
    mute(driveLab); mute(bassLab); mute(trebleLab); mute(cutLab); mute(balLab);
    for (auto& s : stomps)
    {
        s->mixLab.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.70f));
        s->extraALab.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.70f));
        s->extraBLab.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.70f));
    }
    repaint();
}

void FXRackView::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour(palette.chassisDark.interpolatedWith(juce::Colours::black, 0.28f));
    g.fillRect(r);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(0, 34, getWidth(), 12);
    g.fillRect(0, getHeight() - 16, getWidth(), 12);
    g.setColour(juce::Colour(0xff8a8680));
    for (int x = 24; x < getWidth(); x += 48)
    {
        g.fillEllipse((float) x, 36.0f, 8.0f, 8.0f);
        g.fillEllipse((float) x, (float) getHeight() - 14.0f, 8.0f, 8.0f);
    }

    for (int i = 0; i < 6; ++i)
    {
        auto b = stomps[(size_t) i]->bounds.toFloat();
        if (b.isEmpty())
            continue;

        g.setColour(juce::Colours::black.withAlpha(0.40f));
        g.fillRoundedRectangle(b.translated(3.0f, 5.0f), 12.0f);

        juce::ColourGradient body(stomps[(size_t) i]->colour.brighter(0.12f), b.getX(), b.getY(),
                                  stomps[(size_t) i]->colour.darker(0.40f), b.getX(), b.getBottom(), false);
        g.setGradientFill(body);
        g.fillRoundedRectangle(b, 12.0f);
        g.setColour(juce::Colours::white.withAlpha(0.16f));
        g.drawRoundedRectangle(b.reduced(2.0f), 10.0f, 1.1f);

        const bool on = stomps[(size_t) i]->on.getToggleState();
        auto led = juce::Rectangle<float>(b.getX() + 10.0f, b.getY() + 10.0f, 12.0f, 12.0f);
        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.fillEllipse(led.expanded(1.5f));
        g.setColour(on ? juce::Colour(0xffff2a2a) : juce::Colour(0xff2a0c0c));
        g.fillEllipse(led);
        if (on)
        {
            g.setColour(juce::Colours::white.withAlpha(0.65f));
            g.fillEllipse(led.reduced(3.2f).translated(-0.6f, -0.8f));
        }

        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.setFont(juce::Font(juce::FontOptions(11.5f)).withStyle(juce::Font::bold));
        g.drawText(stomps[(size_t) i]->name,
                   juce::Rectangle<float>(b.getX() + 26.0f, b.getY() + 6.0f, b.getWidth() - 34.0f, 18.0f),
                   juce::Justification::centredLeft, false);
    }

    auto paintRack = [&](juce::Rectangle<int> panel, const juce::String& badge)
    {
        auto f = panel.toFloat();
        g.setColour(juce::Colour(0xff141210));
        g.fillRoundedRectangle(f, 8.0f);
        g.setColour(palette.accent.withAlpha(0.40f));
        g.drawRoundedRectangle(f, 8.0f, 1.1f);
        g.setColour(palette.accent.withAlpha(0.85f));
        g.setFont(juce::Font(juce::FontOptions(11.0f)).withStyle(juce::Font::bold));
        g.drawText(badge, f.removeFromLeft(70.0f).reduced(8.0f, 4.0f), juce::Justification::centredLeft, false);
        juce::ignoreUnused(badge);
    };
    paintRack(ampPanel, {});
    paintRack(lesliePanel, {});
}

void FXRackView::resized()
{
    auto r = getLocalBounds().reduced(14, 12);
    rackTitle.setBounds(r.removeFromTop(24));
    r.removeFromTop(14);
    r.removeFromBottom(8);

    auto stompRow = r.removeFromTop(juce::jmax(210, r.getHeight() * 48 / 100));
    const int w = stompRow.getWidth() / 6;
    for (int i = 0; i < 6; ++i)
    {
        auto cell = stompRow.removeFromLeft(w).reduced(8, 4);
        stomps[(size_t) i]->bounds = cell;
        cell.removeFromTop(28);
        auto foot = cell.removeFromBottom(44);
        stomps[(size_t) i]->on.setBounds(foot.withSizeKeepingCentre(36, 36));

        const int extras = stomps[(size_t) i]->extras;
        if (extras == 0)
        {
            auto kn = cell.withSizeKeepingCentre(74, 74);
            stomps[(size_t) i]->mix.setBounds(kn);
            stomps[(size_t) i]->mixLab.setBounds(kn.getX(), kn.getBottom() - 2, kn.getWidth(), 16);
            stomps[(size_t) i]->extraA.setVisible(false);
            stomps[(size_t) i]->extraB.setVisible(false);
        }
        else if (extras == 1)
        {
            auto left = cell.removeFromLeft(cell.getWidth() / 2);
            auto knA = left.withSizeKeepingCentre(62, 62);
            auto knM = cell.withSizeKeepingCentre(62, 62);
            stomps[(size_t) i]->extraA.setBounds(knA);
            stomps[(size_t) i]->extraALab.setBounds(knA.getX(), knA.getBottom() - 2, knA.getWidth(), 16);
            stomps[(size_t) i]->mix.setBounds(knM);
            stomps[(size_t) i]->mixLab.setBounds(knM.getX(), knM.getBottom() - 2, knM.getWidth(), 16);
            stomps[(size_t) i]->extraA.setVisible(true);
            stomps[(size_t) i]->extraB.setVisible(false);
        }
        else
        {
            const int cw = cell.getWidth() / 3;
            auto c0 = cell.removeFromLeft(cw);
            auto c1 = cell.removeFromLeft(cw);
            auto kn0 = c0.withSizeKeepingCentre(56, 56);
            auto kn1 = c1.withSizeKeepingCentre(56, 56);
            auto kn2 = cell.withSizeKeepingCentre(56, 56);
            stomps[(size_t) i]->extraA.setBounds(kn0);
            stomps[(size_t) i]->extraALab.setBounds(kn0.getX(), kn0.getBottom() - 2, kn0.getWidth(), 16);
            stomps[(size_t) i]->extraB.setBounds(kn1);
            stomps[(size_t) i]->extraBLab.setBounds(kn1.getX(), kn1.getBottom() - 2, kn1.getWidth(), 16);
            stomps[(size_t) i]->mix.setBounds(kn2);
            stomps[(size_t) i]->mixLab.setBounds(kn2.getX(), kn2.getBottom() - 2, kn2.getWidth(), 16);
            stomps[(size_t) i]->extraA.setVisible(true);
            stomps[(size_t) i]->extraB.setVisible(true);
        }
    }

    r.removeFromTop(10);
    const int remain = r.getHeight();
    ampPanel = r.removeFromTop(juce::jmax(160, remain * 58 / 100)).reduced(4, 2);
    r.removeFromTop(6);
    lesliePanel = r.reduced(4, 2);

    auto amp = ampPanel.reduced(8, 6);
    auto ampHead = amp.removeFromTop(28);
    ampTitle.setBounds(ampHead.removeFromLeft(64));
    ampBox.setBounds(ampHead.removeFromLeft(170).reduced(4, 1));

    auto place = [](juce::Rectangle<int> col, juce::Slider& sl, juce::Label& lab)
    {
        lab.setBounds(col.removeFromBottom(16));
        sl.setBounds(col.withSizeKeepingCentre(72, 72));
    };
    const int aw = amp.getWidth() / 4;
    place(amp.removeFromLeft(aw).reduced(4, 0), ampDrive, driveLab);
    place(amp.removeFromLeft(aw).reduced(4, 0), ampBass, bassLab);
    place(amp.removeFromLeft(aw).reduced(4, 0), ampTreble, trebleLab);
    place(amp.reduced(4, 0), ampCut, cutLab);

    auto les = lesliePanel.reduced(8, 6);
    auto lesHead = les.removeFromTop(28);
    leslieTitle.setBounds(lesHead.removeFromLeft(72));
    rotaryBox.setBounds(lesHead.removeFromLeft(140).reduced(4, 1));
    place(les.removeFromLeft(juce::jmin(120, les.getWidth() / 3)).reduced(4, 0), rotBal, balLab);
}
