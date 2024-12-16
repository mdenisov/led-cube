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

#include "const.h"

// libs
#include <SparkFun_ADXL345.h>
#include <EEPROM.h>
// #include <GyverPower.h>
#include "CubeFx.h"

// variables
boolean active = true;
boolean freeFall = false;
uint32_t activityTmr = millis();

// instances
ADXL345 adxl = ADXL345();
CubeFx cubeFx = CubeFx();

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
  if (freeFall)
  {
    return;
  }

  cubeFx.setNextEffect();

  EEPROM.put(200, cubeFx.effect);

  Serial.print("effect: ");
  Serial.println(cubeFx.effect);
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
void checkActivity(int &x, int &y, int &z)
{
  static int lastAccels[3];
  int restThreshold = 60;
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
      accelDeltas[1] < restThreshold
      // accelDeltas[2] < restThreshold
  )
  {
    active = false;
  }
  else
  {
    active = true;
  }
}

void setSaveMode(boolean mode)
{
  static uint32_t saveTmr;

  if (!mode)
  {
    saveTmr = millis();
    adxl.setLowPower(false);
    cubeFx.enable();

    return;
  }

  if (millis() - saveTmr < 10 * 1000)
  {
    return;
  }

  adxl.setLowPower(true);
  cubeFx.disable();
}

void setFreeFallMode(boolean mode)
{
  static uint32_t freeFallTmr;
  static boolean prevMode = false;
  static int prevEffect = -1;

  if (mode == prevMode)
  {
    return;
  }

  if (mode)
  {
    freeFallTmr = millis();
    prevMode = mode;
    prevEffect = cubeFx.effect;
    cubeFx.setEffect(15);

    return;
  }

  if (millis() - freeFallTmr < 1 * 1000)
  {
    return;
  }

  if (prevEffect >= 0)
  {
    cubeFx.setEffect(prevEffect);
    prevEffect = -1;
    prevMode = mode;
  }
}

// main
void setup()
{
  Serial.begin(9600);
  Serial.println("Started");

  // power.autoCalibrate();
  // power.correctMillis(true);
  // power.hardwareDisable(PWR_ADC | PWR_TIMER1 | PWR_TIMER2 | PWR_TIMER3 | PWR_TIMER4 | PWR_TIMER5 | PWR_UART1 | PWR_UART2 | PWR_UART3 | PWR_SPI | PWR_USB);

  if (EEPROM.read(198) != 20)
  {
    EEPROM.put(198, 20);
    EEPROM.put(200, 0);
  }

  cubeFx.begin((uint8_t)EEPROM.read(200));

  adxl.powerOn();
  adxl.setSpiBit(0);
  adxl.setRangeSetting(2);

  adxl.setActivityXYZ(1, 0, 0);
  adxl.setActivityThreshold(75);

  adxl.setInactivityXYZ(1, 0, 0);
  adxl.setInactivityThreshold(75);
  adxl.setTimeInactivity(10);

  adxl.setTapDetectionOnXYZ(0, 0, 1);
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
  ADXL_ISR();

  if (millis() - activityTmr >= 300)
  {
    activityTmr = millis();

    int x, y, z;
    adxl.readAccel(&x, &y, &z);

    // Serial.println(active);
    // Serial.println(cubeFx.brightness());

    // currentPosition = whichSideUp(x, y, z);
    checkActivity(x, y, z);
    setFreeFallMode(freeFall);
    setSaveMode(!active);
  }

  cubeFx.loop();

  // if (!cubeFx.enabled() && cubeFx.brightness() == 0) {
  //   power.sleepDelay(500);
  // }
}