#pragma once

#include "BaseEffect.hpp"

template<size_t MIC_PIN>
class PaletteEffect: public BaseEffect {
public:
    PaletteEffect(uint8_t palette_timeout = 5, uint8_t blend_timeout = 100): BaseEffect() {
        _currentBlending = LINEARBLEND;
        _palette_timeout = palette_timeout;
        _blend_timeout = blend_timeout;
    }

    void loop() override {
        EVERY_N_SECONDS(_palette_timeout) {
            for (int i = 0; i < 16; i++) {
                _targetPalette[i] = CHSV(random8(), 255, 255);
            }
        }

        EVERY_N_MILLISECONDS(_blend_timeout) {
            uint8_t maxChanges = 24; 
            nblendPaletteTowardPalette(_currentPalette, _targetPalette, maxChanges);
        }
    }

    void soundmems() override {
        int n = analogRead(MIC_PIN);
        
        n = NORMALIZE(n);
        n = abs(n-512);
        
        CRGB newcolour = ColorFromPalette(_currentPalette, constrain(n,0,255), constrain(n,0,255), _currentBlending);
        nblend(_controller->leds()[0], newcolour, 128);
    
        for (int i = _controller->size()-1; i>0; i--) {
            _controller->leds()[i] = _controller->leds()[i-1];
        }  
    } 

private:
    CRGBPalette16 _currentPalette;
    CRGBPalette16 _targetPalette;
    TBlendType    _currentBlending;
    uint8_t       _palette_timeout;
    uint8_t       _blend_timeout;
};