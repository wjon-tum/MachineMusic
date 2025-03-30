//
// Created by jonas on 28.03.25.
//

#ifndef EVENT_H
#define EVENT_H
#include <vector>


class Event {
public:
    int deltaTime;
    int typeByte;
    int length{};
    std::vector<int> dataBytes;


    Event(const int deltaTime, const int typeByte, const int length, const std::vector<int> &dataBytes) {
        this->deltaTime = deltaTime;
        this->typeByte = typeByte;
        this->length = length;
        this->dataBytes = dataBytes;
    }

    Event(const int deltaTime, const int typeByte, const std::vector<int> &dataBytes) {
        this->deltaTime = deltaTime;
        this->typeByte = typeByte;
        this->dataBytes = dataBytes;
    }
};



#endif //EVENT_H
