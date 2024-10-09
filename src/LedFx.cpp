#include "LedFx.h"
#include "const.h"

// LedFx::LedFx(uint8_t size)
LedFx::LedFx()
{
    // _size = size;
}

void LedFx::begin(uint8_t eff)
{
    pinMode(LED_PIN, OUTPUT);

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(_leds, NUM_LEDS); //.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxPowerInVoltsAndMilliamps(MAX_VOLTS, MAX_MILLIAMPS);

    effect = eff;
    _palette = RainbowColors_p;
    _blending = LINEARBLEND;
}

void LedFx::loop()
{
    if (_enabled && millis() - _effectTimer >= _effectSpeed)
    {
        _effectTimer = millis();

        switch (effect)
        {
        case 0:
            _rainbowEffect();
            break;
        case 1:
            _rainbowVerticalEffect();
            break;
        case 2:
            _rainbowDiagonalEffect();
            break;
        case 3:
            _axelColorRoutineEffect();
            break;
        case 4:
            _lineHorizontalEffect();
            break;
        case 5:
            _sparklesRoutineEffect();
            break;
        case 6:
            _colorRoutineEffect();
            break;
        case 7:
            _rainbowNoiseEffect();
            break;
        case 8:
            _rainbowStripeNoiseEffect();
            break;
        case 9:
            _zebraNoiseEffect();
            break;
        case 10:
            _forestNoiseEffect();
            break;
        case 11:
            _oceanNoiseEffect();
            break;
        case 12:
            _plasmaNoiseEffect();
            break;
        case 13:
            _cloudNoiseEffect();
            break;
        case 14:
            _lavaNoiseEffect();
            break;
        case 15:
            _snowRoutineEffect();
            break;
            // case 16:
            //     _matrixRoutineEffect();
            //     break;
        }

        FastLED.show();
        // FastLED.delay(_effectSpeed);
    }

    // if (_enabled)
    // {
    //     FastLED.show();
    // }
    // else
    // {
    //     FastLED.clear();
    //     FastLED.show();
    // }

    if (_enabled && _brightness < BRIGHTNESS && millis() - _fadeTimer >= 50)
    {
        // _brightness = constrain(_brightness + 1, 0, BRIGHTNESS);
        _brightness += 1;

        FastLED.setBrightness(_brightness);
        FastLED.show();
    }

    if (!_enabled && _brightness > 0 && millis() - _fadeTimer >= 50)
    {
        // _brightness = constrain(_brightness - 1, 0, BRIGHTNESS);
        _brightness -= 1;

        FastLED.setBrightness(_brightness);

        if (_brightness == 0)
        {
            FastLED.clear();
        }

        FastLED.show();
    }
}

void LedFx::disable()
{
    if (!_enabled)
    {
        return;
    }

    _enabled = false;
}

void LedFx::enable()
{
    if (_enabled)
    {
        return;
    }

    _enabled = true;
}

void LedFx::setEffect(uint8_t eff)
{
    if (eff == effect)
    {
        return;
    }

    Serial.print("setEffect: ");
    Serial.println(eff);

    if (eff >= _effects)
    {
        eff = 0;
    }

    effect = eff;
    _effectStartIndex = 0;
    _effectHue = 0;
    _effectTimer = millis();
}

void LedFx::setNextEffect()
{
    setEffect(effect + 1);
}

// effects
void LedFx::_rainbowEffect()
{
    _effectStartIndex += 1;
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED;

    _fillFromPalette(_effectStartIndex);
}

