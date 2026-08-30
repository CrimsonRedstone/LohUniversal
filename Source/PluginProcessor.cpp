#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/FactoryPresets.h"
#include "Utf8.h"
#include <cmath>

namespace
{
    inline float load(std::atomic<float>* p, float fallback) noexcept
    {
        return p != nullptr ? p->load(std::memory_order_relaxed) : fallback;
    }
}

LohUniversalAudioProcessor::LohUniversalAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "LOHUNIVERSAL", createParameterLayout())
{
    cacheParameterPointers();
}

void LohUniversalAudioProcessor::cacheParameterPointers() noexcept
{
    for (int i = 0; i < 8; ++i)
        pUpper[i] = apvts.getRawParameterValue(kUpperDrawbarIds[i]);
    for (int i = 0; i < 4; ++i)
        pLower[i] = apvts.getRawParameterValue(kLowerDrawbarIds[i]);
    for (int i = 0; i < 12; ++i)
        pDetune[i] = apvts.getRawParameterValue(kOscDetuneIds[i]);

    pMixture      = apvts.getRawParameterValue(ParamIDs::DRAWBAR_MIXTURE_IV);
    pFlute        = apvts.getRawParameterValue(ParamIDs::TONE_FLUTE);
    pReed         = apvts.getRawParameterValue(ParamIDs::TONE_REED);
    pEngine       = apvts.getRawParameterValue(ParamIDs::ENGINE_MODE);
    pSkin         = apvts.getRawParameterValue(ParamIDs::CHASSIS_SKIN);
    pDrawbarStyle = apvts.getRawParameterValue(ParamIDs::DRAWBAR_UI_STYLE);
    pVibOn        = apvts.getRawParameterValue(ParamIDs::VIBRATO_TOGGLE);
    pVibSpeed     = apvts.getRawParameterValue(ParamIDs::VIBRATO_SPEED);
    pVibDepth     = apvts.getRawParameterValue(ParamIDs::VIBRATO_DEPTH);
    pPercOn       = apvts.getRawParameterValue(ParamIDs::PERC_TOGGLE);
    pPercDecay    = apvts.getRawParameterValue(ParamIDs::PERC_DECAY);
    pPercVol      = apvts.getRawParameterValue(ParamIDs::PERC_VOLUME);
    pPercHarm     = apvts.getRawParameterValue(ParamIDs::PERC_HARMONIC);
    pDrift        = apvts.getRawParameterValue(ParamIDs::MICRO_DRIFT);
    pClick        = apvts.getRawParameterValue(ParamIDs::MICRO_CLICK);
    pBleed        = apvts.getRawParameterValue(ParamIDs::MICRO_BLEED);
    pAmp          = apvts.getRawParameterValue(ParamIDs::AMP_TYPE);
    pRotary       = apvts.getRawParameterValue(ParamIDs::ROTARY_SPEED);
    pMaster       = apvts.getRawParameterValue(ParamIDs::MASTER_VOLUME);
    pOverdrive    = apvts.getRawParameterValue(ParamIDs::OVERDRIVE);
    pChorus       = apvts.getRawParameterValue(ParamIDs::CHORUS);
    pPhaser       = apvts.getRawParameterValue(ParamIDs::PHASER);
    pDelay        = apvts.getRawParameterValue(ParamIDs::DELAY);
    pReverb       = apvts.getRawParameterValue(ParamIDs::REVERB);
    pWah          = apvts.getRawParameterValue(ParamIDs::WAH);
    pOdOn         = apvts.getRawParameterValue(ParamIDs::FX_OVERDRIVE_ON);
    pChOn         = apvts.getRawParameterValue(ParamIDs::FX_CHORUS_ON);
    pPhOn         = apvts.getRawParameterValue(ParamIDs::FX_PHASER_ON);
    pDlOn         = apvts.getRawParameterValue(ParamIDs::FX_DELAY_ON);
    pRvOn         = apvts.getRawParameterValue(ParamIDs::FX_REVERB_ON);
    pWhOn         = apvts.getRawParameterValue(ParamIDs::FX_WAH_ON);
    pDelayTime    = apvts.getRawParameterValue(ParamIDs::DELAY_TIME);
    pDelayFb      = apvts.getRawParameterValue(ParamIDs::DELAY_FEEDBACK);
    pRevSize      = apvts.getRawParameterValue(ParamIDs::REVERB_SIZE);
    pAmpDrive     = apvts.getRawParameterValue(ParamIDs::AMP_DRIVE);
    pAmpBass      = apvts.getRawParameterValue(ParamIDs::AMP_BASS);
    pAmpTreble    = apvts.getRawParameterValue(ParamIDs::AMP_TREBLE);
    pAmpCut       = apvts.getRawParameterValue(ParamIDs::AMP_CUT);
    pRotaryBal    = apvts.getRawParameterValue(ParamIDs::ROTARY_BAL);
    pEqOn         = apvts.getRawParameterValue(ParamIDs::EQ_ON);
    for (int i = 0; i < 5; ++i)
        pEqGain[i] = apvts.getRawParameterValue(kEqGainIds[i]);
    pMains        = apvts.getRawParameterValue(ParamIDs::MAINS_TOGGLE);
    pMidiMode     = apvts.getRawParameterValue(ParamIDs::MIDI_MODE);
    pSplit        = apvts.getRawParameterValue(ParamIDs::SPLIT_POINT);
}

void LohUniversalAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    voices.prepare(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(juce::jmax(1, samplesPerBlock));
    spec.numChannels = 2;

    overdrive.prepare(spec);
    wah.prepare(spec);
    chorusPhaser.prepare(spec);
    delayReverb.prepare(spec);
    ac30.prepare(spec);
    leslie.prepare(spec);
    graphicEq.prepare(spec);

    scratchCapacity = juce::jmax(samplesPerBlock * 2, 4096);
    organScratch.setSize(1, scratchCapacity);
    stereoScratch.setSize(2, scratchCapacity);
}

void LohUniversalAudioProcessor::releaseResources()
{
    voices.reset();
    overdrive.reset();
    wah.reset();
    chorusPhaser.reset();
    delayReverb.reset();
    ac30.reset();
    leslie.reset();
    graphicEq.reset();
}

bool LohUniversalAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::disabled())
        return false;
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

int LohUniversalAudioProcessor::getChassisSkin() const noexcept
{
    return juce::roundToInt(load(pSkin, 0.0f));
}

int LohUniversalAudioProcessor::getDrawbarUiStyle() const noexcept
{
    return juce::roundToInt(load(pDrawbarStyle, 0.0f));
}

int LohUniversalAudioProcessor::getNumPrograms()
{
    return kNumFactoryPresets;
}

int LohUniversalAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void LohUniversalAudioProcessor::setCurrentProgram(int index)
{
    if (! juce::isPositiveAndBelow(index, kNumFactoryPresets))
        return;
    currentProgram = index;
    applyFactoryPreset(apvts, index);
}

const juce::String LohUniversalAudioProcessor::getProgramName(int index)
{
    if (juce::isPositiveAndBelow(index, kNumFactoryPresets))
        return kFactoryPresets[index].name;
    return lohBrandFull();
}

void LohUniversalAudioProcessor::handleGuiNoteOn(int manual, int midiNote)
{
    const uint32_t packed = 0x80000000u
                          | (static_cast<uint32_t>(manual & 0xFF) << 16)
                          | (static_cast<uint32_t>(midiNote & 0xFFFF));
    const int w = guiWrite.load(std::memory_order_relaxed);
    const int n = (w + 1) % kGuiQueueSize;
    if (n == guiRead.load(std::memory_order_acquire))
        return;
    guiNoteQueue[static_cast<size_t>(w)].store(packed, std::memory_order_relaxed);
    guiWrite.store(n, std::memory_order_release);
}

void LohUniversalAudioProcessor::handleGuiNoteOff(int manual, int midiNote)
{
    const uint32_t packed = (static_cast<uint32_t>(manual & 0xFF) << 16)
                          | (static_cast<uint32_t>(midiNote & 0xFFFF));
    const int w = guiWrite.load(std::memory_order_relaxed);
    const int n = (w + 1) % kGuiQueueSize;
    if (n == guiRead.load(std::memory_order_acquire))
        return;
    guiNoteQueue[static_cast<size_t>(w)].store(packed, std::memory_order_relaxed);
    guiWrite.store(n, std::memory_order_release);
}

