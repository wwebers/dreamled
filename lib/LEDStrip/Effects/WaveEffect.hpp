#pragma once

#include "BaseEffect.hpp"

template<size_t NUM_LEDS, size_t MIC_PIN>
class WaveEffect: public BaseEffect {
public:
    WaveEffect(uint8_t palette_timeout = 5, uint8_t blend_timeout = 100, uint8_t delay = 20): BaseEffect() {
        _palette_timeout = palette_timeout;
        _blend_timeout = blend_timeout;
        _delay = delay;
    }

    void loop() override {
        EVERY_N_SECONDS(_palette_timeout) {                                        // Change the palette every 5 seconds.
            for (int i = 0; i < 16; i++) {
                _targetPalette[i] = CHSV(random8(), 255, 255);
            }
        }

        EVERY_N_MILLISECONDS(_blend_timeout) {                                 // AWESOME palette blending capability once they do change.
            uint8_t maxChanges = 24; 
            nblendPaletteTowardPalette(_currentPalette, _targetPalette, maxChanges);
        }

        EVERY_N_MILLIS_I(thistimer,_delay) {                            // For fun, let's make the animation have a variable rate.
            uint8_t timeval = beatsin8(10,20,50);                   // Use a sinewave for the line below. Could also use peak/beat detection.
            thistimer.setPeriod(timeval);                           // Allows you to change how often this routine runs.
            fadeToBlackBy(_controller->leds(), NUM_LEDS, 16);       // 1 = slow, 255 = fast fade. Depending on the faderate, the LED's further away will fade out.
            sndwave();
        }
    }

    void soundmems() override {
        int val = analogRead(MIC_PIN);

        val = NORMALIZE(val);

        int tmp = val - 512;
        _sample = abs(tmp);
    }

private:
    void sndwave() {
        _controller->leds()[NUM_LEDS/2] = ColorFromPalette(_currentPalette, _sample, _sample*2, _currentBlending); // Put the sample into the center
  
        for (uint8_t i = NUM_LEDS - 1; i > NUM_LEDS/2; i--) {       //move to the left      // Copy to the left, and let the fade do the rest.
            _controller->leds()[i] = _controller->leds()[i - 1];
        }

        for (uint8_t i = 0; i < NUM_LEDS/2; i++) {                  // move to the right    // Copy to the right, and let the fade to the rest.
            _controller->leds()[i] = _controller->leds()[i + 1];
        }
    }

    unsigned int  _sample = 0; 
    CRGBPalette16 _currentPalette;
    CRGBPalette16 _targetPalette;
    TBlendType    _currentBlending; // NOBLEND or LINEARBLEND
    uint8_t       _palette_timeout;
    uint8_t       _blend_timeout;
    uint8_t       _delay;
};