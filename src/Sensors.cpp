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
    float analogValue = (analogRead(PIN_SENSOR_NTC) / REFERENCE_VOLTAGE) * 1023;           // Einlesen der Spannung vom Spannungsteiler
    float rNtc = (RESISTOR_R3 / REFERENCE_VOLTAGE) * analogValue;                   // Widerstand vom NTC
    return (1 / ((log(rNtc/NTC_RN) / NTC_B) + 1 / (NTC_TN + 273.15)) - 273.15);     // Umrechnung von Rt = Rn * e hoch B*(1/T - 1/Tn)    
}

// Funktion Bodenfeuchte Gewächshaus 0 - 100%
float readSoilMoisture()
{
    float soilHumidity = analogRead(PIN_SENSOR_SOIL_HUMIDITY);         
    return (soilHumidity, 0, 1023, 0, 100);                  //Feuchtigkeitswerte von 0% - 100%
}

// Funktion Batteriespannung messen
float readBatteryVoltage()
{
    float auxiliaryVariable = 0;    // Hilfsvariable zum Einlesen der Spannung

    auxiliaryVariable = (analogRead(PIN_SENSOR_BATTERY_VOLTAGE) * REFERENCE_VOLTAGE) / 1024;       //Einlesen der Spannung und gemäß Spannungsteiler umrechnen
    return (auxiliaryVariable * (RESISTOR_R1 + RESISTOR_R2)) / RESISTOR_R2;           
}
