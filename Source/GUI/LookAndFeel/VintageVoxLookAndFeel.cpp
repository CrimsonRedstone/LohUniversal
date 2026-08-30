#include "VintageVoxLookAndFeel.h"
#include "../../Utf8.h"
#include <BinaryData.h>
#include <cmath>
#include <initializer_list>

SkinPalette SkinPalette::forSkin(int chassisSkin) noexcept
{
    SkinPalette p;
    switch (chassisSkin)
    {
        case ChassisSkin::Crimson:
            p.chassis      = juce::Colour(0xff640000);
            p.chassisDark  = juce::Colour(0xff2a0000);
            p.chassisLight = juce::Colour(0xff8e1010);
            p.faceplate    = juce::Colour(0xff0c0c0c);
            p.faceplateHi  = juce::Colour(0xff242424);
            p.accent       = juce::Colour(0xffc41e3a);
            p.script       = juce::Colour(0xffe8d5c4);
            p.text         = juce::Colour(0xffeadfd4);
            p.muted        = juce::Colour(0xff8a7a70);
            p.naturalKey   = juce::Colour(0xff140808);
            p.sharpKey     = juce::Colour(0xffefe6dc);
            p.keyBed       = juce::Colour(0xff080404);
            p.lampOn       = juce::Colour(0xffff4d4d);
            p.ledOn        = juce::Colour(0xffff5555);
            p.reverseKeys  = true;
            p.name = "Crimson";
            break;

        case ChassisSkin::Nosmirc:
            p.chassis      = juce::Colour(0xfff4f7fa);
            p.chassisDark  = juce::Colour(0xffc5ced6);
            p.chassisLight = juce::Colour(0xffffffff);
            p.faceplate    = juce::Colour(0xff1a242c);
            p.faceplateHi  = juce::Colour(0xff2c3a46);
            p.accent       = juce::Colour(0xff00ffff);
            p.script       = juce::Colour(0xff00e5e5);
            p.text         = juce::Colour(0xffe8ffff);
            p.muted        = juce::Colour(0xff7aa0a8);
            p.naturalKey   = juce::Colour(0xfff7fbfd);
            p.sharpKey     = juce::Colour(0xff00c8d4);
            p.keyBed       = juce::Colour(0xff0e161c);
            p.lampOn       = juce::Colour(0xff00ffff);
            p.ledOn        = juce::Colour(0xff33ffff);
            p.ledOff       = juce::Colour(0xff0a3030);
            p.reverseKeys  = false;
            p.name = "Nosmirc";
            break;

        case ChassisSkin::CitrusFrost:
            p.chassis      = juce::Colour(0xffff8c1a);
            p.chassisDark  = juce::Colour(0xffd35400);
            p.chassisLight = juce::Colour(0xfffff7e8);
            p.faceplate    = juce::Colour(0xfff4f0e6);
            p.faceplateHi  = juce::Colour(0xffffffff);
            p.accent       = juce::Colour(0xffff6a00);
            p.script       = juce::Colour(0xffc44a00);
            p.text         = juce::Colour(0xff3a2a18);
            p.muted        = juce::Colour(0xff8a7048);
            p.naturalKey   = juce::Colour(0xfffff8ee);
            p.sharpKey     = juce::Colour(0xff2a2218);
            p.keyBed       = juce::Colour(0xffd0c4b0);
            p.lampOn       = juce::Colour(0xffffaa33);
            p.ledOn        = juce::Colour(0xffff9900);
            p.reverseKeys  = false;
            p.name = "Citrus Frost";
            break;

        case ChassisSkin::Junkyard:
            p.chassis      = juce::Colour(0xff6b4a22);
            p.chassisDark  = juce::Colour(0xff2e2214);
            p.chassisLight = juce::Colour(0xff8a6a38);
            p.faceplate    = juce::Colour(0xff1c1810);
            p.faceplateHi  = juce::Colour(0xff3a3224);
            p.accent       = juce::Colour(0xffc47a22);
            p.script       = juce::Colour(0xffd8c49a);
            p.text         = juce::Colour(0xffc8b896);
            p.muted        = juce::Colour(0xff7a6a48);
            p.naturalKey   = juce::Colour(0xff221c12);
            p.sharpKey     = juce::Colour(0xffd8cbb0);
            p.keyBed       = juce::Colour(0xff0e0c08);
            p.lampOn       = juce::Colour(0xffe0a040);
            p.ledOn        = juce::Colour(0xffc8a030);
            p.reverseKeys  = true;
            p.relic        = true;
            p.name = "Junkyard";
            break;

        default:
            p.chassis      = juce::Colour(0xffd35400);
            p.chassisDark  = juce::Colour(0xff8a2c00);
            p.chassisLight = juce::Colour(0xffff7a28);
            p.faceplate    = juce::Colour(0xff111111);
            p.faceplateHi  = juce::Colour(0xff2a2a2a);
            p.accent       = juce::Colour(0xffd35400);
            p.script       = juce::Colour(0xfff4e6c3);
            p.text         = juce::Colour(0xfff0ead8);
            p.reverseKeys  = true;
            p.name = "Default";
            break;
    }
    return p;
}

