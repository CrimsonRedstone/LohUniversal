# Building Лох Universal

Complete JUCE **9.0.1** plugin. Produces **VST3**, **AU** (macOS), **CLAP**, and **Standalone**.

Inter (Cyrillic + Latin) and Great Vibes are embedded, so `Лох`, `⅓`, `·`, `–`, `±` render on a clean Windows install.

## Requirements

| | |
| --- | --- |
| CMake | 3.22 or newer |
| Compiler | C++17 — Xcode 15+, Visual Studio 2022, GCC 11+, Clang 14+ |
| Git | JUCE 9.0.1 and clap-juce-extensions are fetched on first configure |
| Disk | ~600 MB after JUCE downloads |

Optional local JUCE checkout: place it at `LohUniversal/JUCE` (must contain `CMakeLists.txt`) to skip the download.

## Configure & build

```bash
cd LohUniversal
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```

Disable CLAP (VST3 / AU / Standalone only):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DLOHUNIVERSAL_BUILD_CLAP=OFF
```

### macOS (Universal)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --config Release -j
```

Outputs (typical):

- `build/LohUniversal_artefacts/Release/VST3/Лох Universal.vst3`
- `build/LohUniversal_artefacts/Release/AU/Лох Universal.component`
- `build/LohUniversal_artefacts/Release/Standalone/Лох Universal.app`
- `build/LohUniversal_artefacts/Release/CLAP/Лох Universal.clap`

Copy After Build is on: VST3/AU also land in `~/Library/Audio/Plug-Ins/`.

### Windows (x64)

**Lazy path:** double-click `build.bat`. It configures, compiles, and writes `build.log` (success or fail). Paste that log if anything breaks.

Manual:

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release -j
```

Outputs under `build\LohUniversal_artefacts\Release\`. Copy the `.vst3` bundle into
`C:\Program Files\Common Files\VST3`.

Needs: CMake 3.22+ on PATH, Git on PATH, Visual Studio 2022 with the C++ workload.

If a previous `build.bat` failed **or you are upgrading from JUCE 8**, delete the `build` folder once so CMake fetches JUCE 9.0.1. After that, leave `build\` in place — incremental rebuilds reuse JUCE.

### MSVC notes (Visual Studio 2022)

JUCE's `dsp::IIR::Filter::coefficients` is a reference-counted pointer. Assign the pointer, do not dereference it:

```cpp
f.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, hz, 0.7f);
```

`juce_wchar` lives in `namespace juce` under `/permissive-`. Prefer `int` / `wchar_t` in our own code.

### Linux (x64)

Install headers first (Debian/Ubuntu):

```bash
sudo apt install build-essential cmake git \
    libasound2-dev libfreetype6-dev libx11-dev libxinerama-dev \
    libxrandr-dev libxcursor-dev libxcomposite-dev libgl1-mesa-dev \
    libcurl4-openssl-dev libwebkit2gtk-4.1-dev
```

Then the same `cmake -B build` commands. Copy `.vst3` to `~/.vst3`.

WebKit is unused (`JUCE_WEB_BROWSER=0`) but some JUCE versions still probe it.

## Loading in a DAW

Rescan plug-ins in your host (Ableton, Reaper, Logic, Bitwig, Ardour, FL…).
The plugin is a **MIDI-controlled synthesizer** — no audio input. Route a MIDI
track into it. Standalone opens its own window and speaks to the system MIDI
ports plus the on-screen reverse keybeds.

## MIDI

| | |
| --- | --- |
| Channel 1 | Upper manual |
| Channel 2 | Lower manual |
| Channel 3 | Bass (16' / 8') |
| Omni / Split | Set **MIDI Mode** on the panel |
| CC 12–19 | Upper 16' … 1' |
| CC 20 | Mixture IV |
| CC 21 / 22 | Flute / Reed |
| CC 1 | Vibrato (mod wheel) |
| CC 64 | Sustain |
| CC 7 / 11 | Master volume |
| CC 73 | Key click |
| CC 92 | Vibrato speed |
| QWERTY | Standalone / editor: `A` = C3 (upper). Hold Shift for lower. |

## Skins

Default · Crimson · Nosmirc · Citrus Frost · **Junkyard** (heavy detune, wow/flutter, contact jumps).
