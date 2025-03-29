//
// Created by jonas on 28.03.25.
//

#ifndef EVENT_H
#define EVENT_H
#include <memory>
#include <vector>


class Event {
public:
    long deltaTime;
    std::vector<int> dataBytes;

    virtual ~Event() = default;
    virtual std::shared_ptr<Event> clone() const = 0;
};



#endif //EVENT_H
