#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../../Parameters/ParameterLayout.h"

struct SkinPalette
{
    juce::Colour chassis      { 0xffd35400 };
    juce::Colour chassisDark  { 0xff8a2c00 };
    juce::Colour chassisLight { 0xffff7a28 };
    juce::Colour faceplate    { 0xff111111 };
    juce::Colour faceplateHi  { 0xff2a2a2a };
    juce::Colour accent       { 0xffd35400 };
    juce::Colour script       { 0xfff4e6c3 };
    juce::Colour text         { 0xfff0ead8 };
    juce::Colour muted        { 0xff9a8f7a };
    juce::Colour naturalKey   { 0xff161616 };
    juce::Colour sharpKey     { 0xfff4efe4 };
    juce::Colour keyBed       { 0xff0a0a0a };
    juce::Colour keyLine      { 0xff000000 };
    juce::Colour slot         { 0xff1a1a1a };
    juce::Colour metal        { 0xffc5c0b4 };
    juce::Colour lampOn       { 0xffffb347 };
    juce::Colour lampOff      { 0xff3a2a18 };
    juce::Colour ledOn        { 0xffffcc33 };
    juce::Colour ledOff       { 0xff2a2208 };
    bool reverseKeys = true;
    bool relic = false;
    juce::String name { "Default" };

    static SkinPalette forSkin(int chassisSkin) noexcept;
};

class VintageVoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VintageVoxLookAndFeel();

    void setSkin(int chassisSkin);
    const SkinPalette& getPalette() const noexcept { return palette; }

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle, juce::Slider&) override;

    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;

    void positionComboBoxText(juce::ComboBox&, juce::Label&) override;
    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox&, juce::Label&) override;

    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getPopupMenuFont() override;
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;

    void drawPopupMenuBackground(juce::Graphics&, int width, int height) override;
    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    void drawTabButton(juce::TabBarButton&, juce::Graphics&, bool isMouseOver, bool isMouseDown) override;
    int getTabButtonBestWidth(juce::TabBarButton&, int tabDepth) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    void fillChassis(juce::Graphics& g, juce::Rectangle<int> bounds) const;
    void fillRexine(juce::Graphics& g, juce::Rectangle<float> bounds) const;
    void fillFaceplate(juce::Graphics& g, juce::Rectangle<int> bounds, float corner = 6.0f) const;
    void drawVoxScriptLogo(juce::Graphics& g, juce::Rectangle<float> bounds) const;
    void drawScrew(juce::Graphics& g, juce::Point<float> centre, float radius) const;
    void drawMetalSlot(juce::Graphics& g, juce::Rectangle<float> slot) const;

    juce::Font scriptFont(float height) const;
    juce::Font displayFont(float height) const;
    juce::Font panelFont(float height, bool bold = false) const;

private:
    SkinPalette palette;
    juce::Typeface::Ptr regularFace, boldFace, displayFace, scriptFace;
};
