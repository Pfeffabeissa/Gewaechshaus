#include <Constants.h>
#include <Header.h>
#include <Arduino.h>
#include <BME280I2C.h>
#include <Wire.h>

BME280I2C bme;

// Funktion Temperatur
float readBmeTemperature()
{
    return bme.temp();  // in °C
}

// Funktion Luftfeuchte Gewächshaus
float readBmeHumidity()
{
    return bme.hum();
}

// Funktion Temperatur außerhalb Gewächshaus
float readNtcTemperature()
{
    float analogValue = (analogRead(PIN_NTC) / REFERENCE_VOLTAGE) * 1023;           // Einlesen der Spannung vom Spannungsteiler
    float rNtc = (RESISTOR_R3 / REFERENCE_VOLTAGE) * analogValue;                   // Widerstand vom NTC
    return (1 / ((log(rNtc/NTC_RN) / NTC_B) + 1 / (NTC_TN + 273.15)) - 273.15);     // Umrechnung von Rt = Rn * e hoch B*(1/T - 1/Tn)    
   
}