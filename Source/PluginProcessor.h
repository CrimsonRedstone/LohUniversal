#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "Parameters/ParameterLayout.h"
#include "DSP/OrganVoiceManager.h"
#include "FX/Overdrive.h"
#include "FX/ChorusPhaser.h"
#include "FX/DelayReverb.h"
#include "FX/LeslieRotarySpeaker.h"
#include "FX/GraphicEQ.h"
#include "Utf8.h"

#include <array>
#include <atomic>

class LohUniversalAudioProcessor : public juce::AudioProcessor
{
public:
    LohUniversalAudioProcessor();
    ~LohUniversalAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return lohBrandFull(); }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.5; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void saveUserPreset(const juce::File& file);
    bool loadUserPreset(const juce::File& file);

    juce::AudioProcessorValueTreeState apvts;

    OrganVoiceManager& getVoiceManager() noexcept { return voices; }
    const OrganVoiceManager& getVoiceManager() const noexcept { return voices; }

    void handleGuiNoteOn(int manual, int midiNote);
    void handleGuiNoteOff(int manual, int midiNote);

    int getChassisSkin() const noexcept;
    int getDrawbarUiStyle() const noexcept;

    bool isNoteHeld(int midiNote) const noexcept { return voices.isNoteHeld(midiNote); }
    bool isManualNoteHeld(int manual, int midiNote) const noexcept { return voices.isNoteHeld(manual, midiNote); }

private:
    void cacheParameterPointers() noexcept;
    void handleMidiMessage(const juce::MidiMessage& m);
    void applyMidiCc(int cc, int value);
    void fillRenderParams(OrganVoiceManager::RenderParams& p) noexcept;
    int manualForMessage(int midiChannel, int midiNote) const noexcept;
    void drainGuiQueue() noexcept;
    void noteOnInternal(int manual, int midiNote, float velocity) noexcept;
    void noteOffInternal(int manual, int midiNote) noexcept;
    void setSustain(bool on) noexcept;

    OrganVoiceManager voices;
    Overdrive overdrive;
    WahPedal wah;
    ChorusPhaser chorusPhaser;
    DelayReverb delayReverb;
    AC30Cabinet ac30;
    LeslieRotarySpeaker leslie;
    GraphicEQ graphicEq;

    juce::AudioBuffer<float> organScratch;
    juce::AudioBuffer<float> stereoScratch;

    std::atomic<float>* pUpper[8] {};
    std::atomic<float>* pLower[4] {};
    std::atomic<float>* pMixture = nullptr;
    std::atomic<float>* pFlute = nullptr;
    std::atomic<float>* pReed = nullptr;
    std::atomic<float>* pEngine = nullptr;
    std::atomic<float>* pSkin = nullptr;
    std::atomic<float>* pDrawbarStyle = nullptr;
    std::atomic<float>* pVibOn = nullptr;
    std::atomic<float>* pVibSpeed = nullptr;
    std::atomic<float>* pVibDepth = nullptr;
    std::atomic<float>* pPercOn = nullptr;
    std::atomic<float>* pPercDecay = nullptr;
    std::atomic<float>* pPercVol = nullptr;
    std::atomic<float>* pPercHarm = nullptr;
    std::atomic<float>* pDrift = nullptr;
    std::atomic<float>* pClick = nullptr;
    std::atomic<float>* pBleed = nullptr;
    std::atomic<float>* pAmp = nullptr;
    std::atomic<float>* pRotary = nullptr;
    std::atomic<float>* pMaster = nullptr;
    std::atomic<float>* pOverdrive = nullptr;
    std::atomic<float>* pChorus = nullptr;
    std::atomic<float>* pPhaser = nullptr;
    std::atomic<float>* pDelay = nullptr;
    std::atomic<float>* pReverb = nullptr;
    std::atomic<float>* pWah = nullptr;
    std::atomic<float>* pOdOn = nullptr;
    std::atomic<float>* pChOn = nullptr;
    std::atomic<float>* pPhOn = nullptr;
    std::atomic<float>* pDlOn = nullptr;
    std::atomic<float>* pRvOn = nullptr;
    std::atomic<float>* pWhOn = nullptr;
    std::atomic<float>* pDelayTime = nullptr;
    std::atomic<float>* pDelayFb = nullptr;
    std::atomic<float>* pRevSize = nullptr;
    std::atomic<float>* pAmpDrive = nullptr;
    std::atomic<float>* pAmpBass = nullptr;
    std::atomic<float>* pAmpTreble = nullptr;
    std::atomic<float>* pAmpCut = nullptr;
    std::atomic<float>* pRotaryBal = nullptr;
    std::atomic<float>* pEqOn = nullptr;
    std::atomic<float>* pEqGain[5] {};
    std::atomic<float>* pMains = nullptr;
    std::atomic<float>* pMidiMode = nullptr;
    std::atomic<float>* pSplit = nullptr;
    std::atomic<float>* pDetune[12] {};

    static constexpr int kGuiQueueSize = 64;
    std::atomic<uint32_t> guiNoteQueue[kGuiQueueSize] {};
    std::atomic<int> guiWrite { 0 };
    std::atomic<int> guiRead { 0 };
    int scratchCapacity = 0;
    int currentProgram = 0;
    bool sustainPedal = false;
    std::array<std::array<bool, 128>, OrganVoiceManager::NumManuals> latched {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LohUniversalAudioProcessor)
};
