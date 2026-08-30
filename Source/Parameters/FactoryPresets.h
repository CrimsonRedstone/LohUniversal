#pragma once

#include "ParameterLayout.h"
#include <array>

struct FactoryPreset
{
    const char* name;

    float upper[8];
    float lower[4];
    float mixture;
    float flute;
    float reed;
    int engine;
    int skin;
    bool vibrato;
    float vibSpeed;
    float vibDepth;
    bool perc;
    float percDecay;
    int percVol;
    int percHarmonic;
    float drift;
    float click;
    float bleed;
    int amp;
    int rotary;
    float volume;
    float overdrive;
    bool odOn;
    float chorus;
    bool chOn;
    float phaser;
    bool phOn;
    float delay;
    bool dlOn;
    float reverb;
    bool rvOn;
    float ampDrive;
    float ampBass;
    float ampTreble;
    float ampCut;
    float rotaryBal;
};

inline constexpr int kNumFactoryPresets = 9;

inline constexpr FactoryPreset kFactoryPresets[kNumFactoryPresets] = {
    // 1 Crimson Combo — 16'-heavy, dark flute, cabinet grind, spring reverb.
    { "Crimson Combo",
      { 0.92f, 0.62f, 0.00f, 0.22f, 0.00f, 0.00f, 0.00f, 0.00f },
      { 0.78f, 0.70f, 0.12f, 0.00f },
      0.00f, 0.90f, 0.32f, EngineMode::VoxV301E, ChassisSkin::Default,
      true, 6.2f, 0.36f, false, 280.0f, PercVolume::Normal, 0,
      0.10f, 0.12f, -68.0f, AmpType::AC30Cabinet, RotarySpeed::Slow, 0.70f,
      0.48f, true, 0.0f, false, 0.0f, false, 0.0f, false, 0.58f, true,
      0.42f, 4.5f, -5.0f, 0.62f, 0.38f },

    // 2 Combo Clean — flute-forward 8' + 4'
    { "Combo Clean",
      { 0.00f, 0.88f, 0.00f, 0.72f, 0.00f, 0.18f, 0.00f, 0.00f },
      { 0.55f, 0.80f, 0.35f, 0.00f },
      0.00f, 0.82f, 0.22f, EngineMode::VoxV301E, ChassisSkin::Default,
      false, 6.5f, 0.30f, false, 280.0f, PercVolume::Normal, 0,
      0.12f, 0.22f, -64.0f, AmpType::Direct, RotarySpeed::Slow, 0.74f,
      0.10f, false, 0.0f, false, 0.0f, false, 0.0f, false, 0.12f, true,
      0.20f, 1.0f, 0.0f, 0.25f, 0.50f },

    // 3 Super Combo — brighter reed, dual-manual
    { "Super Combo",
      { 0.40f, 0.90f, 0.25f, 0.70f, 0.20f, 0.35f, 0.00f, 0.10f },
      { 0.70f, 0.85f, 0.50f, 0.15f },
      0.20f, 0.55f, 0.70f, EngineMode::VoxSuper, ChassisSkin::Default,
      true, 6.8f, 0.40f, true, 220.0f, PercVolume::Normal, 0,
      0.18f, 0.28f, -58.0f, AmpType::AC30Cabinet, RotarySpeed::Slow, 0.70f,
      0.22f, true, 0.0f, false, 0.0f, false, 0.0f, false, 0.16f, true,
      0.30f, 2.0f, 1.0f, 0.30f, 0.50f },

    // 4 Saw Combo — leaky-integrator saw path
    { "Saw Combo",
      { 0.55f, 0.80f, 0.00f, 0.65f, 0.00f, 0.25f, 0.00f, 0.00f },
      { 0.60f, 0.75f, 0.40f, 0.00f },
      0.00f, 0.60f, 0.55f, EngineMode::JenningsJ70, ChassisSkin::Default,
      true, 5.8f, 0.45f, false, 400.0f, PercVolume::Soft, 0,
      0.22f, 0.30f, -55.0f, AmpType::Direct, RotarySpeed::Slow, 0.72f,
      0.15f, false, 0.18f, true, 0.0f, false, 0.0f, false, 0.20f, true,
      0.22f, 0.0f, 2.0f, 0.20f, 0.50f },

    // 5 Sixties Combo — 16' + 8' + 4' bite
    { "Sixties Combo",
      { 0.85f, 0.90f, 0.00f, 0.80f, 0.00f, 0.15f, 0.00f, 0.00f },
      { 0.80f, 0.70f, 0.20f, 0.00f },
      0.00f, 0.45f, 0.75f, EngineMode::VoxV301E, ChassisSkin::Crimson,
      true, 6.2f, 0.38f, false, 280.0f, PercVolume::Normal, 0,
      0.20f, 0.35f, -60.0f, AmpType::AC30Cabinet, RotarySpeed::Slow, 0.68f,
      0.35f, true, 0.0f, false, 0.12f, false, 0.0f, false, 0.10f, true,
      0.40f, 3.0f, -2.0f, 0.45f, 0.42f },

    // 6 Rising Sun — hymnal 16' flute, slow rotary
    { "Rising Sun",
      { 0.90f, 0.70f, 0.15f, 0.40f, 0.00f, 0.00f, 0.00f, 0.00f },
      { 0.75f, 0.60f, 0.20f, 0.00f },
      0.00f, 0.90f, 0.15f, EngineMode::VoxV301E, ChassisSkin::CitrusFrost,
      false, 5.5f, 0.25f, false, 600.0f, PercVolume::Soft, 0,
      0.10f, 0.18f, -70.0f, AmpType::LeslieRotary, RotarySpeed::Slow, 0.72f,
      0.08f, false, 0.0f, false, 0.0f, false, 0.0f, false, 0.22f, true,
      0.18f, 3.5f, -3.0f, 0.50f, 0.32f },

    // 6 Reed Bite — bright mutation ranks
    { "Reed Bite",
      { 0.00f, 0.55f, 0.70f, 0.40f, 0.65f, 0.50f, 0.30f, 0.20f },
      { 0.20f, 0.60f, 0.45f, 0.40f },
      0.55f, 0.20f, 0.95f, EngineMode::VoxSuper, ChassisSkin::Nosmirc,
      true, 7.4f, 0.32f, true, 140.0f, PercVolume::Normal, 1,
      0.14f, 0.40f, -62.0f, AmpType::Direct, RotarySpeed::Fast, 0.66f,
      0.18f, false, 0.0f, false, 0.25f, true, 0.0f, false, 0.08f, true,
      0.25f, -1.0f, 4.0f, 0.15f, 0.62f },

    // 7 Church Mixture — full drawbar + mixture IV
    { "Church Mixture",
      { 0.70f, 0.80f, 0.45f, 0.70f, 0.40f, 0.55f, 0.30f, 0.35f },
      { 0.65f, 0.70f, 0.50f, 0.55f },
      0.80f, 0.75f, 0.40f, EngineMode::Extended, ChassisSkin::Default,
      false, 6.0f, 0.20f, true, 450.0f, PercVolume::Soft, 0,
      0.08f, 0.16f, -72.0f, AmpType::LeslieRotary, RotarySpeed::Slow, 0.70f,
      0.05f, false, 0.10f, false, 0.0f, false, 0.0f, false, 0.28f, true,
      0.12f, 2.0f, -1.0f, 0.35f, 0.40f },

    // 8 Junkyard Relic — broken chassis, heavy artefacts
    { "Junkyard Relic",
      { 0.60f, 0.85f, 0.10f, 0.55f, 0.00f, 0.30f, 0.00f, 0.00f },
      { 0.50f, 0.70f, 0.30f, 0.00f },
      0.10f, 0.50f, 0.60f, EngineMode::VoxV301E, ChassisSkin::Junkyard,
      true, 5.2f, 0.55f, true, 180.0f, PercVolume::Normal, 2,
      0.85f, 0.70f, -42.0f, AmpType::AC30Cabinet, RotarySpeed::Slow, 0.64f,
      0.45f, true, 0.0f, false, 0.15f, true, 0.12f, false, 0.18f, true,
      0.55f, 1.0f, -1.0f, 0.30f, 0.50f }
};

