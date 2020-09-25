// fire simulation on a led matrix
// based on https://web.archive.org/web/20160418004150/http://freespace.virgin.net/hugo.elias/models/m_fire.htm
// skittlemittle

#include <FastLED.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 16

const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
const bool kMatrixSerpentineLayout = true;
#define NUM_LEDS (matrixWidth *  matrixHeight)
CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);
// fire buffers fire buffers
CHSV buffer1[NUM_LEDS];
CHSV buffer2[NUM_LEDS];
int scroll;
// performance amirite
int coolingMap [] = {}; // TODO

uint16_t XYsafe(uint8_t x, uint8_t y);

void setup()
{
    // a line of "source fire" at the bottom
    for (size_t x = 0; x < kMatrixWidth; x++) {
        int y = kMatrixHeight - 1;
        int index = x + y * kMatrixWidth;
        buffer1[index] = CHSV(32, 255, 255); // orang
    }
}

void loop()
{
    // "computations", we only mess with the brightness of each pixel
    for (size_t x = 1; x < kMatrixWidth - 1; x++) {
        for (size_t y = 1; y < kMatrixHeight - 1; y++) {
            // the up, down, left, and right neighbors
            int neighbor1 = buffer1[x + 1 + y * kMatrixWidth].val;
            int neighbor2 = buffer1[x - 1 + y * kMatrixWidth].val;
            int neighbor3 = buffer1[x + (y + 1) * kMatrixWidth].val;
            int neighbor4 = buffer1[x + (y - 1) * kMatrixWidth].val;

            int newColor = (neighbor1 + neighbor2 + neighbor3 + neighbor4) * 0.25;
            newColor -= coolingMap[(x + (y + scroll) * kMatrixWidth)];

            buffer2[x + (y - 1) * kMatrixWidth].val = newColor; // don't disturb source
        }
    }

    // draw
    for (size_t x = 0; x < kMatrixWidth; x++) {
        for (size_t y = 0; y < kMatrixHeight; y++) {
            leds[(XYsafe(x, y))] = buffer2[x + y * kMatrixWidth];
        }
    }
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    // swamp
    buffer1 = buffer2;
    scroll++;
    if (scroll > kMatrixHeight) scroll = 0;
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
