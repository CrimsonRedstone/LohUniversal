cmake_minimum_required(VERSION 3.22)

# JUCE 9 requires C (not just CXX) — juceaide / some helpers are C.
project(LohUniversal VERSION 1.0.0 LANGUAGES C CXX)

# UTF-8 source + string literals on every MSVC target (including vst3_helper).
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS   OFF CACHE BOOL "" FORCE)

option(LOHUNIVERSAL_BUILD_CLAP "Build a CLAP wrapper via clap-juce-extensions" ON)

# ---------------------------------------------------------------------------
# JUCE 9.0.1 — use a local copy if present, otherwise fetch it
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
else()
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        9.0.1
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# ---------------------------------------------------------------------------
# clap-juce-extensions — optional CLAP wrapper around the JUCE plugin target
# ---------------------------------------------------------------------------
if(LOHUNIVERSAL_BUILD_CLAP)
    set(CLAP_JUCE_EXTENSIONS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(clap-juce-extensions
        GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
        GIT_TAG        main
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(clap-juce-extensions)
endif()

# ---------------------------------------------------------------------------
# Formats: AU is macOS-only. CLAP is added by clap_juce_extensions_plugin.
# ---------------------------------------------------------------------------
if(APPLE)
    set(LOHUNIVERSAL_FORMATS VST3 AU Standalone)
else()
    set(LOHUNIVERSAL_FORMATS VST3 Standalone)
endif()

juce_add_plugin(LohUniversal
    COMPANY_NAME                "Crimson Redstone"
    COMPANY_WEBSITE             "https://crimsonredstone.bandcamp.com/"
    BUNDLE_ID                   com.crimsonredstone.lohuniversal
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     TRUE
    PLUGIN_MANUFACTURER_CODE    CrRd
    PLUGIN_CODE                 LohU
    FORMATS                     ${LOHUNIVERSAL_FORMATS}
    PRODUCT_NAME                "Loh Universal"
    DESCRIPTION                 "Transistor combo organ"
    ICON_BIG                    "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Icon/icon-1024.png"
    ICON_SMALL                  "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Icon/icon-128.png"
    VST3_CATEGORIES             "Instrument|Synth"
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    MICROPHONE_PERMISSION_ENABLED FALSE)

if(LOHUNIVERSAL_BUILD_CLAP)
    clap_juce_extensions_plugin(
        TARGET        LohUniversal
        CLAP_ID       "com.crimsonredstone.lohuniversal"
        CLAP_FEATURES instrument synthesizer organ)
endif()

target_sources(LohUniversal PRIVATE
    Source/Main.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters/ParameterLayout.cpp
    Source/DSP/TopOctaveOscillator.cpp
    Source/DSP/FrequencyDivider.cpp
    Source/DSP/VoicingFilters.cpp
    Source/DSP/VibratoLFO.cpp
    Source/DSP/PercussionEngine.cpp
    Source/DSP/KeyNoiseGenerator.cpp
    Source/DSP/OrganVoiceManager.cpp
    Source/FX/LeslieRotarySpeaker.cpp
    Source/GUI/LookAndFeel/VintageVoxLookAndFeel.cpp
    Source/GUI/Components/DrawbarComponent.cpp
    Source/GUI/Components/ReverseKeybedComponent.cpp
    Source/GUI/Components/RockerSwitchComponent.cpp
    Source/GUI/Components/TrimPotComponent.cpp
    Source/GUI/Views/MainPanelView.cpp
    Source/GUI/Views/OpenLidView.cpp
    Source/GUI/Views/FXRackView.cpp
    Source/GUI/Views/EQView.cpp)

juce_add_binary_data(LohUniversalFonts
    HEADER_NAME BinaryData.h
    NAMESPACE BinaryData
    SOURCES
        Assets/Fonts/InterRegular.ttf
        Assets/Fonts/InterBold.ttf
        Assets/Fonts/InterDisplayBold.ttf
        Assets/Fonts/GreatVibesRegular.ttf
        Assets/Icon/LohUniversalIcon.png)

target_include_directories(LohUniversal PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_compile_definitions(LohUniversal PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

if(WIN32)
    # JUCE 9 bundles ASIO headers. JUCE_ASIO=1 is enough for standalone.
    target_compile_definitions(LohUniversal PUBLIC
        JUCE_ASIO=1
        JUCE_WASAPI=1
        JUCE_DIRECTSOUND=1)
endif()

target_link_libraries(LohUniversal PRIVATE
    LohUniversalFonts
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

if(MSVC)
    target_compile_options(LohUniversal PRIVATE /W4 /permissive- /wd4996 /utf-8 /bigobj)
else()
    target_compile_options(LohUniversal PRIVATE -Wall -Wextra -Wpedantic)
endif()