inline void applyFactoryPreset(juce::AudioProcessorValueTreeState& apvts, int index)
{
    if (! juce::isPositiveAndBelow(index, kNumFactoryPresets))
        return;

    const auto& p = kFactoryPresets[index];

    auto setFloat = [&apvts](const char* id, float actual)
    {
        if (auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts.getParameter(id)))
            param->setValueNotifyingHost(param->convertTo0to1(actual));
    };
    auto setNorm = [&apvts](const char* id, float norm01)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, norm01));
    };
    auto setChoice = [&apvts](const char* id, int item)
    {
        if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(id)))
        {
            const float n = param->convertTo0to1(static_cast<float>(item));
            param->setValueNotifyingHost(n);
        }
        else if (auto* ip = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(id)))
        {
            ip->setValueNotifyingHost(ip->convertTo0to1(static_cast<float>(item)));
        }
    };
    auto setBool = [&apvts](const char* id, bool v)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(v ? 1.0f : 0.0f);
    };

    for (int i = 0; i < 8; ++i) setNorm(kUpperDrawbarIds[i], p.upper[i]);
    for (int i = 0; i < 4; ++i) setNorm(kLowerDrawbarIds[i], p.lower[i]);
    setNorm(ParamIDs::DRAWBAR_MIXTURE_IV, p.mixture);
    setNorm(ParamIDs::TONE_FLUTE, p.flute);
    setNorm(ParamIDs::TONE_REED, p.reed);
    setChoice(ParamIDs::ENGINE_MODE, p.engine);
    setChoice(ParamIDs::CHASSIS_SKIN, p.skin);
    setBool(ParamIDs::VIBRATO_TOGGLE, p.vibrato);
    setFloat(ParamIDs::VIBRATO_SPEED, p.vibSpeed);
    setNorm(ParamIDs::VIBRATO_DEPTH, p.vibDepth);
    setBool(ParamIDs::PERC_TOGGLE, p.perc);
    setFloat(ParamIDs::PERC_DECAY, p.percDecay);
    setChoice(ParamIDs::PERC_VOLUME, p.percVol);
    setChoice(ParamIDs::PERC_HARMONIC, p.percHarmonic);
    setNorm(ParamIDs::MICRO_DRIFT, p.drift);
    setNorm(ParamIDs::MICRO_CLICK, p.click);
    setFloat(ParamIDs::MICRO_BLEED, p.bleed);
    setChoice(ParamIDs::AMP_TYPE, p.amp);
    setChoice(ParamIDs::ROTARY_SPEED, p.rotary);
    setFloat(ParamIDs::MASTER_VOLUME, p.volume);
    setNorm(ParamIDs::OVERDRIVE, p.overdrive);
    setBool(ParamIDs::FX_OVERDRIVE_ON, p.odOn);
    setNorm(ParamIDs::CHORUS, p.chorus);
    setBool(ParamIDs::FX_CHORUS_ON, p.chOn);
    setNorm(ParamIDs::PHASER, p.phaser);
    setBool(ParamIDs::FX_PHASER_ON, p.phOn);
    setNorm(ParamIDs::DELAY, p.delay);
    setBool(ParamIDs::FX_DELAY_ON, p.dlOn);
    setNorm(ParamIDs::REVERB, p.reverb);
    setBool(ParamIDs::FX_REVERB_ON, p.rvOn);
    setNorm(ParamIDs::AMP_DRIVE, p.ampDrive);
    setFloat(ParamIDs::AMP_BASS, p.ampBass);
    setFloat(ParamIDs::AMP_TREBLE, p.ampTreble);
    setNorm(ParamIDs::AMP_CUT, p.ampCut);
    setNorm(ParamIDs::ROTARY_BAL, p.rotaryBal);
    setBool(ParamIDs::EQ_ON, false);
    for (int i = 0; i < 5; ++i)
        setFloat(kEqGainIds[i], 0.0f);
}
