// falling sand hm
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <FastLED.h>
#include <Wire.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 64
#define FRAMERATE 5

Adafruit_MPU6050 mpu;
const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);
// sand buffers sand buffers
byte current[NUM_LEDS];
byte next[NUM_LEDS];

uint16_t XYsafe(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                bool snakeLayout = true);

void fall(int x, int y, int target) {
  next[target] = 255;
  next[x + y * kMatrixWidth] = 0;
}

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);

  // init sand
  for (size_t x = 0; x < kMatrixWidth; x++) {
    for (size_t y = 0; y <= 4; y++) {
      int index = x + y * kMatrixWidth;
      current[index] = 255;
      next[index] = 255;
    }
  }

  if (!mpu.begin()) {
    Serial.println("accelerometer did a heck");
    while (1)
      delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  int gravx = map(constrain(a.acceleration.x, -3, 3), -3, 3, -1, 1);
  int gravy = map(constrain(a.acceleration.y, -3, 3), -3, 3, -1, 1);
  Serial.print(gravx);
  Serial.print(" ");
  Serial.println(gravy);

  // "computations"
  for (size_t x = 1; x < kMatrixWidth - 1; x++) {
    for (size_t y = 1; y < kMatrixHeight - 1; y++) {
      if (current[x + y * kMatrixWidth] == 0)
        continue;

      int under = ((x + (1 * gravx)) + (y + (1 * gravy)) * kMatrixWidth);
      int left = ((x + (1 * gravx)) + (y + (1 * gravy)) * kMatrixWidth);
      int right = ((x - (1 * gravx)) + (y + (1 * gravy)) * kMatrixWidth);
      // sand fall down
      if (current[under] == 0) {
        fall(x, y, under);
      } else if (current[left] == 0 && current[right] == 0) {
        // oof
        (random(2) == 1) ? fall(x, y, left) : fall(x, y, right);
      } else if (current[left] == 0) {
        fall(x, y, left);
      } else if (current[right] == 0) {
        fall(x, y, right);
      }
    }
  }

  // draw
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte brightness = next[x + y * kMatrixWidth];
      // orange sand
      leds[(XYsafe(x, y, kMatrixWidth, kMatrixHeight))].setHSV(32, 255,
                                                               brightness);
    }
  }
  // swamp
  memcpy(current, next, sizeof next);

  FastLED.show();
  delay(1000 / FRAMERATE);
}

/*
 *Helper function that returns led index number given an xy
 *yoinked from the fastled examples:https://github.com/FastLED/
 */
uint16_t XY(uint8_t x, uint8_t y, bool snakeLayout = true) {
  uint16_t i;

  if (snakeLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if (snakeLayout == true) {
    if (y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }

  return i;
}

/*
 * wraps XY() and gives you array out of bounds protection
 * also yoinked from the fastled examples
 */
uint16_t XYsafe(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                bool snakeLayout = true) {
  if (x >= width)
    return -1;
  if (y >= height)
    return -1;
  return XY(x, y);
}
