//
// Created by jonas on 28.03.25.
//

#ifndef TONE_H
#define TONE_H


class Tone {
public:
    bool on;
    int channel;
    double frequency;
    int deltaTime;
    int velocity;

    Tone(const bool on, const double frequency, const int deltaTime, const int velocity) {
        this->on = on;
        this->channel = 0;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }
    Tone(const bool on, const int channel, const double frequency, const int deltaTime, const int velocity) {
        this->on = on;
        this->channel = channel;
        this->frequency = frequency;
        this->deltaTime = deltaTime;
        this->velocity = velocity;
    }
};


#endif //TONE_H
