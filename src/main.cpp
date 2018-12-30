#include <Arduino.h>
#include <LEDStrip.h>
#include <EEPROM.h>
#include <jsbutton.h>

// Fixed definitions cannot change on the fly.
#define LED_DT 12                                             // Data pin to connect to the strip.
#define LED_CK 11                                             // Clock pin for WS2801 or APA102.
#define COLOR_ORDER BGR                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE APA102                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS 60                                           // Number of LED's.
#define MIC_PIN A0                                             // Analog port for microphone
#define BUTTON_PIN 2

// Initialize global variables for sequences
int thisdelay = 20;                                           // A delay value for the sequence(s)
CRGB leds[NUM_LEDS];
int eepaddress = 0;

void readbutton();

uint8_t effect = 0;

#define MAX_EFFECTS 5
BaseEffect* effects[] = {
  new PaletteEffect<MIC_PIN>(),
  new BraceletEffect<NUM_LEDS, MIC_PIN, 60>(),
  new WaveEffect<NUM_LEDS, MIC_PIN>(),
  new NoiseEffect<NUM_LEDS, MIC_PIN, 24>(),
  new SimpleEffect<NUM_LEDS, MIC_PIN>()
};

LEDStrip<20> strip(effects, MAX_EFFECTS);

void setup() {
  analogReference(EXTERNAL);

  Serial.begin(57600);                                        // Initialize serial port for debugging.
  delay(1000);                                                // Soft startup to ease the flow of electrons.
  
  pinMode(BUTTON_PIN, INPUT);
  
  effect = EEPROM.read(eepaddress);
  if(effect > MAX_EFFECTS) effect = 0;

  strip.begin(&FastLED.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS));
} // setup()

void loop() {
  strip.loop();

  readbutton();

  EVERY_N_SECONDS(1) {
    strip.switchTo(effect);
  }
} // loop()

void readbutton() {
  uint8_t b = checkButton(BUTTON_PIN);

  if(b == 1) {
    effect = (effect+1) % MAX_EFFECTS;
  }

  if(b == 2) {
    effect = 0;
  }

  if(b == 3) {
    EEPROM.write(eepaddress, effect);
  }
} // readbutton()