#include "MainPanelView.h"
#include "../../PluginProcessor.h"
#include "../../Utf8.h"

namespace
{
    juce::StringArray labelsOf (std::initializer_list<juce::String> items)
    {
        juce::StringArray a;
        a.ensureStorageAllocated ((int) items.size());
        for (const auto& s : items)
            a.add (s);
        return a;
    }
}

MainPanelView::MainPanelView(LohUniversalAudioProcessor& p)
    : proc(p),
      upperBars(p.apvts,
                juce::StringArray(ParamIDs::DRAWBAR_UPPER_1, ParamIDs::DRAWBAR_UPPER_2, ParamIDs::DRAWBAR_UPPER_3,
                                  ParamIDs::DRAWBAR_UPPER_4, ParamIDs::DRAWBAR_UPPER_5, ParamIDs::DRAWBAR_UPPER_6,
                                  ParamIDs::DRAWBAR_UPPER_7, ParamIDs::DRAWBAR_UPPER_8),
                labelsOf({ "16'", "8'", "5" + u8OneThird() + "'", "4'",
                           "2" + u8TwoThirds() + "'", "2'",
                           "1" + u8OneFifth() + "'", "1'" }), true),
      lowerBars(p.apvts,
                juce::StringArray(ParamIDs::DRAWBAR_LOWER_1, ParamIDs::DRAWBAR_LOWER_2,
                                  ParamIDs::DRAWBAR_LOWER_3, ParamIDs::DRAWBAR_LOWER_4),
                juce::StringArray("16'", "8'", "4'", "IV"), false),
      mixtureBar(p.apvts, juce::StringArray(ParamIDs::DRAWBAR_MIXTURE_IV), juce::StringArray("IV"), true),
      fluteReed(p.apvts, juce::StringArray(ParamIDs::TONE_FLUTE, ParamIDs::TONE_REED), juce::StringArray("~", "M"), true),
      mains(p.apvts, ParamIDs::MAINS_TOGGLE, "MAINS"),
      vibrato(p.apvts, ParamIDs::VIBRATO_TOGGLE, "VIBRATO"),
      perc(p.apvts, ParamIDs::PERC_TOGGLE, "PERC"),
      ledMode(p.apvts, ParamIDs::DRAWBAR_UI_STYLE, "LED")
{
    addAndMakeVisible(upperBars);
    addAndMakeVisible(lowerBars);
    addAndMakeVisible(mixtureBar);
    addAndMakeVisible(fluteReed);
    addAndMakeVisible(keys);
    addAndMakeVisible(mains);
    addAndMakeVisible(vibrato);
    addAndMakeVisible(perc);
    addAndMakeVisible(ledMode);

    upperBars.setBankTitle("UPPER");
    lowerBars.setBankTitle("LOWER");
    mixtureBar.setBankTitle("VOICING");
    fluteReed.setBankTitle({});
    upperBars.setPaintBackground(false);
    lowerBars.setPaintBackground(false);
    mixtureBar.setPaintBackground(false);
    fluteReed.setPaintBackground(false);

    keys.setManualLabel({});
    keys.setNoteCallback([this](int n, bool on)
    {
        if (on) proc.handleGuiNoteOn(OrganVoiceManager::Upper, n);
        else    proc.handleGuiNoteOff(OrganVoiceManager::Upper, n);
    });
    keys.setHeldQuery([this](int n) { return proc.isNoteHeld(n); });

    percVolBox.addItemList({ "Soft", "Normal" }, 1);
    percVolBox.setSelectedItemIndex(1, juce::dontSendNotification);
    addAndMakeVisible(percVolBox);
    percVolLabel.setText("PERC VOL", juce::dontSendNotification);
    percVolLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(percVolLabel);
    percVolAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc.apvts, ParamIDs::PERC_VOLUME, percVolBox);
}

void MainPanelView::applySkin(const SkinPalette& p, int drawbarStyle)
{
    palette = p;
    upperBars.setPalette(p);  upperBars.setUiStyle(drawbarStyle);
    lowerBars.setPalette(p);  lowerBars.setUiStyle(drawbarStyle);
    mixtureBar.setPalette(p); mixtureBar.setUiStyle(drawbarStyle);
    fluteReed.setPalette(p);  fluteReed.setUiStyle(drawbarStyle);
    keys.setPalette(p);
    mains.setPalette(p);
    vibrato.setPalette(p);
    perc.setPalette(p);
    ledMode.setPalette(p);
    percVolLabel.setColour(juce::Label::textColourId, p.muted);
    repaint();
}

