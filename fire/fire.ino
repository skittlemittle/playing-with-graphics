// fire simulation on a led matrix
// based on https://web.archive.org/web/20160418004150/http://freespace.virgin.net/hugo.elias/models/m_fire.htm
// skittlemittle

#include <FastLED.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 64
#define FRAMERATE 5

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
const int mapHeight = 32;
int coolingMap [] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,11,4,13,9,14,16,12,12,9,5,10,7,19,8,20,7,11,3,4,2,6,3,7,2,5,2,1,2,0,0,2,11,4,17,7,19,18,19,23,16,14,11,12,22,16,35,15,29,10,13,3,8,3,14,3,13,2,8,3,1,0,0,5,3,12,3,16,9,18,20,18,20,14,18,13,21,32,22,41,18,29,7,13,2,15,4,20,4,19,3,8,3,0,0,3,9,3,14,3,15,11,15,17,17,22,10,26,15,25,33,21,40,14,26,3,15,2,18,7,25,7,19,5,4,0,0,6,3,13,2,15,4,12,10,15,18,13,26,6,25,15,19,32,15,34,6,20,2,14,5,18,10,23,7,11,3,0,0,1,8,1,11,2,9,4,8,8,10,15,5,17,3,13,12,9,22,6,19,2,10,2,8,6,14,7,12,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

uint16_t XYsafe(uint8_t x, uint8_t y);

void setup()
{
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);
    // a line of "source fire" at the bottom
    for (size_t x = 0; x < kMatrixWidth; x++) {
        int y = kMatrixHeight - 1;
        int index = x + y * kMatrixWidth;
        buffer1[index] = 255;
        buffer2[index] = 255;
    }
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

            buffer2[x + (y) * kMatrixWidth] = newColor; // don't disturb source
        }
    }

    // draw
    for (int x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
            byte brightness = buffer2[x + y * kMatrixWidth];
            leds[(XYsafe(x, y))].setHSV(32, 255, brightness); //orange
        }
    }
    // swamp
    memcpy(buffer1, buffer2, sizeof buffer2);
    scroll++;
    if (scroll > mapHeight) scroll = 0;

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