VintageVoxLookAndFeel::VintageVoxLookAndFeel()
{
    auto loadFace = [] (std::initializer_list<const char*> keys) -> juce::Typeface::Ptr
    {
        auto fromIdent = [] (const char* ident) -> juce::Typeface::Ptr
        {
            int sz = 0;
            if (const char* data = BinaryData::getNamedResource (ident, sz); data != nullptr && sz > 0)
                return juce::Typeface::createSystemTypefaceFor (data, (size_t) sz);
            return {};
        };

        for (auto* k : keys)
            if (auto face = fromIdent (k); face != nullptr)
                return face;

        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            const juce::String name (BinaryData::namedResourceList[i]);
            for (auto* k : keys)
                if (name.containsIgnoreCase (k))
                    if (auto face = fromIdent (BinaryData::namedResourceList[i]); face != nullptr)
                        return face;
        }
        return {};
    };

    // JUCE strips hyphens from filenames: InterRegular.ttf -> InterRegular_ttf
    regularFace = loadFace ({ "InterRegular_ttf", "Inter_Regular_ttf", "InterRegular" });
    boldFace    = loadFace ({ "InterBold_ttf", "Inter_Bold_ttf", "InterBold" });
    displayFace = loadFace ({ "InterDisplayBold_ttf", "InterDisplay_Bold_ttf", "InterDisplay" });
    scriptFace  = loadFace ({ "GreatVibesRegular_ttf", "GreatVibes_Regular_ttf", "GreatVibes" });

    if (regularFace != nullptr)
        setDefaultSansSerifTypeface(regularFace);

    setSkin(ChassisSkin::Default);
}

void VintageVoxLookAndFeel::setSkin(int chassisSkin)
{
    palette = SkinPalette::forSkin(chassisSkin);

    setColour(juce::ResizableWindow::backgroundColourId, palette.chassis);
    setColour(juce::Label::textColourId, palette.text);
    setColour(juce::ComboBox::backgroundColourId, palette.faceplate);
    setColour(juce::ComboBox::textColourId, palette.text);
    setColour(juce::ComboBox::outlineColourId, palette.accent.withAlpha(0.55f));
    setColour(juce::ComboBox::arrowColourId, palette.accent);
    setColour(juce::PopupMenu::backgroundColourId, palette.faceplate);
    setColour(juce::PopupMenu::textColourId, palette.text);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, palette.accent);
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
    setColour(juce::TextButton::buttonColourId, palette.faceplate);
    setColour(juce::TextButton::textColourOffId, palette.text);
    setColour(juce::TextButton::textColourOnId, palette.accent);
    setColour(juce::Slider::rotarySliderFillColourId, palette.accent);
    setColour(juce::Slider::rotarySliderOutlineColourId, palette.faceplateHi);
    setColour(juce::Slider::thumbColourId, palette.metal);
    setColour(juce::TabbedButtonBar::tabOutlineColourId, palette.chassisDark);
    setColour(juce::TabbedButtonBar::frontOutlineColourId, palette.accent);
    setColour(juce::TooltipWindow::backgroundColourId, palette.faceplate);
    setColour(juce::TooltipWindow::textColourId, palette.text);
    setColour(juce::TooltipWindow::outlineColourId, palette.accent);
}

juce::Font VintageVoxLookAndFeel::scriptFont(float height) const
{
    if (scriptFace != nullptr)
        return juce::Font(juce::FontOptions(scriptFace).withHeight(height));
    return juce::Font(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), height, juce::Font::italic));
}

juce::Font VintageVoxLookAndFeel::displayFont(float height) const
{
    if (displayFace != nullptr)
        return juce::Font(juce::FontOptions(displayFace).withHeight(height));
    return panelFont(height, true);
}

