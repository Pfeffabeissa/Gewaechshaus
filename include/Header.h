#include <Constants.h>
#include <Arduino.h>
#include <Wire.h>
#include <BME280I2C.h>
#include <U8glib.h>

extern uint8_t stateArrayMeasureRequest;
extern uint8_t stateDisplayMeasureRequest;
extern uint8_t stateMeasureAllowed;

void manageSensorPowerSupplies();

void setSystemPowerSupply(bool newState);
void setAnalogSensorsPowerSupply(bool newState);
void setMotorDriverPowerSupply(bool newState);

float readBmeTemperature();
void regulateRoofMotor(int speed);
void regulatePump(int speed);
