#pragma once

#include "BaseEffect.hpp"

template<size_t NUM_LEDS, size_t MIC_PIN>
class SimpleEffect: public BaseEffect {
public:
    SimpleEffect() : BaseEffect() {
    }

    void loop() override {
        nblend(_controller->leds()[0], CHSV(_sample, 255, _sample), 128);
        for(int i=NUM_LEDS-1; i>0; i--) {
            _controller->leds()[i] = _controller->leds()[i-1];
        }
    }

    void soundmems() override {
        _sample = analogRead(MIC_PIN);
        _sample = NORMALIZE(_sample);
        _sample = abs(_sample - 512);
    }

private:
    int _sample = 0;
};