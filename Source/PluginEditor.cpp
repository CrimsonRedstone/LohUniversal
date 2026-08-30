#include "PluginEditor.h"
#include "Parameters/FactoryPresets.h"
#include "Utf8.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <BinaryData.h>

LohUniversalAudioProcessorEditor::LohUniversalAudioProcessorEditor(LohUniversalAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      panelView(p), lidView(p), fxView(p), eqView(p),
      credit("This Freeware is made by Crimson Redstone, If you'd like to support me consider purchasing my music",
             juce::URL("https://crimsonredstone.bandcamp.com/"))
{
    computerHeld.fill(0);
    setLookAndFeel(&lnf);
    setResizable(true, true);
    setResizeLimits(kDesignW / 2, kDesignH / 2, kDesignW * 2, kDesignH * 2);
    setSize(kDesignW, kDesignH);
    if (auto* sizeConstrainer = getConstrainer())
        sizeConstrainer->setFixedAspectRatio(double(kDesignW) / double(kDesignH));

    setWantsKeyboardFocus(true);
    addKeyListener(this);

    addAndMakeVisible(ui);
    ui.addAndMakeVisible(tabs);
    tabs.addTab("Panel", juce::Colours::transparentBlack, &panelView, false);
    tabs.addTab("Open Lid", juce::Colours::transparentBlack, &lidView, false);
    tabs.addTab("FX Rack", juce::Colours::transparentBlack, &fxView, false);
    tabs.addTab("EQ", juce::Colours::transparentBlack, &eqView, false);
    tabs.setOutline(0);

    skinBox.addItemList({ "Default", "Crimson", "Nosmirc", "Citrus Frost", "Junkyard" }, 1);
    engineBox.addItemList({ "Model 301", "Super Combo", "Saw Combo", "Extended" }, 1);
    for (int i = 0; i < kNumFactoryPresets; ++i)
        presetBox.addItem(kFactoryPresets[i].name, i + 1);
    presetBox.setSelectedItemIndex(p.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0)
            processor.setCurrentProgram(idx);
    };

    skinAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        p.apvts, ParamIDs::CHASSIS_SKIN, skinBox);
    engineAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        p.apvts, ParamIDs::ENGINE_MODE, engineBox);

    tagHostParam(skinBox, ParamIDs::CHASSIS_SKIN);
    tagHostParam(engineBox, ParamIDs::ENGINE_MODE);
    tagHostParam(volume, ParamIDs::MASTER_VOLUME);
    volume.setPopupMenuEnabled(false);

    saveBtn.setTooltip("Save a .lohpreset");
    loadBtn.setTooltip("Load a .lohpreset");
    saveBtn.onClick = [this] { savePreset(); };
    loadBtn.onClick = [this] { loadPreset(); };
    ui.addAndMakeVisible(saveBtn);
    ui.addAndMakeVisible(loadBtn);

    ui.addAndMakeVisible(skinBox);
    ui.addAndMakeVisible(engineBox);
    ui.addAndMakeVisible(presetBox);

    credit.setTooltip("https://crimsonredstone.bandcamp.com/");
    credit.setColour(juce::HyperlinkButton::textColourId, juce::Colour(0xffd8c4a8));
    credit.setFont(juce::Font(juce::FontOptions(12.5f)), false, juce::Justification::centredLeft);
    ui.addAndMakeVisible(credit);

    volume.setSliderStyle(juce::Slider::LinearHorizontal);
    volume.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    volAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, ParamIDs::MASTER_VOLUME, volume);
    ui.addAndMakeVisible(volume);

    applySkin();
    startTimerHz(12);

    int iconSize = 0;
    if (const char* data = BinaryData::getNamedResource("LohUniversalIcon_png", iconSize))
        pluginIcon = juce::ImageFileFormat::loadFrom(data, (size_t) iconSize);
    if (pluginIcon.isNull())
    {
        if (const char* data = BinaryData::getNamedResource("icon256_png", iconSize))
            pluginIcon = juce::ImageFileFormat::loadFrom(data, (size_t) iconSize);
    }
    applyWindowIcon();
}

