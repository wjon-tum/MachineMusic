//
// Created by jonas on 28.03.25.
//

#ifndef METAEVENT_H
#define METAEVENT_H
#include <vector>

#include "Event.h"


class MetaEvent final : public Event {
public:
    int typeByte;
    int length;

    MetaEvent(const long deltaTime, const int typeByte, const int length, const std::vector<int> &dataBytes) {
        this->deltaTime = deltaTime;
        this->typeByte = typeByte;
        this->length = length;
        this->dataBytes = dataBytes;
    }

    std::shared_ptr<Event> clone() const override {
        return std::make_shared<MetaEvent>(*this);
    }
};


#endif //METAEVENT_H
