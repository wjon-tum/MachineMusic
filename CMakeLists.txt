cmake_minimum_required(VERSION 3.30)
project(MachineMusic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O0 -g -fsanitize=address,undefined")

include_directories(MIDI_Translate)

add_executable(MachineMusic
        MIDI_Translate/Event.cpp
        MIDI_Translate/Event.h
        MIDI_Translate/main.cpp
        MIDI_Translate/Tone.cpp
        MIDI_Translate/Tone.h
        MIDI_Translate/Track.cpp
        MIDI_Translate/Track.h)
