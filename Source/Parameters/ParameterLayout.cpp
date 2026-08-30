#include "ParameterLayout.h"

namespace
{
    juce::String percentText(float value, int)
    {
        return juce::String(juce::roundToInt(value * 100.0f)) + "%";
    }

    juce::String hzText(float value, int)
    {
        return juce::String(value, 2) + " Hz";
    }

    juce::String msText(float value, int)
    {
        return juce::String(juce::roundToInt(value)) + " ms";
    }

    juce::String dbText(float value, int)
    {
        return juce::String(value, 1) + " dB";
    }

    juce::String centsText(float value, int)
    {
        return juce::String(value, 1) + " ct";
    }

    juce::AudioParameterFloat* makePercent(const juce::String& id,
                                           const juce::String& name,
                                           float defaultValue)
    {
        juce::NormalisableRange<float> range(0.0f, 1.0f, 0.0f, 1.0f);
        return new juce::AudioParameterFloat(
            juce::ParameterID{ id, 1 }, name, range, defaultValue,
            juce::AudioParameterFloatAttributes().withLabel("%").withStringFromValueFunction(percentText));
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    const float upperDefaults[8] = { 0.0f, 0.85f, 0.0f, 0.70f, 0.0f, 0.15f, 0.0f, 0.0f };
    for (int i = 0; i < 8; ++i)
    {
        params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
            makePercent(kUpperDrawbarIds[i],
                        juce::String("Upper ") + kUpperDrawbarLabels[i],
                        upperDefaults[i])));
    }

    const float lowerDefaults[4] = { 0.60f, 0.80f, 0.40f, 0.0f };
    for (int i = 0; i < 4; ++i)
    {
        params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
            makePercent(kLowerDrawbarIds[i],
                        juce::String("Lower ") + kLowerDrawbarLabels[i],
                        lowerDefaults[i])));
    }

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::DRAWBAR_MIXTURE_IV, "Mixture IV", 0.0f)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::TONE_FLUTE, "Flute", 0.70f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::TONE_REED, "Reed", 0.35f)));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::ENGINE_MODE, 1 }, "Engine",
        juce::StringArray{ "Model 301", "Super Combo", "Saw Combo", "Extended" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::CHASSIS_SKIN, 1 }, "Chassis Skin",
        juce::StringArray{ "Default", "Crimson", "Nosmirc", "Citrus Frost", "Junkyard" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::DRAWBAR_UI_STYLE, 1 }, "LED Drawbars", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::VIBRATO_TOGGLE, 1 }, "Vibrato", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::VIBRATO_SPEED, 1 }, "Vibrato Speed",
        juce::NormalisableRange<float>(4.0f, 8.5f, 0.0f, 1.0f), 6.5f,
        juce::AudioParameterFloatAttributes().withLabel("Hz").withStringFromValueFunction(hzText)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::VIBRATO_DEPTH, "Vibrato Depth", 0.35f)));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::PERC_TOGGLE, 1 }, "Percussion", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::PERC_DECAY, 1 }, "Percussion Decay",
        juce::NormalisableRange<float>(20.0f, 1500.0f, 0.0f, 0.4f), 280.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms").withStringFromValueFunction(msText)));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::PERC_VOLUME, 1 }, "Percussion Volume",
        juce::StringArray{ "Soft", "Normal" }, 1));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::PERC_HARMONIC, 1 }, "Percussion Harmonic",
        juce::StringArray{ "4'", "2-2/3'", "2'" }, 0));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::MICRO_DRIFT, "Oscillator Drift", 0.15f)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::MICRO_CLICK, "Key Click", 0.25f)));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::MICRO_BLEED, 1 }, "Transformer Bleed",
        juce::NormalisableRange<float>(-90.0f, -20.0f, 0.0f, 1.0f), -62.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB").withStringFromValueFunction(dbText)));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::AMP_TYPE, 1 }, "Amp Type",
        juce::StringArray{ "Direct", "Combo Cabinet", "Rotary Speaker" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::ROTARY_SPEED, 1 }, "Rotary Speed",
        juce::StringArray{ "Stop", "Slow", "Fast" }, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::MASTER_VOLUME, 1 }, "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.5f), 0.72f,
        juce::AudioParameterFloatAttributes().withLabel("%").withStringFromValueFunction(percentText)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::OVERDRIVE, "Overdrive", 0.18f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::CHORUS,    "Chorus",    0.0f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::PHASER,    "Phaser",    0.55f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::DELAY,     "Delay",     0.0f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::REVERB,    "Reverb",    0.22f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(makePercent(ParamIDs::WAH,       "Wah",       0.0f)));

    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_OVERDRIVE_ON, 1 }, "Overdrive On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_CHORUS_ON,    1 }, "Chorus On",    false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_PHASER_ON,    1 }, "Phaser On",    false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_DELAY_ON,     1 }, "Delay On",     false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_REVERB_ON,    1 }, "Reverb On",    true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ ParamIDs::FX_WAH_ON,       1 }, "Wah On",       false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::DELAY_TIME, 1 }, "Delay Time",
        juce::NormalisableRange<float>(40.0f, 900.0f, 0.0f, 0.4f), 280.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms").withStringFromValueFunction(msText)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::DELAY_FEEDBACK, "Delay Feedback", 0.28f)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::REVERB_SIZE, "Reverb Size", 0.45f)));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::AMP_DRIVE, "Amp Drive", 0.35f)));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::AMP_BASS, 1 }, "Amp Bass",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.0f, 1.0f), 2.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB").withStringFromValueFunction(dbText)));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::AMP_TREBLE, 1 }, "Amp Treble",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.0f, 1.0f), -2.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB").withStringFromValueFunction(dbText)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::AMP_CUT, "Amp Cut", 0.40f)));
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(
        makePercent(ParamIDs::ROTARY_BAL, "Rotary Balance", 0.45f)));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::EQ_ON, 1 }, "EQ On", false));
    const float eqDefaults[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    const char* eqNames[5] = { "EQ 80 Hz", "EQ 250 Hz", "EQ 800 Hz", "EQ 2.5 kHz", "EQ 8 kHz" };
    for (int i = 0; i < 5; ++i)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ kEqGainIds[i], 1 }, eqNames[i],
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.0f, 1.0f), eqDefaults[i],
            juce::AudioParameterFloatAttributes().withLabel("dB").withStringFromValueFunction(dbText)));
    }

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::MAINS_TOGGLE, 1 }, "Mains", true));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::MIDI_MODE, 1 }, "MIDI Mode",
        juce::StringArray{ "Upper (Omni)", "Split", "3-Channel" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{ ParamIDs::SPLIT_POINT, 1 }, "Split Point", 21, 108, 60));

    for (int i = 0; i < 12; ++i)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ kOscDetuneIds[i], 1 },
            juce::String("Osc ") + kPitchClassNames[i] + " Detune",
            juce::NormalisableRange<float>(-50.0f, 50.0f, 0.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("ct").withStringFromValueFunction(centsText)));
    }

    return { params.begin(), params.end() };
}
