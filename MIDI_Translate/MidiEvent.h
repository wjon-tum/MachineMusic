//
// Created by jonas on 28.03.25.
//

#ifndef MIDIEVENT_H
#define MIDIEVENT_H
#include <vector>

#include "Event.h"


class MidiEvent : public Event {
public:
    int statusByte;

    MidiEvent(const long deltaTime, const int statusByte, const std::vector<int>& dataBytes) {
        this->deltaTime = deltaTime;
        this->statusByte = statusByte;
        this->dataBytes = dataBytes;
    }
    std::shared_ptr<Event> clone() const override {
        return std::make_shared<MidiEvent>(*this);
    }
};


#endif //MIDIEVENT_H