void LedFx::_rainbowVerticalEffect()
{
    uint8_t size = WIDTH * HEIGHT;
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED * 3;

    _fillAll(CRGB::Black);

    for (byte s = 1; s < SEGMENTS - 1; s++)
    {
        for (byte j = 0; j < WIDTH; j++)
        {
            _effectHue += 3;
            CHSV thisColor = CHSV((byte)(_effectHue * EFFECT_SCALE), 255, 255);

            for (byte m = 0; m < HEIGHT; m++)
            {
                // uint8_t position = size * s + j + (m * HEIGHT);
                uint8_t pos = _getPixelAddr(s, j, m);
                // _leds[position] = thisColor;
                _leds[pos] = thisColor;
            }
        }

        for (byte j = 0; j < WIDTH; j++)
        {
            for (byte m = 0; m < HEIGHT; m++)
            {
                if (j == 0 || j == WIDTH - 1 || m == 0 || m == HEIGHT - 1)
                {
                    CRGB color = _leds[size * s + j * HEIGHT];
                    _leds[j + m * HEIGHT] = color;
                    _leds[size * (SEGMENTS - 1) + j + m * HEIGHT] = color;
                }
            }
        }
    }

    // _fader(70);
}

void LedFx::_rainbowDiagonalEffect()
{
    uint8_t size = WIDTH * HEIGHT;
    _effectHue += 1;
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED * 3;

    // uint8_t hueMask[3][3] = {
    //     { 1, 3, 5 },
    //     { 3, 5, 7 },
    //     { 5, 7, 9 }
    // };

    // _fillAll(CRGB::Black);

    uint8_t idx = size;
    for (byte s = 1; s < SEGMENTS - 1; s++)
    {
        for (byte j = 0; j < WIDTH; j++)
        {
            uint8_t w = j;

            if (s % 2 == 0)
            {
                w = WIDTH - j;
            }

            for (byte m = 0; m < HEIGHT; m++)
            {
                // CRGB thisColor = ColorFromPalette(_palette, (byte)((w + m + _effectHue) * EFFECT_SCALE), 255);
                CHSV thisColor = CHSV((byte)((w + m + _effectHue) * (EFFECT_SCALE / 4)), 255, 255);

                uint8_t pos = _getPixelAddr(s, m, j);
                _leds[pos] = thisColor;
                // _leds[idx] = thisColor;
                idx += 1;
            }
        }
    }

    for (byte j = 0; j < WIDTH * HEIGHT; j++)
    {
        _leds[j] = _leds[size];
        _leds[size * (SEGMENTS - 1) + j] = _leds[(size * 2) - 1];
    }

    // _fader(70);
}

void LedFx::_lineHorizontalEffect()
{
    static uint8_t direction = 0;

    uint8_t size = WIDTH * HEIGHT;
    _effectHue += 5;
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED * 3;
    CHSV thisColor = CHSV((byte)(_effectHue * (EFFECT_SCALE / 4)), 255, 255);
    CHSV thisFadeColor = CHSV((byte)(_effectHue * (EFFECT_SCALE / 4)), 255, 125);

    _fillAll(CRGB::Black);

    if (_effectStartIndex > 0 && _effectStartIndex <= HEIGHT)
    {
        for (byte s = 1; s <= 4; s++)
        {
            // uint8_t position = size * s + HEIGHT * (_effectStartIndex - 1);

            for (byte i = 0; i < WIDTH; i++)
            {
                uint8_t pos = _getPixelAddr(s, i, _effectStartIndex - 1);
                _leds[pos] = thisColor;
                // _leds[position + i] = thisColor;

                if (direction == 1)
                {
                    if (_effectStartIndex > 1)
                    {
                        uint8_t pos = _getPixelAddr(s, i, _effectStartIndex - 2);
                        _leds[pos] = thisFadeColor;
                    }
                } else {
                    if (_effectStartIndex < HEIGHT)
                    {
                        uint8_t pos = _getPixelAddr(s, i, _effectStartIndex);
                        _leds[pos] = thisFadeColor;
                    }
                }
            }
        }
    }

    if (_effectStartIndex == 0)
    {
        for (byte i = 0; i < size; i++)
        {
            _leds[i] = thisColor;
        }
    }

    if (_effectStartIndex == HEIGHT + 1)
    {
        uint8_t position = size * 5;
        for (byte i = 0; i < size; i++)
        {
            _leds[position + i] = thisColor;
        }
    }

    _effectStartIndex = constrain(direction > 0 ? _effectStartIndex += 1 : _effectStartIndex -= 1, 0, HEIGHT + 1);

    if (_effectStartIndex == HEIGHT + 1)
    {
        direction = 0;
    }

    if (_effectStartIndex == 0)
    {
        direction = 1;
    }
}

