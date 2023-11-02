#include <Constants.h>
#include <Arduino.h>
#include <Wire.h>
#include <BME280I2C.h>
#include <U8glib.h>

extern uint8_t stateArrayMeasureRequest;
extern uint8_t stateDisplayMeasureRequest;
extern uint8_t stateMeasureAllowed;
extern uint8_t stateMotorRequest;
extern uint8_t stateMotorAllowed;

extern float averageBatteryVoltage;
extern float averageTemperatureOutside;
extern float averageTemperatureInside;
extern float averageSoilMoisture;
extern float averageAirHumidity;

void managePowerSupplies();
void manageSensorPowerSupplies();
void manageMotorPowerSupplies();

void arrayMeasure();
bool checkArrayMeasureSchedule();
void setArrayMeasureRequests(bool isRequested);
bool isArrayMeasureAllowed();
void executeArrayMeasures();
float average(float array[], uint8_t amountOfMeasurements);

void setSystemPowerSupply(bool newState);
void setAnalogSensorsPowerSupply(bool newState);
void setMotorDriverPowerSupply(bool newState);

float readBmeTemperature();
float readBmeHumidity();
float readNtcTemperature();
float readSoilMoisture();
float readBatteryVoltage();
bool readButton(int pin);
bool readLimitSwitch(int pin);

void regulateRoofMotor(int speed);
void regulatePump(int speed);
