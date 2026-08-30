#include "OrganVoiceManager.h"
#include <cmath>

void OrganVoiceManager::prepare(double sr, int samplesPerBlock) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    osc.prepare(sampleRate);
    voicing.prepare(sampleRate, samplesPerBlock);
    vibrato.prepare(sampleRate);
    percussion.prepare(sampleRate);
    keyNoise.prepare(sampleRate);
    reset();
}

void OrganVoiceManager::reset() noexcept
{
    for (auto& m : held)
        m.fill(false);

    osc.reset();
    voicing.reset();
    vibrato.reset();
    percussion.reset();
    keyNoise.reset();
    humPhase = 0.0f;
    humPhase2 = 0.0f;
    preampState = 0.0f;
    publishHeld();
}

void OrganVoiceManager::publishHeld() noexcept
{
    uint64_t lo = 0, hi = 0;
    for (int n = 0; n < 128; ++n)
    {
        const bool on = held[Upper][static_cast<size_t>(n)]
                     || held[Lower][static_cast<size_t>(n)]
                     || held[Bass][static_cast<size_t>(n)];
        if (! on)
            continue;
        if (n < 64)
            lo |= (uint64_t(1) << n);
        else
            hi |= (uint64_t(1) << (n - 64));
    }
    heldGuiLow.store(lo, std::memory_order_relaxed);
    heldGuiHigh.store(hi, std::memory_order_relaxed);
}

bool OrganVoiceManager::isNoteHeld(int midiNote) const noexcept
{
    if (midiNote < 0 || midiNote > 127)
        return false;
    return held[Upper][static_cast<size_t>(midiNote)]
        || held[Lower][static_cast<size_t>(midiNote)]
        || held[Bass][static_cast<size_t>(midiNote)];
}

bool OrganVoiceManager::isNoteHeld(int manual, int midiNote) const noexcept
{
    if (midiNote < 0 || midiNote > 127 || manual < 0 || manual >= NumManuals)
        return false;
    return held[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)];
}

void OrganVoiceManager::noteOn(int manual, int midiNote, float) noexcept
{
    if (midiNote < 0 || midiNote > 127 || manual < 0 || manual >= NumManuals)
        return;

    const bool already = held[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)];
    held[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)] = true;

    if (! already)
    {
        if (manual == Upper)
            percussion.trigger();

        keyNoise.triggerOn(midiNote, lastClickAmount);
        osc.applyNoteOnJitter(midiNote % 12, static_cast<uint32_t>(midiNote * 17 + manual * 31));
        publishHeld();
    }
}

void OrganVoiceManager::noteOff(int manual, int midiNote) noexcept
{
    if (midiNote < 0 || midiNote > 127 || manual < 0 || manual >= NumManuals)
        return;

    if (held[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)])
        keyNoise.triggerOff(midiNote, lastClickAmount);

    held[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)] = false;
    publishHeld();
}

void OrganVoiceManager::allNotesOff() noexcept
{
    for (auto& m : held)
        m.fill(false);
    publishHeld();
}

float OrganVoiceManager::mixManual(int manual, int midiNote, const RenderParams& p,
                                   float& percAccum) noexcept
{
    const uint32_t* phases = osc.getPhases();
    const uint32_t* incs   = osc.getIncrements();

    float s = 0.0f;

    auto addRank = [&](int offset, float level)
    {
        if (level <= 0.0001f)
            return;
        s += FrequencyDivider::squareForMidi(midiNote + offset, phases, incs) * level;
    };

    if (manual == Upper)
    {
        for (int r = 0; r < 8; ++r)
            addRank(kUpperRankOffsets[r], p.upperDrawbar[r]);

        if (p.mixtureIV > 0.0001f)
        {
            const float m = p.mixtureIV * 0.25f;
            for (int r = 0; r < 4; ++r)
                addRank(kMixtureOffsets[r], m);
        }

        int percOffset = 12; // 4'
        if (p.percHarmonic == PercHarmonic::Foot2_2_3) percOffset = 19;
        else if (p.percHarmonic == PercHarmonic::Foot2) percOffset = 24;
        percAccum += FrequencyDivider::squareForMidi(midiNote + percOffset, phases, incs);
    }
    else if (manual == Lower)
    {
        addRank(-12, p.lowerDrawbar[0]);
        addRank(  0, p.lowerDrawbar[1]);
        addRank( 12, p.lowerDrawbar[2]);
        if (p.lowerDrawbar[3] > 0.0001f)
        {
            const float m = p.lowerDrawbar[3] * 0.25f;
            for (int r = 0; r < 4; ++r)
                addRank(kMixtureOffsets[r], m);
        }
    }
    else // Bass: 16' and 8' only
    {
        addRank(-12, p.lowerDrawbar[0] > 0.001f ? p.lowerDrawbar[0] : p.upperDrawbar[0]);
        addRank(  0, p.lowerDrawbar[1] > 0.001f ? p.lowerDrawbar[1] : p.upperDrawbar[1]);
        if (p.lowerDrawbar[0] <= 0.001f && p.upperDrawbar[0] <= 0.001f)
            addRank(-12, 0.7f);
        if (p.lowerDrawbar[1] <= 0.001f && p.upperDrawbar[1] <= 0.001f)
            addRank(0, 0.7f);
    }

    return s;
}

