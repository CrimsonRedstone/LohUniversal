Third-party code
================

Do not add the Steinberg ASIO SDK here. Redistributing that SDK is not
allowed under Steinberg's license.

JUCE 9 already bundles ASIO headers. CMake sets JUCE_ASIO=1 on Windows
so the standalone can use ASIO drivers. WASAPI and DirectSound remain
available if ASIO is not.

CLAP and JUCE itself are fetched by CMake (see the project README).
