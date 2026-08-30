#pragma once

#include "TopOctaveOscillator.h"
#include "FrequencyDivider.h"
#include "VoicingFilters.h"
#include "VibratoLFO.h"
#include "PercussionEngine.h"
#include "KeyNoiseGenerator.h"
#include "../Parameters/ParameterLayout.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <cstdint>

class OrganVoiceManager
{
public:
    enum Manual { Upper = 0, Lower = 1, Bass = 2, NumManuals = 3 };

    void prepare(double sampleRate, int samplesPerBlock) noexcept;
    void reset() noexcept;

    void noteOn(int manual, int midiNote, float velocity) noexcept;
    void noteOff(int manual, int midiNote) noexcept;
    void allNotesOff() noexcept;

    bool isNoteHeld(int midiNote) const noexcept;
    bool isNoteHeld(int manual, int midiNote) const noexcept;

    /** Lock-free snapshot for the GUI (bit i of word n/64). */
    uint64_t heldBitsLow() const noexcept { return heldGuiLow.load(std::memory_order_relaxed); }
    uint64_t heldBitsHigh() const noexcept { return heldGuiHigh.load(std::memory_order_relaxed); }

    struct RenderParams
    {
        float upperDrawbar[8] {};
        float lowerDrawbar[4] {};
        float mixtureIV = 0.0f;
        float flute = 0.7f;
        float reed = 0.35f;
        int engineMode = 0;
        int chassisSkin = 0;
        bool vibratoOn = false;
        float vibratoSpeed = 6.5f;
        float vibratoDepth = 0.35f;
        bool percOn = false;
        float percDecayMs = 280.0f;
        bool percSoft = false;
        int percHarmonic = 0; // 0=4', 1=2-2/3', 2=2'
        float microDrift = 0.15f;
        float microClick = 0.25f;
        float microBleedDb = -62.0f;
        float oscDetune[12] {};
        bool mainsOn = true;
        float masterVolume = 0.72f;
    };

    void render(float* monoOut, int numSamples, const RenderParams& p) noexcept;

    TopOctaveOscillator& oscillators() noexcept { return osc; }

private:
    void publishHeld() noexcept;
    float mixManual(int manual, int midiNote, const RenderParams& p,
                    float& percAccum) noexcept;

    TopOctaveOscillator osc;
    VoicingFilters voicing;
    VibratoLFO vibrato;
    PercussionEngine percussion;
    KeyNoiseGenerator keyNoise;

    std::array<std::array<bool, 128>, NumManuals> held {};
    std::atomic<uint64_t> heldGuiLow { 0 };
    std::atomic<uint64_t> heldGuiHigh { 0 };

    double sampleRate = 44100.0;
    float humPhase = 0.0f;
    float humPhase2 = 0.0f;
    float preampState = 0.0f;
    uint32_t crosstalkRng = 0xBEEFu;
    float lastClickAmount = 0.25f;
};
