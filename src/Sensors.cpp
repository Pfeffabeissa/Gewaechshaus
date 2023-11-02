#include <Constants.h>
#include <Header.h>
#include <Arduino.h>
#include <BME280I2C.h>
#include <Wire.h>

BME280I2C bme;


//return Temperatur in °C
float readBmeTemperature()
{
    return bme.temp();
}