LohUniversalAudioProcessorEditor::~LohUniversalAudioProcessorEditor()
{
    releaseComputerKeys();
    removeKeyListener(this);
    setLookAndFeel(nullptr);
}

void LohUniversalAudioProcessorEditor::applySkin()
{
    const int skin = processor.getChassisSkin();
    const int style = processor.getDrawbarUiStyle();
    lastSkin = skin;
    lastStyle = style;
    lnf.setSkin(skin);
    const auto& pal = lnf.getPalette();
    panelView.applySkin(pal, style);
    lidView.applySkin(pal);
    fxView.applySkin(pal);
    eqView.applySkin(pal);
    credit.setColour(juce::HyperlinkButton::textColourId, pal.muted.brighter(0.25f));
    repaint();
}

void LohUniversalAudioProcessorEditor::applyWindowIcon()
{
    if (pluginIcon.isNull())
        return;
    if (auto* top = getTopLevelComponent())
        if (auto* peer = top->getPeer())
            peer->setIcon(pluginIcon);
}

void LohUniversalAudioProcessorEditor::parentHierarchyChanged()
{
    applyWindowIcon();
}

void LohUniversalAudioProcessorEditor::visibilityChanged()
{
    applyWindowIcon();
}

void LohUniversalAudioProcessorEditor::timerCallback()
{
    applyWindowIcon();
    if (processor.getChassisSkin() != lastSkin || processor.getDrawbarUiStyle() != lastStyle)
        applySkin();
}

void LohUniversalAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0a0908));
    const float s = juce::jmin((float) getWidth() / (float) kDesignW,
                               (float) getHeight() / (float) kDesignH);
    g.addTransform(juce::AffineTransform::scale(s));
    auto header = juce::Rectangle<float>(0.0f, 0.0f, (float) kDesignW, (float) kHeaderH);
    g.setColour(juce::Colour(0xff12100e));
    g.fillRect(header);
    g.setColour(juce::Colour(0xff2a2420));
    g.fillRect(header.removeFromBottom(1.0f));
    lnf.drawVoxScriptLogo(g, juce::Rectangle<float>(14.0f, 6.0f, 250.0f, 34.0f));
}

void LohUniversalAudioProcessorEditor::layoutDesign()
{
    auto r = juce::Rectangle<int>(0, 0, kDesignW, kDesignH);
    auto header = r.removeFromTop(kHeaderH);
    auto top = header.removeFromTop(42).reduced(12, 6);
    auto creditRow = header.reduced(12, 0);
    top.removeFromLeft(250);

    volume.setBounds(top.removeFromRight(110));
    top.removeFromRight(8);
    presetBox.setBounds(top.removeFromRight(168).reduced(2, 1));
    engineBox.setBounds(top.removeFromRight(140).reduced(2, 1));
    skinBox.setBounds(top.removeFromRight(130).reduced(2, 1));
    top.removeFromRight(10);
    loadBtn.setBounds(top.removeFromRight(64).reduced(2, 1));
    top.removeFromRight(4);
    saveBtn.setBounds(top.removeFromRight(64).reduced(2, 1));

    creditRow.removeFromLeft(14);
    credit.setBounds(creditRow.reduced(0, 2));
    tabs.setBounds(r.reduced(8, 4));
}

void LohUniversalAudioProcessorEditor::savePreset()
{
    auto start = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                     .getChildFile("LohUniversal")
                     .getChildFile("Untitled.lohpreset");
    fileChooser = std::make_unique<juce::FileChooser>("Save preset", start, "*.lohpreset");
    const auto flags = juce::FileBrowserComponent::saveMode
                     | juce::FileBrowserComponent::canSelectFiles
                     | juce::FileBrowserComponent::warnAboutOverwriting;
    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file == juce::File{})
            return;
        processor.saveUserPreset(file);
    });
}

