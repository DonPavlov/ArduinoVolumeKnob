#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define TOGGLE_PIN 3
#define LIGHT_PIN    10
#define NR_OF_PIXELS 16
#define TIMEOUT_VIBRA_MS   50
#define TIMEOUT_LIGHTS_MS 600

Adafruit_NeoPixel strip(NR_OF_PIXELS, LIGHT_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t RED   = strip.Color(255,   0,   0);
const uint32_t GREEN = strip.Color(  0, 255,   0);
const uint32_t BLUE  = strip.Color(  0,   0, 255);
const uint32_t BLACK = 0;

ClickEncoder encoder(A1, A2, A0);
unsigned long lastInteraction = 0;
int16_t intensity = 0;

void setColor(uint32_t c);
void volumeChange(uint16_t key, uint32_t color);
void setup();
void loop();

void setup() {
  lastInteraction = 0;
  intensity = 0;
  strip.begin();
  strip.show();
  setColor(BLACK);
  // disable leds because we only need neopixels
  pinMode(LED_BUILTIN_TX,INPUT);
  pinMode(LED_BUILTIN_RX,INPUT);

  pinMode(TOGGLE_PIN, INPUT);
  uint8_t val = digitalRead(TOGGLE_PIN);

#ifdef DEBUG
  Serial.begin(115200);
  while(true) {
    Serial.println(val);
    delay(1000);
    val = digitalRead(TOGGLE_PIN);
  }
#endif

  Timer1.stop();
  Timer1.detachInterrupt();
  Timer1.initialize(1000);
  Timer1.attachInterrupt([]{ encoder.service(); });
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
  // LEDs nach inaktiver Zeit abschalten.
  //
  unsigned long timeDiff = millis() - lastInteraction;
  if (timeDiff > TIMEOUT_LIGHTS_MS) {
    setColor(BLACK);
    intensity = 0;
  }
}

void volumeChange(uint16_t key, uint32_t color) {
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
