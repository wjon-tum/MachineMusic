cmake_minimum_required(VERSION 3.30)
project(MachineMusic)

set(CMAKE_CXX_STANDARD 14)

include_directories(MIDI_Translate)

add_executable(MachineMusic
    MIDI_Translate/Event.cpp
    MIDI_Translate/Event.h
    MIDI_Translate/main.cpp
    MIDI_Translate/MetaEvent.cpp
    MIDI_Translate/MetaEvent.h
    MIDI_Translate/MidiEvent.cpp
    MIDI_Translate/MidiEvent.h
    MIDI_Translate/Tone.cpp
    MIDI_Translate/Tone.h
    MIDI_Translate/Track.cpp
    MIDI_Translate/Track.h)