juce::Font VintageVoxLookAndFeel::panelFont(float height, bool bold) const
{
    auto face = bold ? (boldFace != nullptr ? boldFace : regularFace)
                     : regularFace;
    if (face != nullptr)
        return juce::Font(juce::FontOptions(face).withHeight(height));
    return juce::Font(juce::FontOptions(juce::Font::getDefaultSansSerifFontName(), height,
                                       bold ? juce::Font::bold : juce::Font::plain));
}

juce::Typeface::Ptr VintageVoxLookAndFeel::getTypefaceForFont(const juce::Font& font)
{
    const auto name = font.getTypefaceName();
    if (name == "Great Vibes" || name == "LohScript")
        return scriptFace != nullptr ? scriptFace : LookAndFeel_V4::getTypefaceForFont(font);
    if (name == "LohDisplay" || name == "Inter Display")
        return displayFace != nullptr ? displayFace : LookAndFeel_V4::getTypefaceForFont(font);

    if (font.isBold())
        return boldFace != nullptr ? boldFace : LookAndFeel_V4::getTypefaceForFont(font);
    return regularFace != nullptr ? regularFace : LookAndFeel_V4::getTypefaceForFont(font);
}

juce::Font VintageVoxLookAndFeel::getComboBoxFont(juce::ComboBox&) { return panelFont(13.0f); }
juce::Font VintageVoxLookAndFeel::getTextButtonFont(juce::TextButton&, int h) { return panelFont(juce::jmax(11.0f, (float) h * 0.38f), true); }
juce::Font VintageVoxLookAndFeel::getLabelFont(juce::Label&) { return panelFont(12.0f); }
juce::Font VintageVoxLookAndFeel::getPopupMenuFont() { return panelFont(13.0f); }

juce::PopupMenu::Options VintageVoxLookAndFeel::getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label& label)
{
    // Drop BELOW the combo. Do not pin the selected item to the box (that
    // is what made Engine "Extended" and long preset lists open upward).
    const auto screen = box.getScreenBounds();
    const juce::Rectangle<int> below(screen.getX(), screen.getBottom(),
                                     juce::jmax(screen.getWidth(), 160), 1);
    return juce::PopupMenu::Options()
        .withTargetScreenArea(below)
        .withMinimumWidth(juce::jmax(box.getWidth(), 160))
        .withMaximumNumColumns(1)
        .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards)
        .withStandardItemHeight(juce::jmax(22, label.getHeight()));
}

