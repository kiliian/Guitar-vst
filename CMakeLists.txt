cmake_minimum_required(VERSION 3.18)
project(SWAMStyleGuitarPro VERSION 1.3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(JUCE CONFIG REQUIRED) # -DJUCE_DIR=/path/to/JUCE/cmake

juce_add_plugin(SWAMStyleGuitarPro
    COMPANY_NAME "OpenStudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE OSio
    PLUGIN_CODE SwG3
    FORMATS VST3 Standalone
    PRODUCT_NAME "SWAM-Style Electric Guitar Pro"
)

juce_generate_juce_header(SWAMStyleGuitarPro)

set(SRC
    src/Main.cpp
    src/ChordEngine.h src/ChordEngine.cpp
    src/PatternLibrary.h src/PatternLibrary.cpp
    src/RhythmEngine.h src/RhythmEngine.cpp
    src/KeySwitch.h src/KeySwitch.cpp
    src/MidiExporter.h src/MidiExporter.cpp
    src/Editor.h src/Editor.cpp
    src/FavoritesStore.h src/FavoritesStore.cpp
)

target_sources(SWAMStyleGuitarPro PRIVATE ${SRC})
target_compile_definitions(SWAMStyleGuitarPro PRIVATE JUCE_USE_MPESYNTH=1)

if (MSVC)
  target_compile_options(SWAMStyleGuitarPro PRIVATE /W4)
else()
  target_compile_options(SWAMStyleGuitarPro PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(SWAMStyleGuitarPro PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_basics
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
)
