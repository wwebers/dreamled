#pragma once

#include "BaseEffect.hpp"

template<size_t NUM_LEDS, size_t MIC_PIN, size_t SAMPLES>
class BraceletEffect: public BaseEffect {
public:
    BraceletEffect(uint8_t noise = 30, uint8_t peak_fall = 10): BaseEffect() {
        _NOISE = noise;
        _PEAK_FALL = peak_fall;
    };

    void loop() override {
        uint8_t  i;
        uint16_t minLvl, maxLvl;
        int      n, height;
        
        n = analogRead(MIC_PIN);
        n = NORMALIZE(n);
        n = abs(n - 512);                               // Center on zero

        n = (n <= _NOISE) ? 0 : (n - _NOISE);                         // Remove noise/hum
        _lvl = ((_lvl * 7) + n) >> 3;                                 // "Dampened" reading (else looks twitchy)
        
        // Calculate bar height based on dynamic min/max levels (fixed point):
        height = _TOP * (_lvl - _minLvlAvg) / (long)(_maxLvlAvg - _minLvlAvg);
        
        if (height < 0L)       height = 0;                          // Clip output
        else if (height > _TOP) height = _TOP;
        if (height > _peak)     _peak   = height;                     // Keep 'peak' dot at top
        
        
        // Color pixels based on rainbow gradient
        for (i=0; i<NUM_LEDS; i++) {
            if (i >= height)   _controller->leds()[i].setRGB( 0, 0, 0);
            else _controller->leds()[i] = CHSV(map(i,0,NUM_LEDS-1,30,150), 255, 255);
        }
        
        // Draw peak dot  
        if (_peak > 0 && _peak <= NUM_LEDS-1) 
            _controller->leds()[_peak] = CHSV(map(_peak,0,NUM_LEDS-1,30,150), 255, 255);

        // Every few frames, make the peak pixel drop by 1:
        
        if (++_dotCount >= _PEAK_FALL) {                            // fall rate 
            if(_peak > 0) _peak--;
            _dotCount = 0;
        }
        
        _vol[_volCount] = n;                                          // Save sample for dynamic leveling
        if (++_volCount >= SAMPLES) _volCount = 0;                    // Advance/rollover sample counter
        
        // Get volume range of prior frames
        minLvl = maxLvl = _vol[0];
        for (i=1; i<SAMPLES; i++) {
            if (_vol[i] < minLvl)      minLvl = _vol[i];
            else if (_vol[i] > maxLvl) maxLvl = _vol[i];
        }
        // minLvl and maxLvl indicate the volume range over prior frames, used
        // for vertically scaling the output graph (so it looks interesting
        // regardless of volume level).  If they're too close together though
        // (e.g. at very low volume levels) the graph becomes super coarse
        // and 'jumpy'...so keep some minimum distance between them (this
        // also lets the graph go to zero when no sound is playing):
        if((maxLvl - minLvl) < _TOP) maxLvl = minLvl + _TOP;
        _minLvlAvg = (_minLvlAvg * 63 + minLvl) >> 6;                 // Dampen min/max levels
        _maxLvlAvg = (_maxLvlAvg * 63 + maxLvl) >> 6;       
    }

    void soundmems() override {
    }

private:
    uint8_t _NOISE;                  // Noise/hum/interference in mic signal and increased value until it went quiet
    uint8_t _PEAK_FALL;              // Rate of peak falling dot
    uint8_t _TOP = (NUM_LEDS + 2);   // Allow dot to go slightly off scale
    uint8_t _peak      = 0;          // Used for falling dot
    uint8_t _dotCount  = 0;          // Frame counter for delaying dot-falling speed
    uint8_t _volCount  = 0;          // Frame counter for storing past volume data
    uint16_t _vol[SAMPLES];          // Collection of prior volume samples
    uint16_t _lvl = 10;              // Current "dampened" audio level
    uint16_t _minLvlAvg = 0;         // For dynamic adjustment of graph low & high
    uint16_t _maxLvlAvg = 512;    
};