void LohUniversalAudioProcessorEditor::loadPreset()
{
    auto start = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                     .getChildFile("LohUniversal");
    fileChooser = std::make_unique<juce::FileChooser>("Load preset", start, "*.lohpreset");
    const auto flags = juce::FileBrowserComponent::openMode
                     | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file == juce::File{})
            return;
        processor.loadUserPreset(file);
    });
}

void LohUniversalAudioProcessorEditor::resized()
{
    const float s = juce::jmin((float) getWidth() / (float) kDesignW,
                               (float) getHeight() / (float) kDesignH);
    ui.setTransform(juce::AffineTransform::scale(s));
    ui.setBounds(0, 0, kDesignW, kDesignH);
    layoutDesign();
}

void LohUniversalAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    grabKeyboardFocus();
}

int LohUniversalAudioProcessorEditor::computerKeyToNote(const juce::KeyPress& key, bool& lower) const noexcept
{
    lower = key.getModifiers().isShiftDown();
    const int raw = static_cast<int>(key.getTextCharacter());
    const int c = (raw >= 'A' && raw <= 'Z') ? (raw - 'A' + 'a') : raw;

    static const int kMap[][2] = {
        { 'a', 48 }, { 'w', 49 }, { 's', 50 }, { 'e', 51 }, { 'd', 52 },
        { 'f', 53 }, { 't', 54 }, { 'g', 55 }, { 'y', 56 }, { 'h', 57 },
        { 'u', 58 }, { 'j', 59 }, { 'k', 60 }, { 'o', 61 }, { 'l', 62 },
        { 'p', 63 }, { ';', 64 }, { '\'', 65 },
        { 'z', 36 }, { 'x', 38 }, { 'c', 40 }, { 'v', 41 }, { 'b', 43 },
        { 'n', 45 }, { 'm', 47 }, { ',', 48 }, { '.', 50 }, { '/', 52 }
    };
    for (const auto& m : kMap)
        if (m[0] == c)
            return m[1];
    return -1;
}

void LohUniversalAudioProcessorEditor::releaseComputerKeys()
{
    for (size_t i = 0; i < computerHeld.size(); ++i)
    {
        if (computerHeld[i] == 0)
            continue;
        const int packed = computerHeld[i] - 1;
        const int manual = packed >> 8;
        const int note = packed & 0x7F;
        processor.handleGuiNoteOff(manual, note);
        computerHeld[i] = 0;
    }
}

bool LohUniversalAudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component*)
{
    if (key == juce::KeyPress::escapeKey)
    {
        releaseComputerKeys();
        return true;
    }

    bool lower = false;
    const int note = computerKeyToNote(key, lower);
    if (note < 0)
        return false;

    const int code = key.getKeyCode();
    if (code < 0 || code >= (int) computerHeld.size())
        return false;
    if (computerHeld[static_cast<size_t>(code)] != 0)
        return true;

    const int manual = lower ? OrganVoiceManager::Lower : OrganVoiceManager::Upper;
    computerHeld[static_cast<size_t>(code)] = ((manual << 8) | (note & 0x7F)) + 1;
    processor.handleGuiNoteOn(manual, note);
    return true;
}

bool LohUniversalAudioProcessorEditor::keyStateChanged(bool isKeyDown, juce::Component*)
{
    if (isKeyDown)
        return false;

    for (size_t i = 0; i < computerHeld.size(); ++i)
    {
        if (computerHeld[i] == 0)
            continue;
        if (juce::KeyPress::isKeyCurrentlyDown((int) i))
            continue;
        const int packed = computerHeld[i] - 1;
        const int manual = packed >> 8;
        const int note = packed & 0x7F;
        processor.handleGuiNoteOff(manual, note);
        computerHeld[i] = 0;
    }
    return false;
}
