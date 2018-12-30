#pragma once

#include <FastLED.h>
#include "BaseEffect.hpp"

template<size_t NUM_LEDS, size_t MIC_PIN, size_t NSAMPLES>
class NoiseEffect: public BaseEffect {
public:
    NoiseEffect(uint8_t palette_timeout = 5, uint8_t blend_timeout = 10) : BaseEffect() {
        
        for (uint8_t i=0; i<NSAMPLES; i++)
            _samplearray[i]=0;

        _palette_timeout = palette_timeout;
        _blend_timeout = blend_timeout;
        _currentPalette = OceanColors_p;
        _targetPalette = LavaColors_p;
    }
    
    void loop() override {
        
        EVERY_N_MILLISECONDS(_blend_timeout) {
            nblendPaletteTowardPalette(_currentPalette, _targetPalette);   // AWESOME palette blending capability.
            fillnoise8();                                             // Update the LED array with noise based on sound input
            fadeToBlackBy(_controller->leds(), NUM_LEDS, 1);                         // 8 bit, 1 = slow, 255 = fast
            sndwave();                                                // Move the pixels to the left/right.
        }
        
        EVERY_N_SECONDS(_palette_timeout) {                                        // Change the target palette to a random one every 5 seconds.
            _targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), 
                                          CHSV(random8(), 255, random8(128,255)), 
                                          CHSV(random8(), 192, random8(128,255)), 
                                          CHSV(random8(), 255, random8(128,255)));
        }
        
    }

    void soundmems() override {
        int tmp = analogRead(MIC_PIN);
        tmp = NORMALIZE(tmp);
        
        _sample = abs(tmp-512);
        _samplesum += _sample - _samplearray[_samplecount];  // Add the new sample and remove the oldest sample in the array 
        _sampleavg = _samplesum / NSAMPLES;                  // Get an average
        _samplearray[_samplecount] = _sample;                // Update oldest sample in the array with new sample
        _samplecount = (_samplecount + 1) % NSAMPLES;        // Update the counter for the array
    }

private:
    void fillnoise8() {  // Add Perlin noise with modifiers from the soundmems routine.

        if (_sampleavg > NUM_LEDS) _sampleavg = NUM_LEDS;

        for (uint8_t i= (NUM_LEDS - _sampleavg)/2; i<(NUM_LEDS + _sampleavg)/2; i++) {  // The louder the sound, the wider the soundbar.            
            uint8_t index = inoise8(i*_sampleavg+_xdist, _ydist+i*_sampleavg);          // Get a value from the noise function. I'm using both x and y axis.
            _controller->leds()[i] = ColorFromPalette(_currentPalette, index, _sampleavg, LINEARBLEND);  // With that value, look up the 8 bit colour palette value and assign it to the current LED.
        }

        _xdist=_xdist+beatsin8(5,0,3);
        _ydist=_ydist+beatsin8(4,0,3);                                                                            
    } // fillnoise8()

    void sndwave() {                                              // Shifting pixels from the center to the left and right.
        for (uint8_t i = NUM_LEDS - 1; i > NUM_LEDS/2; i--) {           // Move to the right , and let the fade do the rest.
            _controller->leds()[i] = _controller->leds()[i - 1];
        }

        for (uint8_t i = 0; i < NUM_LEDS/2; i++) {                      // Move to the left, and let the fade to the rest.
            _controller->leds()[i] = _controller->leds()[i + 1];
        }  
    } // sndwave()

    CRGBPalette16 _currentPalette;
    CRGBPalette16 _targetPalette;
    TBlendType    _currentBlending;                          // NOBLEND or LINEARBLEND 

    unsigned int _samplearray[NSAMPLES];
    unsigned int _samplesum = 0;
    unsigned int _sampleavg = 0;
    unsigned int _samplecount = 0;
    unsigned int _sample = 0;

    int16_t _xdist;      // A random number for our noise generator.
    int16_t _ydist;
    uint16_t _xscale = 30;      // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
    uint16_t _yscale = 30;      // Wouldn't recommend changing this on the fly, or the animation will be really blocky.

    uint8_t _palette_timeout;
    uint8_t _blend_timeout;
};