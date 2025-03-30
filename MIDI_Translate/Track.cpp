//
// Created by jonas on 28.03.25.
//

#include "Track.h"

void Track::addTone(const Tone &tone) {
    tones.push_back(tone);
}

void Track::addEvent(const Event &event) {
    events.push_back(event);
}