void LedFx::_sparklesRoutineEffect()
{
    // _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED * 5;

    for (byte i = 0; i < NUM_LEDS; i++)
    {
        _leds[i] = CHSV(random(0, 255), 255, 255);
    }
    // for (byte i = 0; i < EFFECT_SCALE; i++)
    // {
    //     byte x = random(0, WIDTH);
    //     byte y = random(0, HEIGHT);

    //     if (_getPixColorXY(x, y) == 0)
    //     {
    //         // _leds[_getPixelNumber(x, y)] = CHSV(random(0, 255), 255, 255);
    //         _drawPixelXY(x, y, CHSV(random(0, 255), 255, 255));
    //     }
    // }

    _fader(70);
}

void LedFx::_colorRoutineEffect()
{
    _effectHue += 10;
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED;

    for (byte s = 0; s < SEGMENTS; s++)
    {
        CHSV thisColor = CHSV((byte)(_effectHue + s * EFFECT_SCALE), 255, 255);

        for (byte j = 0; j < HEIGHT; j++)
        {
            for (byte i = 0; i < WIDTH; i++)
            {
                _drawPixelXY(i, j, thisColor);
            }
        }
    }
}

void LedFx::_axelColorRoutineEffect()
{
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED;
    CRGB sides[6] = {
        _getColorForSide(1, 255),
        _getColorForSide(2, 255),
        _getColorForSide(3, 255),
        _getColorForSide(4, 255),
        _getColorForSide(5, 255),
        _getColorForSide(6, 255),
    };

    for (int i = 0; i < SEGMENTS; i++)
    {
        uint8_t lenght = WIDTH * HEIGHT;
        uint8_t position = lenght * i;

        for (int j = 0; j < lenght; j++)
        {
            _leds[position + j] = sides[i];
        }
    }

    // if (current_position == 1)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[0 + i] = getColorForSide(1, 255);
    //   }
    // }
    // else if (current_position == 2)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[9 + i] = getColorForSide(2, 255);
    //   }
    // }
    // else if (current_position == 3)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[18 + i] = getColorForSide(3, 255);
    //   }
    // }
    // else if (current_position == 4)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[27 + i] = getColorForSide(4, 255);
    //   }
    // }
    // else if (current_position == 5)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[36 + i] = getColorForSide(5, 255);
    //   }
    // }
    // if (current_position == 6)
    // {
    //   for (int i = 0; i < WIDTH * HEIGHT; i++)
    //   {
    //     leds[45 + i] = getColorForSide(6, 255);
    //   }
    // }
}

void LedFx::_snowRoutineEffect()
{
    // _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED;

    for (byte i = 0; i < NUM_LEDS; i++)
    {
        _leds[i] = random(0, 2) ? CRGB::White : CRGB::Black;
    }

    // _fader(70);
}

void LedFx::_matrixRoutineEffect()
{
    _palette = RainbowColors_p;
    _effectSpeed = EFFECT_SPEED;

    for (byte x = 0; x < WIDTH; x++)
    {
        // заполняем случайно верхнюю строку
        uint32_t thisColor = _getPixColorXY(x, HEIGHT - 1);
        if (thisColor == 0)
        {
            _drawPixelXY(x, HEIGHT - 1, 0x00FF00 * (random(0, EFFECT_SCALE) == 0));
        }
        else if (thisColor < 0x002000)
        {
            _drawPixelXY(x, HEIGHT - 1, 0);
        }
        else
        {
            _drawPixelXY(x, HEIGHT - 1, thisColor - 0x002000);
        }
    }

    // сдвигаем всё вниз
    for (byte x = 0; x < WIDTH; x++)
    {
        for (byte y = 0; y < HEIGHT - 1; y++)
        {
            _drawPixelXY(x, y, _getPixColorXY(x, y + 1));
        }
    }
}

