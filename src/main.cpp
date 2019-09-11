#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define LIGHT_PIN    10
// #define VIBRA_PIN    16
#define NR_OF_PIXELS 12
#define TIMEOUT_VIBRA_MS   50
#define TIMEOUT_LIGHTS_MS 600

Adafruit_NeoPixel strip(NR_OF_PIXELS, LIGHT_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t RED   = strip.Color(255,   0,   0);
const uint32_t GREEN = strip.Color(  0, 255,   0);
const uint32_t BLUE  = strip.Color(  0,   0, 255);
const uint32_t BLACK = 0;

ClickEncoder encoder(A0, A1, A2);
unsigned long lastInteraction = 0;
int16_t intensity = 0;

void setColor(uint32_t c);
void volumeChange(uint16_t key, uint32_t color);

void setup() {
  strip.begin();
  setColor(BLACK);

  // pinMode(VIBRA_PIN, OUTPUT);

  Timer1.attachInterrupt([](){ encoder.service(); }, 1000 );

  Consumer.begin();
}

void loop() {
  int16_t value = encoder.getValue();
  if (value != 0) {
    if (value < 0) {
      intensity = max(1, min(intensity + 1, 10));
      volumeChange(MEDIA_VOL_UP, GREEN);
    }
    else {
      intensity = min(-1, max(intensity - 1, -10));
      volumeChange(MEDIA_VOL_DOWN, RED);
    }
  }
  ClickEncoder::Button b = encoder.getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        intensity = 9;
        volumeChange(MEDIA_VOL_MUTE, BLUE);
        break;
    }
  }
  //
  // LEDs / Vibration nach inaktiver Zeit abschalten.
  //
  unsigned long timeDiff = millis() - lastInteraction;
  if (timeDiff > TIMEOUT_VIBRA_MS) {
    // digitalWrite(VIBRA_PIN, LOW);
  }
  if (timeDiff > TIMEOUT_LIGHTS_MS) {
    setColor(BLACK);
    intensity = 0;
  }
}

void volumeChange(uint16_t key, uint32_t color) {
  // digitalWrite(VIBRA_PIN, HIGH);
  setColor(color);
  Consumer.write(key);
  lastInteraction = millis();
}

void setColor(uint32_t c) {
  strip.setBrightness(abs(intensity) * 255 / 10);
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}
