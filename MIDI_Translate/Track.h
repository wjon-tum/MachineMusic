//
// Created by jonas on 28.03.25.
//

#ifndef TRACK_H
#define TRACK_H
#include <memory>
#include <vector>

#include "Event.h"
#include "Tone.h"


class Track {
public:
    std::vector<Tone> tones;
    std::vector<Event> events;
    int ticksPerQuarter;

    void addTone(const Tone &tone);

    void addEvent(const Event &event);
};


#endif //TRACK_H
