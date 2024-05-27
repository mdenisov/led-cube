
#include "const.h"

// libs
#include <SparkFun_ADXL345.h>
#include <EEPROM.h>
#include "LedFx.h"

// instances
ADXL345 adxl = ADXL345();
LedFx ledFx = LedFx();

boolean active = true;
boolean saveMode = false;
boolean freeFall = false;
uint32_t restTmr = millis();

/* Maxtrix Cube
 *
 *
 *  02 01 00
 *  05 04 03
 *  08 07 06
 * ----------
 *  11 10 09 | 20 19 18 | 29 28 27 | 38 37 36
 *  14 13 12 | 23 22 21 | 32 31 30 | 41 40 39
 *  17 16 15 | 26 25 24 | 35 34 33 | 44 43 42
 *                                  ----------
 *                                   53 52 51
 *                                   50 49 48
 *                                   47 46 45
 */

uint8_t sideTop[WIDTH][HEIGHT] = {{2, 1, 0}, {5, 4, 3}, {8, 7, 6}};

uint8_t matrix[6][6][WIDTH][HEIGHT] = {
    {{2, 1, 0}, {5, 4, 3}, {8, 7, 6}}, {{11, 10, 9}, {14, 13, 12}, {17, 16, 15}}, {{20, 19, 18}, {23, 22, 21}, {26, 25, 24}}, {{29, 28, 27}, {32, 31, 30}, {35, 34, 33}}, {{38, 37, 36}, {41, 40, 39}, {44, 43, 42}}, {{53, 52, 51}, {50, 49, 48}, {47, 46, 45}}};

uint8_t getAddr(uint8_t sideUp, uint8_t side, int x, int y)
{
    return matrix[sideUp][side][x][y];
}

boolean inBetween(int value, int min, int max)
{
  return value > min && value < max;
}

int whichSideUp(int _x, int _y, int _z)
{
  int up = 0;
  if (inBetween(_x, -20, 20) && inBetween(_y, -20, 20) && _z < -30)
    up = 1;
  else if (_x > 30 && inBetween(_y, -20, 20) && inBetween(_z, -20, 20))
    up = 2;
  else if (inBetween(_x, -20, 20) && _y < -30 && inBetween(_z, -20, 20))
    up = 3;
  else if (_x < -30 && inBetween(_y, -20, 20) && inBetween(_z, -20, 20))
    up = 4;
  else if (inBetween(_x, -20, 20) && _y > 30 && inBetween(_z, -20, 20))
    up = 5;
  else if (inBetween(_x, -20, 20) && inBetween(_y, -20, 20) && _z > 30)
    up = 6;

  return up;
}

void setNextEffect()
{
  ledFx.setNextEffect();

  EEPROM.put(200, ledFx.effect);

  Serial.print("effect: ");
  Serial.println(ledFx.effect);
}

void ADXL_ISR()
{
  byte interrupts = adxl.getInterruptSource();

  if (adxl.triggered(interrupts, ADXL345_FREE_FALL))
  {
    Serial.println("*** FREE FALL ***");

    freeFall = true;
  }
  else
  {
    freeFall = false;
  }

  if (adxl.triggered(interrupts, ADXL345_INACTIVITY))
  {
    Serial.println("*** INACTIVITY ***");

    // activity = false;
  }

  if (adxl.triggered(interrupts, ADXL345_ACTIVITY))
  {
    Serial.println("*** ACTIVITY ***");

    // activity = true;
  }

  if (adxl.triggered(interrupts, ADXL345_DOUBLE_TAP))
  {
    Serial.println("*** DOUBLE TAP ***");

    setNextEffect();
  }

  if (adxl.triggered(interrupts, ADXL345_SINGLE_TAP))
  {
    Serial.println("*** TAP ***");
  }
}

// rest and power
void waitForRest(int &x, int &y, int &z)
{
  static int lastAccels[3];
  float restThreshold = 40;
  int accelDeltas[3] = {0, 0, 0};
  int accels[3] = {x, y, z};

  for (size_t i = 0; i < 3; i++)
  {
    accelDeltas[i] = fabs(accels[i] - lastAccels[i]);
  }

  // lastAccels = {x, y, z};
  lastAccels[0] = x;
  lastAccels[1] = y;
  lastAccels[2] = z;

  // Serial.printf("accels: %d, %d, %d", accels[0], accels[1], accels[2]);
  // Serial.println();
  // Serial.printf("accelDeltas: %d, %d, %d", accelDeltas[0], accelDeltas[1], accelDeltas[2]);
  // Serial.println();

  if (
      accelDeltas[0] < restThreshold &&
      accelDeltas[1] < restThreshold &&
      accelDeltas[2] < restThreshold)
  {
    if (active) {
      restTmr = millis();
    }

    active = false;
  }
  else
  {
    active = true;
  }
}

void setSaveMode(boolean mode)
{
  if (mode == saveMode)
  {
    return;
  }

  saveMode = mode;

  if (mode)
  {
    adxl.setLowPower(true);
    ledFx.disable();
  }
  else
  {
    adxl.setLowPower(false);
    ledFx.enable();
  }
}

void setFreeFallMode(boolean mode)
{
  static int prevEffect = -1;

  if (mode == freeFall)
  {
    return;
  }

  if (mode)
  {
    prevEffect = ledFx.effect;
    ledFx.setEffect(8);
  }
  else
  {
    if (prevEffect >= 0)
    {
      ledFx.setEffect(prevEffect);
      prevEffect = -1;
    }
  }
}

// main
void setup()
{
  Serial.begin(9600);
  Serial.println("Started");

  if (EEPROM.read(198) != 20)
  {
    EEPROM.put(198, 20);
    EEPROM.put(200, 0);
  }

  pinMode(LED_PIN, OUTPUT);

  ledFx.begin((uint8_t)EEPROM.read(200));

  adxl.powerOn();
  adxl.setRangeSetting(2);

  adxl.setActivityXYZ(1, 0, 0);
  adxl.setActivityThreshold(300);

  adxl.setInactivityXYZ(1, 0, 0);
  adxl.setInactivityThreshold(75);
  adxl.setTimeInactivity(5);

  adxl.setTapDetectionOnXYZ(1, 1, 1);
  adxl.setTapThreshold(50);
  adxl.setTapDuration(15);

  adxl.setDoubleTapLatency(80);
  adxl.setDoubleTapWindow(200);

  adxl.setFreeFallThreshold(7);
  adxl.setFreeFallDuration(30);

  adxl.InactivityINT(0);
  adxl.ActivityINT(0);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(0);
}

void loop()
{
  int x, y, z;
  adxl.readAccel(&x, &y, &z);
  // currentPosition = whichSideUp(x, y, z);
  waitForRest(x, y, z);
  setFreeFallMode(freeFall);

  if (active)
  {
    setSaveMode(false);
  }
  else
  {
    if (millis() - restTmr >= 10 * 1000)
    {
      restTmr = millis();
      setSaveMode(true);
    }
  }

  ledFx.loop();
  ADXL_ISR();
}