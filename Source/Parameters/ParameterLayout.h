#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    // Upper drawbars: 16', 8', 5-1/3', 4', 2-2/3', 2', 1-3/5', 1'
    inline constexpr const char* DRAWBAR_UPPER_1 = "DRAWBAR_UPPER_1";
    inline constexpr const char* DRAWBAR_UPPER_2 = "DRAWBAR_UPPER_2";
    inline constexpr const char* DRAWBAR_UPPER_3 = "DRAWBAR_UPPER_3";
    inline constexpr const char* DRAWBAR_UPPER_4 = "DRAWBAR_UPPER_4";
    inline constexpr const char* DRAWBAR_UPPER_5 = "DRAWBAR_UPPER_5";
    inline constexpr const char* DRAWBAR_UPPER_6 = "DRAWBAR_UPPER_6";
    inline constexpr const char* DRAWBAR_UPPER_7 = "DRAWBAR_UPPER_7";
    inline constexpr const char* DRAWBAR_UPPER_8 = "DRAWBAR_UPPER_8";

    inline constexpr const char* DRAWBAR_LOWER_1 = "DRAWBAR_LOWER_1";
    inline constexpr const char* DRAWBAR_LOWER_2 = "DRAWBAR_LOWER_2";
    inline constexpr const char* DRAWBAR_LOWER_3 = "DRAWBAR_LOWER_3";
    inline constexpr const char* DRAWBAR_LOWER_4 = "DRAWBAR_LOWER_4";

    inline constexpr const char* DRAWBAR_MIXTURE_IV = "DRAWBAR_MIXTURE_IV";

    inline constexpr const char* TONE_FLUTE = "TONE_FLUTE";
    inline constexpr const char* TONE_REED  = "TONE_REED";

    inline constexpr const char* ENGINE_MODE      = "ENGINE_MODE";
    inline constexpr const char* CHASSIS_SKIN     = "CHASSIS_SKIN";
    inline constexpr const char* DRAWBAR_UI_STYLE = "DRAWBAR_UI_STYLE";

    inline constexpr const char* VIBRATO_TOGGLE = "VIBRATO_TOGGLE";
    inline constexpr const char* VIBRATO_SPEED  = "VIBRATO_SPEED";
    inline constexpr const char* VIBRATO_DEPTH  = "VIBRATO_DEPTH";

    inline constexpr const char* PERC_TOGGLE = "PERC_TOGGLE";
    inline constexpr const char* PERC_DECAY  = "PERC_DECAY";
    inline constexpr const char* PERC_VOLUME = "PERC_VOLUME";
    inline constexpr const char* PERC_HARMONIC = "PERC_HARMONIC";

    inline constexpr const char* MICRO_DRIFT = "MICRO_DRIFT";
    inline constexpr const char* MICRO_CLICK = "MICRO_CLICK";
    inline constexpr const char* MICRO_BLEED = "MICRO_BLEED";

    inline constexpr const char* AMP_TYPE     = "AMP_TYPE";
    inline constexpr const char* ROTARY_SPEED = "ROTARY_SPEED";

    inline constexpr const char* MASTER_VOLUME = "MASTER_VOLUME";
    inline constexpr const char* OVERDRIVE     = "OVERDRIVE";
    inline constexpr const char* CHORUS        = "CHORUS";
    inline constexpr const char* PHASER        = "PHASER";
    inline constexpr const char* DELAY         = "DELAY";
    inline constexpr const char* REVERB        = "REVERB";
    inline constexpr const char* WAH           = "WAH";

    inline constexpr const char* FX_OVERDRIVE_ON = "FX_OVERDRIVE_ON";
    inline constexpr const char* FX_CHORUS_ON    = "FX_CHORUS_ON";
    inline constexpr const char* FX_PHASER_ON    = "FX_PHASER_ON";
    inline constexpr const char* FX_DELAY_ON     = "FX_DELAY_ON";
    inline constexpr const char* FX_REVERB_ON    = "FX_REVERB_ON";
    inline constexpr const char* FX_WAH_ON       = "FX_WAH_ON";

    inline constexpr const char* DELAY_TIME     = "DELAY_TIME";
    inline constexpr const char* DELAY_FEEDBACK = "DELAY_FEEDBACK";
    inline constexpr const char* REVERB_SIZE    = "REVERB_SIZE";

    inline constexpr const char* AMP_DRIVE   = "AMP_DRIVE";
    inline constexpr const char* AMP_BASS    = "AMP_BASS";
    inline constexpr const char* AMP_TREBLE  = "AMP_TREBLE";
    inline constexpr const char* AMP_CUT     = "AMP_CUT";
    inline constexpr const char* ROTARY_BAL  = "ROTARY_BAL";

    inline constexpr const char* EQ_ON     = "EQ_ON";
    inline constexpr const char* EQ_GAIN_1 = "EQ_GAIN_1";
    inline constexpr const char* EQ_GAIN_2 = "EQ_GAIN_2";
    inline constexpr const char* EQ_GAIN_3 = "EQ_GAIN_3";
    inline constexpr const char* EQ_GAIN_4 = "EQ_GAIN_4";
    inline constexpr const char* EQ_GAIN_5 = "EQ_GAIN_5";

    inline constexpr const char* MAINS_TOGGLE = "MAINS_TOGGLE";
    inline constexpr const char* MIDI_MODE    = "MIDI_MODE";
    inline constexpr const char* SPLIT_POINT  = "SPLIT_POINT";

    inline constexpr const char* OSC_DETUNE_1  = "OSC_DETUNE_1";
    inline constexpr const char* OSC_DETUNE_2  = "OSC_DETUNE_2";
    inline constexpr const char* OSC_DETUNE_3  = "OSC_DETUNE_3";
    inline constexpr const char* OSC_DETUNE_4  = "OSC_DETUNE_4";
    inline constexpr const char* OSC_DETUNE_5  = "OSC_DETUNE_5";
    inline constexpr const char* OSC_DETUNE_6  = "OSC_DETUNE_6";
    inline constexpr const char* OSC_DETUNE_7  = "OSC_DETUNE_7";
    inline constexpr const char* OSC_DETUNE_8  = "OSC_DETUNE_8";
    inline constexpr const char* OSC_DETUNE_9  = "OSC_DETUNE_9";
    inline constexpr const char* OSC_DETUNE_10 = "OSC_DETUNE_10";
    inline constexpr const char* OSC_DETUNE_11 = "OSC_DETUNE_11";
    inline constexpr const char* OSC_DETUNE_12 = "OSC_DETUNE_12";
}

