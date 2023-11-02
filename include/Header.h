#include <Constants.h>
#include <Arduino.h>


void setSystemPowerSupply(bool newState);
void setAnalogSensorsPowerSupply(bool newState);
void setMotorDriverPowerSupply(bool newState);

float readBmeTemperature(void);