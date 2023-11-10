#include <Arduino.h>
#include <Header.h>
#include <BME280I2C.h>
#include <U8glib.h>
#include <Wire.h>
#include <Constants.h>


void setup() {
    pinMode(PIN_UI_BUTTON_BOTTOM, INPUT_PULLUP);
    pinMode(PIN_UI_BUTTON_LEFT, INPUT_PULLUP);
    pinMode(PIN_UI_BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(PIN_UI_BUTTON_TOP, INPUT_PULLUP);
    pinMode(PIN_ROOF_BOTTOM_LIMIT_SWITCH, INPUT_PULLUP);
    pinMode(PIN_ROOF_TOP_LIMIT_SWITCH, INPUT_PULLUP);
    pinMode(PIN_MOSFET_ANALOGS, OUTPUT);
    pinMode(PIN_MOSFET_MOTOR_DRIVERS, OUTPUT);
    pinMode(PIN_MOSFET_SYSTEM_POWER, OUTPUT);
    pinMode(45, OUTPUT);
    pinMode(PIN_ROOF_OPENING, OUTPUT);
    pinMode(PIN_ROOF_CLOSING, OUTPUT);
    pinMode(PIN_ROOF_SPEED, OUTPUT);
    pinMode(PIN_PUMP_CONTROL, OUTPUT);
    pinMode(PIN_PUMP_SPEED, OUTPUT);

    digitalWrite(PIN_MOSFET_SYSTEM_POWER, LOW);  //Wird gebraucht, da Spannung noch nicht gleich gemessen
    delay(300);
    Wire.begin();
    initializeBme();
    initializeDisplay();
    Serial.begin(9600);
}

void loop() {
    manageDisplayfunctions();
    arrayMeasure();
    actualMeasure();
    manageMotors();
    managePowerSupplies();
}