#pragma once

#include "PluginProcessor.h"
#include "GUI/LookAndFeel/VintageVoxLookAndFeel.h"
#include "GUI/Views/MainPanelView.h"
#include "GUI/Views/OpenLidView.h"
#include "GUI/Views/FXRackView.h"
#include "GUI/Views/EQView.h"
#include "GUI/HostAutomation.h"
#include <array>
#include <memory>

class LohUniversalAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::KeyListener,
                                      private juce::Timer
{
public:
    explicit LohUniversalAudioProcessorEditor(LohUniversalAudioProcessor&);
    ~LohUniversalAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void parentHierarchyChanged() override;
    void visibilityChanged() override;

    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) override;

    static constexpr int kDesignW = 1280;
    static constexpr int kDesignH = 720;
    static constexpr int kHeaderH = 70;

private:
    void timerCallback() override;
    void applySkin();
    void layoutDesign();
    void releaseComputerKeys();
    int computerKeyToNote(const juce::KeyPress& key, bool& lower) const noexcept;
    void savePreset();
    void loadPreset();
    void applyWindowIcon();

    LohUniversalAudioProcessor& processor;
    VintageVoxLookAndFeel lnf;

    juce::Component ui;
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    MainPanelView panelView;
    OpenLidView lidView;
    FXRackView fxView;
    EQView eqView;

    HostComboBox skinBox, engineBox, presetBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> skinAtt, engineAtt;
    juce::TextButton saveBtn { "Save" }, loadBtn { "Load" };
    juce::HyperlinkButton credit;
    HostSlider volume { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtt;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Image pluginIcon;

    int lastSkin = -1;
    int lastStyle = -1;
    std::array<int, 512> computerHeld {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LohUniversalAudioProcessorEditor)
};
