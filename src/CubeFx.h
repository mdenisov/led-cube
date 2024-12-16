#ifndef CUBE_FX_H
#define CUBE_FX_H

#include "const.h"
#include <Arduino.h>
#include <FastLED.h>

// enum Effect { Rainbow = 0, RainbowVertical = 1, RainbowHorizontal = 2, LineHorizontal = 3, SparklesRoutine = 4, ColorRoutine = 5 };

class CubeFx
{
public:
    // CubeFx(uint8_t size);
    CubeFx();

    uint8_t effect = 0;

    void begin(uint8_t effect = 0);
    void loop();
    void disable();
    void enable();
    boolean enabled();
    uint8_t brightness();
    void setEffect(uint8_t effect);
    void setNextEffect();

private:
    uint8_t _size;
    uint8_t _matrix[6][WIDTH][HEIGHT] = {
        {
            {2, 1, 0}, {5, 4, 3}, {8, 7, 6}
        },
        {
            {11, 10, 9}, {14, 13, 12}, {17, 16, 15}
        },
        {
            {20, 19, 18}, {23, 22, 21}, {26, 25, 24}
        },
        {
            {29, 28, 27}, {32, 31, 30}, {35, 34, 33}
        },
        {
            {38, 37, 36}, {41, 40, 39}, {44, 43, 42}
        },
        {
            {53, 52, 51}, {50, 49, 48}, {47, 46, 45}
        }
    };
    CRGB _leds[NUM_LEDS];
    CRGBPalette16 _palette = RainbowColors_p;
    TBlendType _blending;
    uint32_t _effectTimer = 0;
    uint32_t _fadeTimer = 0;
    boolean _enabled = true;
    uint8_t _brightness = BRIGHTNESS;
    uint8_t _effects = 16;

    // effect variables
    uint8_t _effectStartIndex = 0;
    uint8_t _effectHue = 0;
    uint8_t _effectSat = 255;
    uint16_t _effectScale = EFFECT_SCALE;
    uint16_t _effectSpeed = EFFECT_SPEED;
    uint8_t _effectColorLoop = 1;
    uint8_t _effectNoise[WIDTH][WIDTH];

    // effects
    void _rainbowEffect();
    void _rainbowVerticalEffect();
    void _rainbowDiagonalEffect();
    void _lineHorizontalEffect();
    void _sparklesRoutineEffect();
    void _colorRoutineEffect();
    void _axelColorRoutineEffect();
    void _snowRoutineEffect();
    void _matrixRoutineEffect();
    void _rainbowNoiseEffect();
    void _rainbowStripeNoiseEffect();
    void _zebraNoiseEffect();
    void _forestNoiseEffect();
    void _oceanNoiseEffect();
    void _plasmaNoiseEffect();
    void _cloudNoiseEffect();
    void _lavaNoiseEffect();

    // utils
    CRGB _getColorForSide(int s, int brigthness);
    uint8_t _getPixelAddr(uint8_t side, int x, int y);
    uint8_t _getPixelNumber(int8_t x, int8_t y);
    uint32_t _getPixelColor(int segment);
    uint32_t _getPixColorXY(int8_t x, int8_t y);
    void _fillAll(CRGB color);
    void _fillFromPalette(uint8_t colorIndex);
    void _fillNoiseLED();
    void _drawPixelXY(int8_t x, int8_t y, CRGB color);
    void _fadePixel(byte i, byte j, byte step);
    void _fader(byte step);
    void _fade(byte step);
};
#endif