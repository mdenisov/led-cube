#ifndef LEDFX_H
#define LEDFX_H

#include "const.h"
#include <Arduino.h>
#include <FastLED.h>

// enum Effect { Rainbow = 0, RainbowVertical = 1, RainbowHorizontal = 2, LineHorizontal = 3, SparklesRoutine = 4, ColorRoutine = 5 };

class LedFx
{
public:
    LedFx();

    uint8_t effect = 0;

    void begin(uint8_t effect = 0);
    void loop();
    void disable();
    void enable();
    void setEffect(uint8_t effect);
    void setNextEffect();
    void setDebugMode(boolean debug);

private:
    CRGB _leds[NUM_LEDS];
    CRGBPalette16 _palette = RainbowColors_p;
    TBlendType _blending;
    uint32_t _effectTimer = 0;
    uint32_t _fadeTimer = 0;
    boolean _enabled = true;
    uint8_t _brightness = BRIGHTNESS;
    uint8_t _effects = 15;

    // effect variables
    uint8_t _effectStartIndex = 0;
    uint8_t _effectHue = 0;
    uint8_t _effectSat = 255;
    uint16_t _effectScale = 30;
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
    void _fillAll(CRGB color);
    void _fillFromPalette(uint8_t colorIndex);
    void _fillNoiseLED();
    CRGB _getColorForSide(int s, int brigthness);
    uint16_t _getPixelNumber(int8_t x, int8_t y);
    uint32_t _getPixelColor(int segment);
    uint32_t _getPixColorXY(int8_t x, int8_t y);
    void _drawPixelXY(int8_t x, int8_t y, CRGB color);
    void _fadePixel(byte i, byte j, byte step);
    void _fader(byte step);
    void _fade(byte step);
};
#endif