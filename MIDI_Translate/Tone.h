//
// Created by jonas on 28.03.25.
//

#ifndef TONE_H
#define TONE_H
#include <cstdint>


class Tone {
public:
    int channel;
    float frequency;
    int deltaTime;
    int velocity;

    Tone(const float frequency, const int deltaTime, const int velocity) {
        this->channel = 0;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }
    Tone(const int channel, const float frequency, const int deltaTime, const int velocity) {
        this->channel = channel;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }
};


#endif //TONE_H