int LohUniversalAudioProcessor::manualForMessage(int midiChannel, int midiNote) const noexcept
{
    const int mode = juce::roundToInt(load(pMidiMode, 0.0f));
    const int split = juce::roundToInt(load(pSplit, 60.0f));

    if (mode == MidiMode::ThreeChannel)
    {
        if (midiChannel == 2) return OrganVoiceManager::Lower;
        if (midiChannel == 3) return OrganVoiceManager::Bass;
        return OrganVoiceManager::Upper;
    }

    if (mode == MidiMode::Split)
    {
        if (midiNote < 36)
            return OrganVoiceManager::Bass;
        return (midiNote < split) ? OrganVoiceManager::Lower : OrganVoiceManager::Upper;
    }

    juce::ignoreUnused(midiChannel);
    return OrganVoiceManager::Upper;
}

void LohUniversalAudioProcessor::applyMidiCc(int cc, int value)
{
    auto setNorm = [this](const char* id, float norm)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, norm));
    };

    const float n = static_cast<float>(value) / 127.0f;

    if (cc >= 12 && cc <= 19)
        setNorm(kUpperDrawbarIds[cc - 12], n);
    else if (cc == 20) setNorm(ParamIDs::DRAWBAR_MIXTURE_IV, n);
    else if (cc == 21) setNorm(ParamIDs::TONE_FLUTE, n);
    else if (cc == 22) setNorm(ParamIDs::TONE_REED, n);
    else if (cc == 1)
    {
        setNorm(ParamIDs::VIBRATO_DEPTH, n);
        if (auto* t = apvts.getParameter(ParamIDs::VIBRATO_TOGGLE))
            t->setValueNotifyingHost(n > 0.02f ? 1.0f : 0.0f);
    }
    else if (cc == 92)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParamIDs::VIBRATO_SPEED)))
        {
            const float hz = juce::jmap(n, 4.0f, 8.5f);
            p->setValueNotifyingHost(p->getNormalisableRange().convertTo0to1(hz));
        }
    }
    else if (cc == 73) setNorm(ParamIDs::MICRO_CLICK, n);
    else if (cc == 7)  setNorm(ParamIDs::MASTER_VOLUME, n);
    else if (cc == 11) setNorm(ParamIDs::MASTER_VOLUME, n);
    else if (cc == 64) setSustain(value >= 64);
}

void LohUniversalAudioProcessor::noteOnInternal(int manual, int midiNote, float velocity) noexcept
{
    if (manual < 0 || manual >= OrganVoiceManager::NumManuals)
        return;
    if (midiNote < 0 || midiNote > 127)
        return;
    latched[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)] = false;
    voices.noteOn(manual, midiNote, velocity);
}

void LohUniversalAudioProcessor::noteOffInternal(int manual, int midiNote) noexcept
{
    if (manual < 0 || manual >= OrganVoiceManager::NumManuals)
        return;
    if (midiNote < 0 || midiNote > 127)
        return;

    if (sustainPedal)
    {
        latched[static_cast<size_t>(manual)][static_cast<size_t>(midiNote)] = true;
        return;
    }

    voices.noteOff(manual, midiNote);
}

void LohUniversalAudioProcessor::setSustain(bool on) noexcept
{
    if (sustainPedal && ! on)
    {
        for (int m = 0; m < OrganVoiceManager::NumManuals; ++m)
        {
            for (int n = 0; n < 128; ++n)
            {
                if (latched[static_cast<size_t>(m)][static_cast<size_t>(n)])
                {
                    latched[static_cast<size_t>(m)][static_cast<size_t>(n)] = false;
                    voices.noteOff(m, n);
                }
            }
        }
    }
    sustainPedal = on;
}