void LedFx::_rainbowNoiseEffect()
{
    _palette = RainbowColors_p;
    _effectScale = 30;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 1;

    _fillNoiseLED();
}

void LedFx::_rainbowStripeNoiseEffect()
{
    _palette = RainbowStripeColors_p;
    _effectScale = 20;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 1;

    _fillNoiseLED();
}

void LedFx::_zebraNoiseEffect()
{

    // 'black out' all 16 palette entries...
    fill_solid(_palette, 16, CRGB::Black);
    // and set every fourth one to white.
    _palette[0] = CRGB::White;
    _palette[4] = CRGB::White;
    _palette[8] = CRGB::White;
    _palette[12] = CRGB::White;
    _effectScale = 30;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 1;

    _fillNoiseLED();
}

void LedFx::_forestNoiseEffect()
{
    _palette = ForestColors_p;
    _effectScale = 120;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 0;

    _fillNoiseLED();
}

void LedFx::_oceanNoiseEffect()
{

    _palette = OceanColors_p;
    _effectScale = 90;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 0;

    _fillNoiseLED();
}

void LedFx::_plasmaNoiseEffect()
{
    _palette = PartyColors_p;
    _effectScale = 30;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 1;

    _fillNoiseLED();
}

void LedFx::_cloudNoiseEffect()
{
    _palette = CloudColors_p;
    _effectScale = 30;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 0;

    _fillNoiseLED();
}

void LedFx::_lavaNoiseEffect()
{
    _palette = LavaColors_p;
    _effectScale = 50;
    _effectSpeed = EFFECT_SPEED;
    _effectColorLoop = 0;

    _fillNoiseLED();
}

// void LedFx::flashLights(const CRGB &color)
// {
//   for (size_t i = 0; i < 3; i++)
//   {
//     fill_solid(_leds, NUM_LEDS, color);
//     FastLED.show();
//     delay(100);
//     FastLED.clear();
//     FastLED.show();
//     delay(100);
//   }
// }

// utils
void LedFx::_fillAll(CRGB color)
{
    fill_solid(_leds, NUM_LEDS, color);
}

void LedFx::_fillFromPalette(uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for (int i = 0; i < NUM_LEDS; i++)
    {
        _leds[i] = ColorFromPalette(_palette, colorIndex, brightness, _blending);
        colorIndex += 3;
    }
}

void LedFx::_fillNoiseLED()
{
    static uint16_t x;
    static uint16_t y;
    static uint16_t z;

    uint8_t dataSmoothing = 0;
    if (EFFECT_SPEED < 50)
    {
        dataSmoothing = 200 - (EFFECT_SPEED * 4);
    }
    for (int i = 0; i < MAX_DIMENSION; i++)
    {
        int ioffset = _effectScale * i;
        for (int j = 0; j < MAX_DIMENSION; j++)
        {
            int joffset = _effectScale * j;

            uint8_t data = inoise8(x + ioffset, y + joffset, z);

            data = qsub8(data, 16);
            data = qadd8(data, scale8(data, 39));

            if (dataSmoothing)
            {
                uint8_t olddata = _effectNoise[i][j];
                uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
                data = newdata;
            }

            _effectNoise[i][j] = data;
        }
    }
    z += EFFECT_SPEED;

    // apply slow drift to X and Y, just for visual variation.
    x += EFFECT_SPEED / 8;
    y -= EFFECT_SPEED / 16;

    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            uint8_t index = _effectNoise[j][i];
            uint8_t bri = _effectNoise[i][j];
            // if this palette is a 'loop', add a slowly-changing base value
            if (_effectColorLoop)
            {
                index += _effectHue;
            }
            // brighten up, as the color palette itself often contains the
            // light/dark dynamic range desired
            if (bri > 127)
            {
                bri = 255;
            }
            else
            {
                bri = dim8_raw(bri * 2);
            }
            CRGB color = ColorFromPalette(_palette, index, bri);
            _drawPixelXY(i, j, color); // leds[getPixelNumber(i, j)] = color;
        }
    }

    _effectHue += 1;
}

