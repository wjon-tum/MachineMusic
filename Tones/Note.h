#include "Arduino.h"

#ifndef NOTE_H
#define NOTE_H

class Note {
public:
    byte channel;
    byte velocity;
    float frequency;
    int deltaTime;

    Note(const float frequency, const int deltaTime, const byte velocity) {
        this->channel = 0;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }
    Note(const byte channel, const float frequency, const int deltaTime, const byte velocity) {
        this->channel = channel;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }

    String toString();
};

#endif