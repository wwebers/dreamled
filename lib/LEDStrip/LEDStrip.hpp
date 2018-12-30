#pragma once

#include <FastLED.h>

#include "Effects/BaseEffect.hpp"
#include "Effects/PaletteEffect.hpp"
#include "Effects/BraceletEffect.hpp"
#include "Effects/WaveEffect.hpp"
#include "Effects/NoiseEffect.hpp"
#include "Effects/SimpleEffect.hpp"

template<size_t DELAY>
class LEDStrip {
public:
    // The class will be constructed with an array of effects
    LEDStrip(BaseEffect** effects, uint8_t numEffects) : _effects(effects), _numEffects(numEffects) {};

    void begin(CLEDController* controller, CFastLED fastLed) {
        _fastLed = fastLed;
        _controller = controller;
        _fastLed.setMaxPowerInVoltsAndMilliamps(5, 500);

        for(uint8_t i = 0; i < _numEffects; i++) {
            _effects[i]->begin(_controller);
        }
    }

    void begin(CLEDController* controller) {
        begin(controller, FastLED);
    }

    // Main loop controlling the inner loop of the current effect
    void loop() {
        _effects[_currentEffect]->loop();

        EVERY_N_MILLISECONDS(DELAY) {
            _effects[_currentEffect]->soundmems();
            _fastLed.show();
        }
    }

    // Switch to another effect, if we have not already activated it
    void switchTo(uint8_t n) {
        if(n != _currentEffect) {
            _fastLed.showColor(CRGB::Black);
            n = (n >= _numEffects) ? _numEffects : n;
            _currentEffect = n;
        }
    }

private:
    CLEDController* _controller;
    CFastLED _fastLed;
    BaseEffect** _effects;
    uint8_t _numEffects;
    uint8_t _currentEffect = 0;
};