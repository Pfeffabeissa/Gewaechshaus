#include <Constants.h>
#include <Arduino.h>
#include <Wire.h>
#include <BME280I2C.h>
#include <U8glib.h>

uint8_t stateArrayMeasureRequest = 0;
uint8_t stateDisplayMeasureRequest = 0;
uint8_t stateMeasureAllowed = 0;

void managePowerSupplies();

void setSystemPowerSupply(bool newState);
void setAnalogSensorsPowerSupply(bool newState);
void setMotorDriverPowerSupply(bool newState);

void regulateRoofMotor(int speed);
void regulatePump(int speed);