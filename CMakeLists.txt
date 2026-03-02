cmake_minimum_required(VERSION 3.21)

project(HalfMeter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# JUCE is fetched at configure time to keep this repo lightweight.
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

# libebur128 for BS.1770 / EBU R128 loudness + true peak
set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  libebur128
  GIT_REPOSITORY https://github.com/jiixyj/libebur128.git
  GIT_TAG v1.2.6
)
FetchContent_MakeAvailable(libebur128)

juce_add_plugin(HalfMeter
  COMPANY_NAME "TylerA"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  PLUGIN_MANUFACTURER_CODE TylA
  PLUGIN_CODE Hmtr
  FORMATS VST3 Standalone
  PRODUCT_NAME "HalfMeter"
)

target_sources(HalfMeter
  PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
    src/ui/MeterTile.cpp
    src/ui/MeterTile.h
)

target_compile_definitions(HalfMeter
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(HalfMeter
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    ebur128
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_include_directories(HalfMeter
  PRIVATE
    ${libebur128_SOURCE_DIR}/ebur128
)