void LohUniversalAudioProcessor::handleMidiMessage(const juce::MidiMessage& m)
{
    if (m.isNoteOn())
    {
        const int manual = manualForMessage(m.getChannel(), m.getNoteNumber());
        noteOnInternal(manual, m.getNoteNumber(), m.getFloatVelocity());
    }
    else if (m.isNoteOff())
    {
        const int manual = manualForMessage(m.getChannel(), m.getNoteNumber());
        noteOffInternal(manual, m.getNoteNumber());
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        sustainPedal = false;
        for (auto& row : latched)
            row.fill(false);
        voices.allNotesOff();
    }
    else if (m.isSustainPedalOn())
    {
        setSustain(true);
    }
    else if (m.isSustainPedalOff())
    {
        setSustain(false);
    }
    else if (m.isController())
    {
        applyMidiCc(m.getControllerNumber(), m.getControllerValue());
    }
}

void LohUniversalAudioProcessor::drainGuiQueue() noexcept
{
    int r = guiRead.load(std::memory_order_relaxed);
    const int w = guiWrite.load(std::memory_order_acquire);
    while (r != w)
    {
        const uint32_t packed = guiNoteQueue[static_cast<size_t>(r)].load(std::memory_order_relaxed);
        const bool on = (packed & 0x80000000u) != 0;
        const int manual = static_cast<int>((packed >> 16) & 0xFF);
        const int note = static_cast<int>(packed & 0xFFFF);
        if (on) noteOnInternal(manual, note, 1.0f);
        else    noteOffInternal(manual, note);
        r = (r + 1) % kGuiQueueSize;
    }
    guiRead.store(r, std::memory_order_release);
}

void LohUniversalAudioProcessor::fillRenderParams(OrganVoiceManager::RenderParams& p) noexcept
{
    for (int i = 0; i < 8; ++i)
        p.upperDrawbar[i] = load(pUpper[i], 0.0f);
    for (int i = 0; i < 4; ++i)
        p.lowerDrawbar[i] = load(pLower[i], 0.0f);
    for (int i = 0; i < 12; ++i)
        p.oscDetune[i] = load(pDetune[i], 0.0f);

    p.mixtureIV     = load(pMixture, 0.0f);
    p.flute         = load(pFlute, 0.7f);
    p.reed          = load(pReed, 0.35f);
    p.engineMode    = juce::roundToInt(load(pEngine, 0.0f));
    p.chassisSkin   = juce::roundToInt(load(pSkin, 0.0f));
    p.vibratoOn     = load(pVibOn, 0.0f) >= 0.5f;
    p.vibratoSpeed  = load(pVibSpeed, 6.5f);
    p.vibratoDepth  = load(pVibDepth, 0.35f);
    p.percOn        = load(pPercOn, 0.0f) >= 0.5f;
    p.percDecayMs   = load(pPercDecay, 280.0f);
    p.percSoft      = juce::roundToInt(load(pPercVol, 1.0f)) == PercVolume::Soft;
    p.percHarmonic  = juce::roundToInt(load(pPercHarm, 0.0f));
    p.microDrift    = load(pDrift, 0.15f);
    p.microClick    = load(pClick, 0.25f);
    p.microBleedDb  = load(pBleed, -62.0f);
    p.mainsOn       = load(pMains, 1.0f) >= 0.5f;
    p.masterVolume  = load(pMaster, 0.72f);
}

void LohUniversalAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    for (int ch = 0; ch < numCh; ++ch)
        buffer.clear(ch, 0, numSamples);

    drainGuiQueue();

    if (numSamples <= 0)
        return;

    if (numSamples > scratchCapacity)
    {
        scratchCapacity = juce::jmax(numSamples * 2, 4096);
        organScratch.setSize(1, scratchCapacity);
        stereoScratch.setSize(2, scratchCapacity);
    }

    const int n = numSamples;

    organScratch.setSize(1, n, false, false, true);
    stereoScratch.setSize(2, n, false, false, true);

    OrganVoiceManager::RenderParams rp;
    fillRenderParams(rp);

    float* dest = organScratch.getWritePointer(0);
    int cursor = 0;

    for (const auto metadata : midi)
    {
        const int pos = juce::jlimit(0, n, metadata.samplePosition);
        if (pos > cursor)
        {
            voices.render(dest + cursor, pos - cursor, rp);
            cursor = pos;
        }
        handleMidiMessage(metadata.getMessage());
    }

    if (cursor < n)
        voices.render(dest + cursor, n - cursor, rp);

    const float* organ = organScratch.getReadPointer(0);
    stereoScratch.copyFrom(0, 0, organ, n);
    stereoScratch.copyFrom(1, 0, organ, n);

    if (load(pOdOn, 0.0f) >= 0.5f)
        overdrive.process(stereoScratch, load(pOverdrive, 0.0f));

    if (load(pWhOn, 0.0f) >= 0.5f)
        wah.process(stereoScratch, load(pWah, 0.0f));

    const float chMix = (load(pChOn, 0.0f) >= 0.5f) ? load(pChorus, 0.0f) : 0.0f;
    const float phMix = (load(pPhOn, 0.0f) >= 0.5f) ? load(pPhaser, 0.0f) : 0.0f;
    chorusPhaser.process(stereoScratch, chMix, phMix);

    const float dMix = (load(pDlOn, 0.0f) >= 0.5f) ? load(pDelay, 0.0f) : 0.0f;
    const float rMix = (load(pRvOn, 0.0f) >= 0.5f) ? load(pReverb, 0.0f) : 0.0f;
    delayReverb.process(stereoScratch, dMix, load(pDelayTime, 280.0f),
                        load(pDelayFb, 0.28f), rMix, load(pRevSize, 0.45f));

    const int amp = juce::roundToInt(load(pAmp, 0.0f));
    if (amp == AmpType::AC30Cabinet || amp == AmpType::LeslieRotary)
        ac30.process(stereoScratch,
                     load(pAmpDrive, 0.35f),
                     load(pAmpBass, 2.0f),
                     load(pAmpTreble, -2.0f),
                     load(pAmpCut, 0.40f));

    leslie.setSpeed(juce::roundToInt(load(pRotary, 1.0f)));
    leslie.setBalance(load(pRotaryBal, 0.45f));
    leslie.setEnabled(amp == AmpType::LeslieRotary);
    leslie.process(stereoScratch);

    if (load(pEqOn, 0.0f) >= 0.5f)
    {
        float gains[5];
        for (int i = 0; i < 5; ++i)
            gains[i] = load(pEqGain[i], 0.0f);
        graphicEq.process(stereoScratch, gains);
    }

    bool blewUp = false;
    const int outCh = juce::jmin(numCh, 2);
    for (int ch = 0; ch < outCh; ++ch)
    {
        const float* src = stereoScratch.getReadPointer(ch);
        float* dst = buffer.getWritePointer(ch);
        for (int i = 0; i < n; ++i)
        {
            float x = src[i];
            if (! std::isfinite(x))
            {
                x = 0.0f;
                blewUp = true;
            }
            else
            {
                x = juce::jlimit(-1.8f, 1.8f, x);
            }
            dst[i] = x;
        }
    }

    if (blewUp)
    {
        overdrive.reset();
        wah.reset();
        chorusPhaser.reset();
        delayReverb.reset();
        ac30.reset();
        leslie.reset();
        graphicEq.reset();
    }
}

void LohUniversalAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void LohUniversalAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

void LohUniversalAudioProcessor::saveUserPreset(const juce::File& file)
{
    auto target = file;
    if (! target.hasFileExtension(".lohpreset"))
        target = target.withFileExtension(".lohpreset");
    target.getParentDirectory().createDirectory();
    if (auto xml = apvts.copyState().createXml())
    {
        xml->setAttribute("lohPreset", 1);
        xml->writeTo(target);
    }
}

bool LohUniversalAudioProcessor::loadUserPreset(const juce::File& file)
{
    if (auto xml = juce::XmlDocument::parse(file))
    {
        auto tree = juce::ValueTree::fromXml(*xml);
        if (tree.isValid())
        {
            apvts.replaceState(tree);
            return true;
        }
    }
    return false;
}

juce::AudioProcessorEditor* LohUniversalAudioProcessor::createEditor()
{
    return new LohUniversalAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LohUniversalAudioProcessor();
}
