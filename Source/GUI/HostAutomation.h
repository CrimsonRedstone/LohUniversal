#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

inline constexpr const char* kHostParamProperty = "lohParamId";

inline void tagHostParam (juce::Component& c, const juce::String& paramId)
{
    c.getProperties().set (kHostParamProperty, paramId);
}

inline bool showHostParamMenu (juce::Component& origin, juce::Point<int> screenPos)
{
    juce::String id;
    for (auto* c = &origin; c != nullptr; c = c->getParentComponent())
    {
        id = c->getProperties()[kHostParamProperty].toString();
        if (id.isNotEmpty())
            break;
    }
    if (id.isEmpty())
        return false;

    auto* editor = origin.findParentComponentOfClass<juce::AudioProcessorEditor>();
    if (editor == nullptr)
        return false;

    juce::AudioProcessorParameter* param = nullptr;
    for (auto* p : editor->processor.getParameters())
        if (auto* withId = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
            if (withId->paramID == id)
            {
                param = p;
                break;
            }

    if (param == nullptr)
        return false;

    if (auto* ctx = editor->getHostContext())
        if (auto menu = ctx->getContextMenuForParameter (param))
        {
            menu->showNativeMenu (screenPos);
            return true;
        }
    return false;
}

/** Slider that yields right-click to the host (FL "create automation clip"). */
class HostSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu() && showHostParamMenu (*this, e.getScreenPosition()))
            return;
        juce::Slider::mouseDown (e);
    }
};

class HostComboBox : public juce::ComboBox
{
public:
    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu() && showHostParamMenu (*this, e.getScreenPosition()))
            return;
        juce::ComboBox::mouseDown (e);
    }
};

class HostToggle : public juce::ToggleButton
{
public:
    using juce::ToggleButton::ToggleButton;

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu() && showHostParamMenu (*this, e.getScreenPosition()))
            return;
        juce::ToggleButton::mouseDown (e);
    }
};
