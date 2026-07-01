cmake_minimum_required(VERSION 3.22)

project(GrangrandMA VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_SOURCE_DIR "" CACHE PATH "Path to a local JUCE source checkout")

if (JUCE_SOURCE_DIR)
    add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")
else()
    find_package(JUCE CONFIG QUIET)
endif()

if (NOT JUCE_SOURCE_DIR AND NOT JUCE_FOUND)
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.12
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(GrangrandMA
    COMPANY_NAME "Yifeng Yuan"
    BUNDLE_ID "com.yifengyuan.grangrandma"
    PLUGIN_MANUFACTURER_CODE YfYu
    PLUGIN_CODE GrMa
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "GrangrandMA"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "GrangrandMA needs microphone access to granulate your voice."
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(GrangrandMA)

target_sources(GrangrandMA PRIVATE
    Source/GranularEngine.cpp
    Source/GranularEngine.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h)

target_sources(GrangrandMA_Standalone PRIVATE
    Source/StandaloneApp.cpp)

target_compile_definitions(GrangrandMA_Standalone PRIVATE
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1)

target_compile_definitions(GrangrandMA PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(GrangrandMA PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
