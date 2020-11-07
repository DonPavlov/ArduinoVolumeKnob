#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ClickEncoder.h>
#include <HID-Project.h>
#include <TimerOne.h>

#define TOGGLE_PIN 3
#define LIGHT_PIN 10
#define NR_OF_PIXELS 16
#define TIMEOUT_VIBRA_MS 50
#define TIMEOUT_LIGHTS_MS 600

#define DEBUG 1
Adafruit_NeoPixel strip(NR_OF_PIXELS, LIGHT_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t WHITE = strip.Color(255, 255, 255);
const uint32_t RED = strip.Color(255, 0, 0);
const uint32_t GREEN = strip.Color(0, 255, 0);
const uint32_t BLUE = strip.Color(0, 0, 255);
const uint32_t BLACK = 0;
const uint32_t PINK = strip.Color(255, 0, 255);
const uint32_t DANGER_GREEN = strip.Color(51, 255, 51);
const uint32_t YELLOW = strip.Color(240, 240, 0);
const uint32_t ORANGE = strip.Color(255, 128, 0);
const uint32_t TURKOISE = strip.Color(101, 240, 240);

ClickEncoder encoder(A1, A2, A0);
unsigned long lastInteraction = 0;
int16_t intensity = 0;

void setColor(uint32_t c);
void volumeChange(uint16_t key, uint32_t color);
void setup();
void loop();
void grubSelector();
void colorWipe(uint32_t c, uint8_t wait);
void setColorState(uint8_t state);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void startShow(uint8_t i);

void setup() {
  delay(3000);
  lastInteraction = 0;
  intensity = 0;
  strip.begin();
  strip.show();
  setColor(BLACK);
  // disable leds because we only need neopixels
  pinMode(LED_BUILTIN_TX, INPUT);
  pinMode(LED_BUILTIN_RX, INPUT);

  pinMode(TOGGLE_PIN, INPUT);
#ifdef DEBUG
  Serial.begin(115200);
  uint8_t val1 = digitalRead(TOGGLE_PIN);
  Serial.println(val1);
#endif
  Timer1.stop();
  Timer1.detachInterrupt();
  delay(100);
  // uint8_t val = digitalRead(TOGGLE_PIN);
  // if(val == 1) {
  //   delay(5000);  // wait 15 seconds before it is usable
  //   // grubSelector();   // currently not needed thanks to Linux only boot
  // }

  Timer1.initialize(1000);
  Timer1.attachInterrupt([] { encoder.service(); });

  Consumer.begin();
}

uint8_t colorState = 0u;
uint8_t debugval = 0u;
uint8_t count = 0;

uint8_t showType = 0;

void loop() {
  int16_t value = encoder.getValue();
  if (value != 0) {
    if (value < 0) {
      intensity = max(1, min(intensity + 1, 10));
      volumeChange(MEDIA_VOLUME_UP, GREEN);
#ifdef DEBUG
      Serial.println("VolUp");
#endif
    } else {
      intensity = min(-1, max(intensity - 1, -10));
      volumeChange(MEDIA_VOLUME_DOWN, RED);
#ifdef DEBUG
      Serial.println("VolDown");
#endif
    }
  }

  ClickEncoder::Button b = encoder.getButton();
  if (b != ClickEncoder::Open) {
    if (b == ClickEncoder::Clicked) {
      intensity = 9;
      volumeChange(MEDIA_VOL_MUTE, BLUE);
#ifdef DEBUG
      Serial.println("Mute");
#endif
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

void grubSelector() {
  // Select Windows OS
  BootKeyboard.begin();
  BootKeyboard.write(KEY_DOWN_ARROW);
  delay(100);
  BootKeyboard.write(KEY_DOWN_ARROW);
  delay(100);
  BootKeyboard.write(KEY_ENTER);
  delay(100);

  BootKeyboard.end();
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setColorState(uint8_t state) {
  switch (state) {
    case 0:
      setColor(WHITE);
      break;
    case 1:
      setColor(RED);
      break;
    case 2:
      setColor(GREEN);
      break;
    case 3:
      setColor(BLUE);
      break;
    case 4:
      setColor(PINK);
      break;
    case 5:
      setColor(DANGER_GREEN);
      break;
    case 6:
      setColor(YELLOW);
      break;
    case 7:
      setColor(ORANGE);
      break;
    case 8:
      setColor(TURKOISE);
      break;
    default:
      break;
  }
}