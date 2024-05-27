#ifndef CONST_H
#define CONST_H

#define LED_PIN PIN_PB1

#define WIDTH 3    // ширина матрицы
#define HEIGHT 3   // высота матрицы
#define SEGMENTS 6 // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
#define NUM_LEDS WIDTH *HEIGHT * 6
#define BRIGHTNESS 30
#define MATRIX_TYPE 0 // тип матрицы: 0 - зигзаг, 1 - параллельная
#define MAX_VOLTS 3.3
#define MAX_MILLIAMPS 850
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

#define EFFECT_SCALE 100
#define EFFECT_SPEED 50
#define MAX_DIMENSION (max(WIDTH, HEIGHT))

#endif