void OrganVoiceManager::render(float* monoOut, int numSamples, const RenderParams& p) noexcept
{
    juce::FloatVectorOperations::clear(monoOut, numSamples);

    if (! p.mainsOn)
    {
        // Still run LFOs so they don't jump when mains come back.
        vibrato.setEnabled(false);
        for (int i = 0; i < numSamples; ++i)
            (void) vibrato.nextRatio();
        return;
    }

    for (int i = 0; i < 12; ++i)
        osc.setDetuneCents(i, p.oscDetune[i]);

    vibrato.setEnabled(p.vibratoOn);
    vibrato.setRate(p.vibratoSpeed);
    vibrato.setDepth(p.vibratoDepth);

    percussion.setEnabled(p.percOn);
    percussion.setDecayMs(p.percDecayMs);
    percussion.setSoft(p.percSoft);

    voicing.setEngineMode(p.engineMode);
    lastClickAmount = p.microClick;

    const float bleedLin = juce::Decibels::decibelsToGain(p.microBleedDb);
    const float humInc = static_cast<float>(2.0 * juce::MathConstants<double>::pi * 60.0 / sampleRate);
    const float humInc50 = static_cast<float>(2.0 * juce::MathConstants<double>::pi * 50.0 / sampleRate);

    for (int n = 0; n < numSamples; ++n)
    {
        const float vibRatio = vibrato.nextRatio();
        osc.tick(vibRatio, p.microDrift, p.chassisSkin);

        float mix = 0.0f;
        float percBus = 0.0f;
        int upperCount = 0;

        for (int note = 0; note < 128; ++note)
        {
            if (held[Upper][static_cast<size_t>(note)])
            {
                mix += mixManual(Upper, note, p, percBus);
                ++upperCount;
            }
            if (held[Lower][static_cast<size_t>(note)])
            {
                float dummy = 0.0f;
                mix += mixManual(Lower, note, p, dummy) * 0.92f;
            }
            if (held[Bass][static_cast<size_t>(note)])
            {
                float dummy = 0.0f;
                mix += mixManual(Bass, note, p, dummy) * 1.05f;
            }
        }

        if (upperCount > 0)
            percBus /= static_cast<float>(upperCount);

        mix += percussion.process(percBus);

        // Busbar crosstalk: a whisper of neighbouring pitch-class squares.
        if (bleedLin > 0.00001f)
        {
            const uint32_t* phases = osc.getPhases();
            const uint32_t* incs   = osc.getIncrements();
            float xtalk = 0.0f;
            for (int pc = 0; pc < 12; ++pc)
                xtalk += FrequencyDivider::squareAtBit(phases[pc], incs[pc], 29);
            mix += xtalk * bleedLin * 0.08f;
        }

        mix += keyNoise.process();

        // Transformer hum (50 + 60 Hz + 2nd harmonic).
        humPhase += humInc;
        if (humPhase > juce::MathConstants<float>::twoPi)
            humPhase -= juce::MathConstants<float>::twoPi;
        humPhase2 += humInc50;
        if (humPhase2 > juce::MathConstants<float>::twoPi)
            humPhase2 -= juce::MathConstants<float>::twoPi;

        const float hum = std::sin(humPhase) * 0.65f
                        + std::sin(humPhase * 2.0f) * 0.22f
                        + std::sin(humPhase2) * 0.35f;
        mix += hum * bleedLin;

        monoOut[n] = mix * 0.085f; // headroom for many keys
    }

    voicing.process(monoOut, numSamples, p.flute, p.reed);

    // Master preamp tanh saturation (always on, mild).
    for (int n = 0; n < numSamples; ++n)
    {
        const float x = monoOut[n] * 1.35f;
        monoOut[n] = std::tanh(x);
    }

    const float vol = juce::jlimit(0.0f, 1.0f, p.masterVolume);
    juce::FloatVectorOperations::multiply(monoOut, vol, numSamples);
}
