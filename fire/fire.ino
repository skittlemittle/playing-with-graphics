// fire simulation on a led matrix
// based on https://web.archive.org/web/20160418004150/http://freespace.virgin.net/hugo.elias/models/m_fire.htm
// skittlemittle

#include <FastLED.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 64
#define FRAMERATE 1

const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
const bool kMatrixSerpentineLayout = true;
#define NUM_LEDS (kMatrixWidth *  kMatrixHeight)
CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);
// fire buffers fire buffers
int buffer1[NUM_LEDS];
int buffer2[NUM_LEDS];
int scroll = 0;
// performance amirite
int coolingMap [] = {
    0,0,0,0,0,0,0,0,
    0,13,34,30,15,33,1,0,
    0,44,44,50,62,10,37,0,
    0,32,76,71,34,71,3,0,
    0,54,54,60,73,11,42,0,
    0,22,57,48,24,49,2,0,
    0,22,19,24,26,4,16,0,
    0,0,0,0,0,0,0,0
};

uint16_t XYsafe(uint8_t x, uint8_t y);

void setup()
{
    FastLED.setBrightness(BRIGHTNESS);
    // a line of "source fire" at the bottom
    for (size_t x = 0; x < kMatrixWidth; x++) {
        int y = kMatrixHeight - 1;
        int index = x + y * kMatrixWidth;
        buffer1[index] = 255;
        buffer2[index] = 255;
    }
    Serial.begin(9600);
}

void loop()
{
    // "computations", we only mess with the brightness of each pixel
    for (size_t x = 1; x < kMatrixWidth - 1; x++) {
        for (size_t y = 1; y < kMatrixHeight - 1; y++) {
            // the up, down, left, and right neighbors
            int neighbor1 = buffer1[x + 1 + y * kMatrixWidth];
            int neighbor2 = buffer1[x - 1 + y * kMatrixWidth];
            int neighbor3 = buffer1[x + (y + 1) * kMatrixWidth];
            int neighbor4 = buffer1[x + (y - 1) * kMatrixWidth];

            int newColor = (neighbor1 + neighbor2 + neighbor3 + neighbor4) * 0.25;
            newColor -= coolingMap[(x + (y + scroll) * kMatrixWidth)];
            if (newColor < 0) newColor = 0;
            if (newColor > 255) newColor = 255;

            buffer2[x + (y - 1) * kMatrixWidth] = newColor; // don't disturb source
        }
    }

    // draw
    for (size_t x = 0; x < kMatrixWidth; x++) {
        for (size_t y = 0; y < kMatrixHeight; y++) {
            byte brightness = buffer2[x + y * kMatrixWidth];
            leds[(XYsafe(x, y))].setHSV(32, 255, brightness); //orange
        }
    }
    // swamp
    memcpy(buffer1, buffer2, sizeof buffer2);
    scroll++;
    if (scroll > kMatrixHeight) scroll = 0;

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
