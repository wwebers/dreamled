#pragma once

#include <FastLED.h>

#define DC_OFFSET      60 
#define FILTER(x)      ((x>503) &&( x<520)) ? 512 : x
#define NORMALIZE(x)   map(FILTER(x),400,600,0,1023) - DC_OFFSET

class BaseEffect {
public:
    BaseEffect() {};

    virtual void begin(CLEDController* controller) {
        _controller = controller;
    }

    virtual void loop() = 0;
    virtual void soundmems() = 0;

protected:
    CLEDController* _controller;
};