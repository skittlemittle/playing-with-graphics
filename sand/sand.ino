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
const bool kMatrixSerpentineLayout = true;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);
// sand buffers sand buffers
#define BUFFER_SIZE (kMatrixHeight + 1) * (kMatrixWidth + 1)
byte current[BUFFER_SIZE];
byte next[BUFFER_SIZE];

uint16_t XYsafe(uint8_t x, uint8_t y);

void fall(char dir, int x, int y)
{
    if (dir == 'L') {
        next[(x + 1) + (y + 1) * kMatrixWidth] = 255;
        next[x + y * kMatrixWidth] = 0;
    } else if (dir == 'R') {
        next[(x - 1) + (y + 1) * kMatrixWidth] = 255;
        next[x + y * kMatrixWidth] = 0;
    } else if (dir == 'D') {
        next[x + (y + 1) * kMatrixWidth] = 255;
        next[x + y * kMatrixWidth] = 0;
    }
}

void setup()
{
    Serial.begin(9600);
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);

    // init sand
    for (size_t x = 0; x < kMatrixWidth; x++) {
        for (size_t y = 0; y <= 3; y++) {
            // int y = 0;
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
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    // "computations", we only mess with the brightness of each pixel
    for (size_t x = 0; x < kMatrixWidth; x++) {
        for (size_t y = 0; y < kMatrixHeight; y++) {
            if (current[x + y * kMatrixWidth] == 0) continue;
            // sand fall down
            if (current[x + (y + 1) * kMatrixWidth] == 0) {
                fall('D', x, y);
            } else if (
                current[(x + 1) + (y + 1) * kMatrixWidth] == 0 &&
                current[(x - 1) + (y + 1) * kMatrixWidth]) {
                // oof
                (random(2) == 1) ? fall('L', x, y) : fall('R', x, y);
            } else if (current[(x + 1) + (y + 1) * kMatrixWidth] == 0) {
                fall('L', x, y);
            } else if (current[(x - 1) + (y + 1) * kMatrixWidth] == 0) {
                fall('R', x, y);
            }
        }
    }

    // draw
    for (int x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
            byte brightness = next[x + y * kMatrixWidth];
            leds[(XYsafe(x, y))].setHSV(32, 255, brightness);  //orange sand
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
uint16_t XY(uint8_t x, uint8_t y)
{
    uint16_t i;

    if (kMatrixSerpentineLayout == false) {
        i = (y * kMatrixWidth) + x;
    }

    if (kMatrixSerpentineLayout == true) {
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
uint16_t XYsafe(uint8_t x, uint8_t y)
{
    if (x >= kMatrixWidth)
        return -1;
    if (y >= kMatrixHeight)
        return -1;
    return XY(x, y);
}