void VintageVoxLookAndFeel::fillRexine(juce::Graphics& g, juce::Rectangle<float> r) const
{
    juce::ColourGradient grad(palette.chassisLight, r.getX(), r.getY(),
                              palette.chassisDark, r.getX(), r.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRect(r);

    g.setColour(juce::Colours::black.withAlpha(0.055f));
    for (float x = r.getX(); x < r.getRight(); x += 3.0f)
        g.drawVerticalLine((int) x, r.getY(), r.getBottom());
    g.setColour(juce::Colours::white.withAlpha(0.035f));
    for (float y = r.getY(); y < r.getBottom(); y += 4.0f)
        g.drawHorizontalLine((int) y, r.getX(), r.getRight());

    g.setColour(juce::Colours::white.withAlpha(0.10f));
    g.fillRect(juce::Rectangle<float>(r.getX(), r.getY(), r.getWidth(), 1.5f));
    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.fillRect(juce::Rectangle<float>(r.getX(), r.getBottom() - 1.5f, r.getWidth(), 1.5f));
}

void VintageVoxLookAndFeel::fillChassis(juce::Graphics& g, juce::Rectangle<int> bounds) const
{
    auto r = bounds.toFloat();
    fillRexine(g, r);

    if (palette.relic)
    {
        juce::Random rng(0xDEAD45);
        g.setColour(juce::Colours::black.withAlpha(0.08f));
        for (int i = 0; i < 80; ++i)
        {
            const float x = r.getX() + rng.nextFloat() * r.getWidth();
            const float y = r.getY() + rng.nextFloat() * r.getHeight();
            g.fillEllipse(x, y, 8.0f + rng.nextFloat() * 28.0f, 4.0f + rng.nextFloat() * 10.0f);
        }
        g.setColour(juce::Colours::white.withAlpha(0.04f));
        for (int i = 0; i < 30; ++i)
            g.drawLine(r.getX() + rng.nextFloat() * r.getWidth(),
                       r.getY() + rng.nextFloat() * r.getHeight(),
                       r.getX() + rng.nextFloat() * r.getWidth(),
                       r.getY() + rng.nextFloat() * r.getHeight(), 0.6f);
    }
}

void VintageVoxLookAndFeel::fillFaceplate(juce::Graphics& g, juce::Rectangle<int> bounds, float corner) const
{
    auto r = bounds.toFloat();
    g.setColour(palette.faceplate);
    g.fillRoundedRectangle(r, corner);
    g.setColour(palette.faceplateHi.withAlpha(0.45f));
    g.drawRoundedRectangle(r.reduced(0.5f), corner, 1.0f);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(r, corner, 1.2f);
}

void VintageVoxLookAndFeel::drawScrew(juce::Graphics& g, juce::Point<float> c, float radius) const
{
    g.setColour(juce::Colour(0xff8a8680));
    g.fillEllipse(c.x - radius, c.y - radius, radius * 2.0f, radius * 2.0f);
    g.setColour(juce::Colour(0xffd8d4cc));
    g.fillEllipse(c.x - radius * 0.7f, c.y - radius * 0.7f, radius * 1.4f, radius * 1.1f);
    g.setColour(juce::Colour(0xff3a3834));
    g.drawLine(c.x - radius * 0.55f, c.y - radius * 0.15f,
               c.x + radius * 0.55f, c.y + radius * 0.15f, 1.4f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.drawEllipse(c.x - radius, c.y - radius, radius * 2.0f, radius * 2.0f, 0.8f);
}

void VintageVoxLookAndFeel::drawMetalSlot(juce::Graphics& g, juce::Rectangle<float> slot) const
{
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(slot, 3.0f);
    juce::ColourGradient steel(juce::Colour(0xff3a3a3a), slot.getX(), slot.getY(),
                               juce::Colour(0xff101010), slot.getRight(), slot.getY(), false);
    g.setGradientFill(steel);
    g.fillRoundedRectangle(slot.reduced(1.5f), 2.0f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawVerticalLine((int) slot.getCentreX(), slot.getY() + 2.0f, slot.getBottom() - 2.0f);
}

void VintageVoxLookAndFeel::drawVoxScriptLogo(juce::Graphics& g, juce::Rectangle<float> bounds) const
{
    const auto brand = lohBrand();
    auto dFont = displayFont(juce::jlimit(22.0f, 72.0f, bounds.getHeight() * 0.82f));
    g.setFont(dFont);
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.drawText(brand, bounds.translated(1.5f, 1.5f), juce::Justification::centredLeft, false);
    g.setColour(palette.script);
    g.drawText(brand, bounds, juce::Justification::centredLeft, false);

    auto rest = bounds;
    rest.removeFromLeft(juce::GlyphArrangement::getStringWidth(dFont, brand) + 8.0f);
    rest.translate(0.0f, bounds.getHeight() * 0.06f);
    auto sFont = scriptFont(juce::jlimit(16.0f, 48.0f, bounds.getHeight() * 0.62f));
    g.setFont(sFont);
    g.setColour(palette.accent);
    g.drawText("Universal", rest, juce::Justification::centredLeft, false);
}

void VintageVoxLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                             float pos, float start, float end, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat();
    const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.42f;
    const auto c = bounds.getCentre();
    const bool trim = slider.getProperties().contains("trimPot");

    if (trim)
    {
        g.setColour(juce::Colour(0xff2a2a28));
        g.fillEllipse(c.x - radius, c.y - radius, radius * 2, radius * 2);
        g.setColour(juce::Colour(0xff8a8680));
        g.fillEllipse(c.x - radius * 0.78f, c.y - radius * 0.78f, radius * 1.56f, radius * 1.56f);
        g.setColour(juce::Colour(0xffc4c0b6));
        g.fillEllipse(c.x - radius * 0.55f, c.y - radius * 0.55f, radius * 1.1f, radius * 1.1f);

        const float ang = start + pos * (end - start);
        juce::Path slot;
        slot.addRectangle(-0.7f, -radius * 0.42f, 1.4f, radius * 0.84f);
        g.setColour(juce::Colour(0xff1a1a18));
        g.fillPath(slot, juce::AffineTransform::rotation(ang).translated(c.x, c.y));
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(c.x - radius, c.y - radius, radius * 2, radius * 2, 1.0f);
        return;
    }

    // Skirted vintage knob
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillEllipse(c.x - radius, c.y - radius + 3.0f, radius * 2, radius * 2);

    juce::ColourGradient skirt(palette.faceplateHi.brighter(0.15f), c.x, c.y - radius,
                               palette.faceplate.darker(0.4f), c.x, c.y + radius, false);
    g.setGradientFill(skirt);
    g.fillEllipse(c.x - radius, c.y - radius, radius * 2, radius * 2);

    const float inner = radius * 0.62f;
    juce::Colour top = palette.metal;
    juce::ColourGradient cap(top.brighter(0.35f), c.x - inner * 0.3f, c.y - inner * 0.5f,
                             top.darker(0.35f), c.x, c.y + inner, true);
    g.setGradientFill(cap);
    g.fillEllipse(c.x - inner, c.y - inner, inner * 2, inner * 2);

    const float ang = start + pos * (end - start);
    juce::Path pointer;
    pointer.addRoundedRectangle(-1.3f, -inner * 0.92f, 2.6f, inner * 0.72f, 1.0f);
    g.setColour(palette.accent);
    g.fillPath(pointer, juce::AffineTransform::rotation(ang).translated(c.x, c.y));

    g.setColour(juce::Colours::white.withAlpha(0.18f));
    g.drawEllipse(c.x - inner * 0.7f, c.y - inner * 0.75f, inner * 1.1f, inner * 0.45f, 1.2f);

    // Tick ring
    g.setColour(palette.muted);
    const int ticks = 11;
    for (int i = 0; i < ticks; ++i)
    {
        const float t = start + (end - start) * (float) i / (float) (ticks - 1);
        const float innerR = radius * 1.02f;
        const float outerR = radius * 1.14f;
        g.drawLine(c.x + innerR * std::cos(t - juce::MathConstants<float>::halfPi),
                   c.y + innerR * std::sin(t - juce::MathConstants<float>::halfPi),
                   c.x + outerR * std::cos(t - juce::MathConstants<float>::halfPi),
                   c.y + outerR * std::sin(t - juce::MathConstants<float>::halfPi),
                   i == 0 || i == ticks - 1 ? 1.6f : 0.9f);
    }

    juce::ignoreUnused(slider);
}

void VintageVoxLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int w, int h,
                                             float sliderPos, float minPos, float maxPos,
                                             juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearVertical)
    {
        auto track = juce::Rectangle<float>((float) x + (float) w * 0.5f - 5.0f, (float) y,
                                            10.0f, (float) h);
        drawMetalSlot(g, track);

        const float capW = juce::jmin((float) w * 0.86f, 28.0f);
        const float capH = 16.0f;
        juce::Colour cap = slider.findColour(juce::Slider::thumbColourId);
        auto thumb = juce::Rectangle<float>(capW, capH).withCentre({ (float) x + (float) w * 0.5f, sliderPos });

        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(thumb.translated(0, 1.5f), 3.0f);
        juce::ColourGradient cg(cap.brighter(0.35f), thumb.getX(), thumb.getY(),
                                cap.darker(0.25f), thumb.getX(), thumb.getBottom(), false);
        g.setGradientFill(cg);
        g.fillRoundedRectangle(thumb, 3.0f);
        g.setColour(juce::Colours::white.withAlpha(0.45f));
        g.drawHorizontalLine((int) thumb.getY() + 4, thumb.getX() + 3.0f, thumb.getRight() - 3.0f);
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.drawRoundedRectangle(thumb, 3.0f, 0.8f);
        juce::ignoreUnused(minPos, maxPos);
        return;
    }

    LookAndFeel_V4::drawLinearSlider(g, x, y, w, h, sliderPos, minPos, maxPos, style, slider);
}

void VintageVoxLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                             bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    const bool on = button.getToggleState();

    if (button.getProperties().contains("stomp"))
    {
        auto r = bounds;
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillEllipse(r.translated(0, 3.0f));
        juce::Colour body = on ? palette.accent.darker(0.2f) : juce::Colour(0xff2a2a2a);
        if (highlighted) body = body.brighter(0.1f);
        g.setColour(body);
        g.fillEllipse(r);
        g.setColour(on ? palette.lampOn : palette.lampOff);
        g.fillEllipse(r.reduced(r.getWidth() * 0.32f));
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawEllipse(r.reduced(3.0f), 1.2f);
        return;
    }

    // Default rocker-ish pill
    g.setColour(palette.faceplate);
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(on ? palette.accent : palette.faceplateHi);
    auto lamp = bounds.removeFromLeft(10.0f).reduced(2.0f);
    g.fillEllipse(lamp);
    g.setColour(palette.text.withAlpha(highlighted || down ? 1.0f : 0.85f));
    g.setFont(panelFont(12.0f, true));
    g.drawText(button.getButtonText(), bounds, juce::Justification::centredLeft, false);
}

void VintageVoxLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                 const juce::Colour& bg,
                                                 bool highlighted, bool down)
{
    auto r = button.getLocalBounds().toFloat().reduced(1.0f);
    auto c = bg;
    if (down) c = c.darker(0.25f);
    else if (highlighted) c = c.brighter(0.12f);

    g.setColour(c);
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(palette.accent.withAlpha(button.getToggleState() ? 0.9f : 0.35f));
    g.drawRoundedRectangle(r, 4.0f, 1.2f);
}

void VintageVoxLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                           bool, bool)
{
    g.setFont(getTextButtonFont(button, button.getHeight()));
    g.setColour(palette.text);
    g.drawText(button.getButtonText(), button.getLocalBounds().reduced(4),
               juce::Justification::centred, false);
}

void VintageVoxLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                         int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float>(0, 0, (float) width, (float) height);
    g.setColour(palette.faceplate);
    g.fillRoundedRectangle(r, 3.0f);
    g.setColour(palette.accent.withAlpha(box.hasKeyboardFocus(true) ? 0.9f : 0.4f));
    g.drawRoundedRectangle(r.reduced(0.5f), 3.0f, 1.0f);

    juce::Path arrow;
    const float cx = (float) width - 12.0f;
    const float cy = (float) height * 0.5f;
    arrow.addTriangle(cx - 4.0f, cy - 2.5f, cx + 4.0f, cy - 2.5f, cx, cy + 3.5f);
    g.setColour(palette.accent);
    g.fillPath(arrow);
}

void VintageVoxLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(4, 1, box.getWidth() - 22, box.getHeight() - 2);
    label.setFont(getComboBoxFont(box));
}

void VintageVoxLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    g.fillAll(palette.faceplate);
    g.setColour(palette.accent.withAlpha(0.5f));
    g.drawRect(0, 0, width, height, 1);
}

void VintageVoxLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              bool isSeparator, bool isActive, bool isHighlighted,
                                              bool isTicked, bool, const juce::String& text,
                                              const juce::String&, const juce::Drawable*,
                                              const juce::Colour*)
{
    if (isSeparator)
    {
        g.setColour(palette.muted.withAlpha(0.4f));
        g.fillRect(area.reduced(8, 0).withHeight(1).withY(area.getCentreY()));
        return;
    }

    if (isHighlighted)
        g.fillAll(palette.accent);

    g.setColour(isHighlighted ? juce::Colours::black : (isActive ? palette.text : palette.muted));
    g.setFont(panelFont(13.0f, isTicked));
    auto r = area.reduced(8, 0);
    auto tick = r.removeFromLeft(18).toFloat();
    if (isTicked)
    {
        g.setColour(isHighlighted ? juce::Colours::black : palette.accent);
        g.fillEllipse(tick.withSizeKeepingCentre(7.0f, 7.0f));
    }
    g.setColour(isHighlighted ? juce::Colours::black : (isActive ? palette.text : palette.muted));
    g.drawText(text, r, juce::Justification::centredLeft, false);
}

void VintageVoxLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
                                          bool isMouseOver, bool isMouseDown)
{
    const bool front = button.isFrontTab();
    auto r = button.getActiveArea().toFloat().reduced(3.0f, 6.0f);

    if (front)
    {
        g.setColour(palette.accent);
        g.fillRoundedRectangle(r, 7.0f);
        g.setColour(juce::Colours::black.withAlpha(0.85f));
    }
    else
    {
        if (isMouseOver)
        {
            g.setColour(palette.faceplateHi.withAlpha(0.6f));
            g.fillRoundedRectangle(r, 7.0f);
        }
        g.setColour(palette.muted.brighter(isMouseOver ? 0.25f : 0.0f));
    }

    g.setFont(panelFont(13.0f, true));
    g.drawText(button.getName(), r, juce::Justification::centred, false);
    juce::ignoreUnused(isMouseDown);
}

int VintageVoxLookAndFeel::getTabButtonBestWidth(juce::TabBarButton& button, int)
{
    return 28 + juce::GlyphArrangement::getStringWidthInt(panelFont(14.0f, true), button.getName());
}

void VintageVoxLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(panelFont((float) juce::jmax(11, label.getHeight() - 4)));
    g.drawText(label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
}