void MainPanelView::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();

    auto* lnf = dynamic_cast<VintageVoxLookAndFeel*>(&getLookAndFeel());

    // Orange rexine lid
    auto lid = r.removeFromTop(248.0f);
    juce::Path lidPath;
    lidPath.addRoundedRectangle(lid.getX() + 6.0f, lid.getY() + 4.0f,
                                lid.getWidth() - 12.0f, lid.getHeight() + 18.0f,
                                18.0f, 18.0f, true, true, false, false);
    g.saveState();
    g.reduceClipRegion(lidPath);
    if (lnf != nullptr)
        lnf->fillRexine(g, lid.expanded(0, 20.0f));
    else
    {
        g.setColour(palette.chassis);
        g.fillRect(lid);
    }
    g.setColour(juce::Colours::white.withAlpha(0.14f));
    g.fillRect(juce::Rectangle<float>(lid.getX() + 8.0f, lid.getY() + 5.0f, lid.getWidth() - 16.0f, 1.4f));

    // Logo
    auto logo = juce::Rectangle<float>(lid.getX() + 22.0f, lid.getY() + 10.0f, 340.0f, 44.0f);
    if (lnf != nullptr)
        lnf->drawVoxScriptLogo(g, logo);
    else
    {
        g.setColour(palette.script);
        g.setFont(juce::Font(juce::FontOptions(32.0f)).withStyle(juce::Font::bold));
        g.drawText(lohBrandFull(), logo, juce::Justification::centredLeft, false);
    }

    g.setColour(juce::Colours::black.withAlpha(0.40f));
    g.setFont(juce::Font(juce::FontOptions(10.0f)).withStyle(juce::Font::bold));
    g.drawText("EXTENDED", juce::Rectangle<float>(lid.getRight() - 120.0f, lid.getY() + 22.0f, 100.0f, 16.0f),
               juce::Justification::centredRight, false);
    g.restoreState();
    // Drawbar well
    auto well = juce::Rectangle<float>(lid.getX() + 16.0f, lid.getY() + 58.0f,
                                       lid.getWidth() - 32.0f, lid.getHeight() - 64.0f);
    g.setColour(juce::Colour(0xff12100e).withAlpha(0.96f));
    g.fillRoundedRectangle(well, 8.0f);
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.drawRoundedRectangle(well, 8.0f, 1.2f);
    g.setColour(juce::Colours::white.withAlpha(0.04f));
    g.drawRoundedRectangle(well.reduced(1.0f), 8.0f, 1.0f);

    // Chassis
    auto chassis = getLocalBounds().toFloat();
    chassis.removeFromTop(236.0f);
    g.setColour(juce::Colour(0xff141210));
    g.fillRoundedRectangle(chassis.reduced(6.0f, 0.0f).withTrimmedBottom(6.0f), 0.0f);
    juce::Path chassisPath;
    chassisPath.addRoundedRectangle(chassis.getX() + 6.0f, chassis.getY(),
                                    chassis.getWidth() - 12.0f, chassis.getHeight() - 6.0f,
                                    18.0f, 18.0f, false, false, true, true);
    g.setColour(juce::Colour(0xff1a1412));
    g.fillPath(chassisPath);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.strokePath(chassisPath, juce::PathStrokeType(1.2f));
}

void MainPanelView::resized()
{
    auto r = getLocalBounds().reduced(10, 8);
    auto lid = r.removeFromTop(236);
    auto logoRow = lid.removeFromTop(52);
    ledMode.setBounds(logoRow.removeFromRight(118).reduced(8, 6));

    auto well = lid.reduced(10, 4);

    const int voicingW = juce::jmax(90, well.getWidth() / 9);
    const int lowerW   = juce::jmax(140, well.getWidth() / 5);
    auto voice = well.removeFromRight(voicingW + 70);
    auto lower = well.removeFromLeft(lowerW);
    well.removeFromLeft(8);
    voice.removeFromLeft(8);

    lowerBars.setBounds(lower);
    upperBars.setBounds(well);
    auto flute = voice.removeFromLeft(voice.getWidth() * 2 / 3);
    fluteReed.setBounds(flute);
    mixtureBar.setBounds(voice);

    r.removeFromTop(8);
    auto keysArea = r;

    auto left = keysArea.removeFromLeft(118);
    auto right = keysArea.removeFromRight(118);

    mains.setBounds(left.removeFromTop(left.getHeight() / 3).reduced(8, 2));
    vibrato.setBounds(left.removeFromTop(left.getHeight() / 2).reduced(8, 2));
    perc.setBounds(left.reduced(8, 2));

    percVolLabel.setBounds(right.removeFromTop(16));
    percVolBox.setBounds(right.removeFromTop(24).reduced(8, 0));

    keys.setBounds(keysArea.reduced(4, 8));
}