namespace EngineMode
{
    enum : int { VoxV301E = 0, VoxSuper = 1, JenningsJ70 = 2, Extended = 3 };
}

namespace ChassisSkin
{
    enum : int { Default = 0, Crimson = 1, Nosmirc = 2, CitrusFrost = 3, Junkyard = 4 };
}

namespace DrawbarUiStyle
{
    enum : int { VintageMetal = 0, KorgLed = 1 };
}

namespace PercVolume
{
    enum : int { Soft = 0, Normal = 1 };
}

namespace PercHarmonic
{
    enum : int { Foot4 = 0, Foot2_2_3 = 1, Foot2 = 2 };
}

namespace AmpType
{
    enum : int { Direct = 0, AC30Cabinet = 1, LeslieRotary = 2 };
}

namespace RotarySpeed
{
    enum : int { Stop = 0, Slow = 1, Fast = 2 };
}

namespace MidiMode
{
    enum : int { OmniUpper = 0, Split = 1, ThreeChannel = 2 };
}

inline constexpr const char* kUpperDrawbarIds[8] = {
    ParamIDs::DRAWBAR_UPPER_1, ParamIDs::DRAWBAR_UPPER_2, ParamIDs::DRAWBAR_UPPER_3,
    ParamIDs::DRAWBAR_UPPER_4, ParamIDs::DRAWBAR_UPPER_5, ParamIDs::DRAWBAR_UPPER_6,
    ParamIDs::DRAWBAR_UPPER_7, ParamIDs::DRAWBAR_UPPER_8
};

inline constexpr const char* kLowerDrawbarIds[4] = {
    ParamIDs::DRAWBAR_LOWER_1, ParamIDs::DRAWBAR_LOWER_2,
    ParamIDs::DRAWBAR_LOWER_3, ParamIDs::DRAWBAR_LOWER_4
};

inline constexpr const char* kOscDetuneIds[12] = {
    ParamIDs::OSC_DETUNE_1,  ParamIDs::OSC_DETUNE_2,  ParamIDs::OSC_DETUNE_3,
    ParamIDs::OSC_DETUNE_4,  ParamIDs::OSC_DETUNE_5,  ParamIDs::OSC_DETUNE_6,
    ParamIDs::OSC_DETUNE_7,  ParamIDs::OSC_DETUNE_8,  ParamIDs::OSC_DETUNE_9,
    ParamIDs::OSC_DETUNE_10, ParamIDs::OSC_DETUNE_11, ParamIDs::OSC_DETUNE_12
};

inline constexpr const char* kEqGainIds[5] = {
    ParamIDs::EQ_GAIN_1, ParamIDs::EQ_GAIN_2, ParamIDs::EQ_GAIN_3,
    ParamIDs::EQ_GAIN_4, ParamIDs::EQ_GAIN_5
};

inline constexpr const char* kPitchClassNames[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

inline constexpr const char* kUpperDrawbarLabels[8] = {
    "16'", "8'", "5-1/3'", "4'", "2-2/3'", "2'", "1-3/5'", "1'"
};

inline constexpr const char* kLowerDrawbarLabels[4] = {
    "16'", "8'", "4'", "IV"
};

// Semitone offsets of each upper rank from the 8' key fundamental.
inline constexpr int kUpperRankOffsets[8] = { -12, 0, 7, 12, 19, 24, 28, 36 };

// Mixture IV = 2-2/3' + 2' + 1-3/5' + 1'
inline constexpr int kMixtureOffsets[4] = { 19, 24, 28, 36 };

inline constexpr int kLowerRankOffsets[4] = { -12, 0, 12, 0 }; // IV handled separately

inline constexpr double kTopOctaveHz[12] = {
    1046.502, 1108.731, 1174.659, 1244.508,
    1318.510, 1396.913, 1479.978, 1567.982,
    1661.219, 1760.000, 1864.655, 1975.533
};

inline constexpr int kLowestKey = 21;   // A0
inline constexpr int kHighestKey = 108; // C8
inline constexpr int kNumKeys = 88;

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