CRGB LedFx::_getColorForSide(int s, int brigthness)
{
    CRGB c = 0;
    if (s == 1)
    {
        // синий
        c = CRGB(0, 0, brigthness);
    }
    else if (s == 2)
    {
        // желтый
        c = CRGB(brigthness, brigthness, 0);
    }
    else if (s == 3)
    {
        // красный
        c = CRGB(brigthness, 0, 0);
    }
    else if (s == 4)
    {
        // лиловый
        c = CRGB(brigthness, 0, brigthness);
    }
    else if (s == 5)
    {
        // зеленый
        c = CRGB(0, brigthness, 0);
    }
    else if (s == 6)
    {
        // белый
        c = CRGB(brigthness, brigthness, brigthness);
    }

    return c;
}

uint8_t LedFx::_getPixelAddr(uint8_t side, int x, int y)
{
    return _matrix[side][y][x];
}

// получить номер пикселя в ленте по координатам
uint8_t LedFx::_getPixelNumber(int8_t x, int8_t y)
{
    // uint8_t sideSize = WIDTH + HEIGHT;
    // uint8_t side = 1;
    // uint8_t sideX = x / 4;
    // uint8_t sideY = y / 3;

    // if (x < sideSize && y < sideSize) {
    //     side = 1;
    // } else {

    // }

    if (y % 2 == 0)
    { // если чётная строка
        return (y * WIDTH + x);
    }
    else
    { // если нечётная строка
        return (y * WIDTH + WIDTH - x - 1);
    }
}

// функция получения цвета пикселя по его номеру
uint32_t LedFx::_getPixelColor(int segment)
{
    int thisPixel = segment * SEGMENTS;
    if (thisPixel < 0 || thisPixel > NUM_LEDS - 1)
    {
        return 0;
    }

    return (((uint32_t)_leds[thisPixel].r << 16) | ((long)_leds[thisPixel].g << 8) | (long)_leds[thisPixel].b);
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t LedFx::_getPixColorXY(int8_t x, int8_t y)
{
    return _getPixelColor(_getPixelNumber(x, y));
}

// функция отрисовки точки по координатам X Y
void LedFx::_drawPixelXY(int8_t x, int8_t y, CRGB color)
{
    if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
    {
        return;
    }

    int thisPixel = _getPixelNumber(x, y) * SEGMENTS;
    for (byte i = 0; i < SEGMENTS; i++)
    {
        _leds[thisPixel + i] = color;
    }
}

void LedFx::_fadePixel(byte i, byte j, byte step)
{ // новый фейдер
    int pixelNum = _getPixelNumber(i, j);
    if (_getPixelColor(pixelNum) == 0)
    {
        return;
    }

    if (_leds[pixelNum].r >= 30 ||
        _leds[pixelNum].g >= 30 ||
        _leds[pixelNum].b >= 30)
    {
        _leds[pixelNum].fadeToBlackBy(step);
    }
    else
    {
        _leds[pixelNum] = 0;
    }
}

// функция плавного угасания цвета для всех пикселей
void LedFx::_fader(byte step)
{
    for (byte i = 0; i < WIDTH; i++)
    {
        for (byte j = 0; j < HEIGHT; j++)
        {
            _fadePixel(i, j, step);
        }
    }
}

void LedFx::_fade(byte step)
{
    for (byte i = 0; i < NUM_LEDS; i++)
    {
        if (_leds[i].r >= 30 ||
            _leds[i].g >= 30 ||
            _leds[i].b >= 30)
        {
            _leds[i].fadeToBlackBy(step);
        }
        else
        {
            _leds[i] = 0;
        }
    }
}