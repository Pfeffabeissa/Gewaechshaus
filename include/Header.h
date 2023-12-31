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

extern float actualBatteryVoltage;
extern float actualTemperatureOutside;
extern float actualTemperatureInside;
extern float actualSoilMoisture;
extern float actualAirHumidity;

extern float targetSoilMoisture;

extern uint8_t targetRoofPosition;
extern uint8_t actualRoofPosition;
extern uint8_t requiredPumpCycles;      
extern bool isIrrigationRequired;
extern bool isPumpRunning;

void managePowerSupplies();
void manageSensorPowerSupplies();
void manageMotorPowerSupplies();

void arrayMeasure();
bool checkArrayMeasureSchedule();
void setArrayMeasureRequests(bool isRequested);
bool isArrayMeasureAllowed();
void executeArrayMeasures();
float average(float array[], uint8_t amountOfMeasurements);

void actualMeasure();
bool checkActualMeasureSchedule();
bool executeActualMeasures();

void setSystemPowerSupply(bool newState);
void setAnalogSensorsPowerSupply(bool newState);
void setMotorDriverPowerSupply(bool newState);

void initializeBme();
float readBmeTemperature();
float readBmeHumidity();
float readNtcTemperature();
float readSoilMoisture();
float readBatteryVoltage();
bool readButton(int pin);
bool readLimitSwitch(int pin);
int readPoti(int min, int max);

void manageMotors();
void manageRoofMotor();
void managePump();
int switchPump(bool pumpShouldStart = 0);
void regulateRoofMotor(int8_t speed);
void regulatePump(int speed);

void initializeDisplay();
void manageDisplayfunctions();
void switchDisplayOnOff();
void checkDisplayUserInput();
void setDisplayParameters();
void setRewdrawDisplayMeasure();
void setSettings();
void printDisplayPage();