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
    float analogValue = (analogRead(PIN_SENSOR_NTC) / REFERENCE_VOLTAGE) * 1023;            // Einlesen der Spannung vom Spannungsteiler
    float rNtc = (RESISTOR_R3 / REFERENCE_VOLTAGE) * analogValue;                           // Widerstand vom NTC
    return (1 / ((log(rNtc/NTC_RN) / NTC_B) + 1 / (NTC_TN + 273.15)) - 273.15);             // Umrechnung von Rt = Rn * e hoch B*(1/T - 1/Tn)    
}

// Funktion Bodenfeuchte Gewächshaus 0 - 100%
float readSoilMoisture()
{
    float soilHumidity = analogRead(PIN_SENSOR_SOIL_HUMIDITY);         
    return map(soilHumidity, 0, 1023, 0, 100);                  //Feuchtigkeitswerte von 0% - 100%
}

// Funktion Batteriespannung messen
float readBatteryVoltage()
{
    float auxiliaryVariable = 0;   

    //Einlesen der Spannung und gemäß Spannungsteiler umrechnen
    auxiliaryVariable = (analogRead(PIN_SENSOR_BATTERY_VOLTAGE) * REFERENCE_VOLTAGE) / 1024;       
    return (auxiliaryVariable * (RESISTOR_R1 + RESISTOR_R2)) / RESISTOR_R2;           
}

// Funktion Taster lesen, return 1 wenn gedrückt
boolean readButton(int pin)
{
    if(digitalRead(pin) == LOW)				    //Prüft ob Taster gedrückt
	{
	    delay(10);							    //Wenn 10ms vergangen && Taster noch gedrückt --> übergibt 1
	    if(digitalRead(pin) == LOW) return 1;
	    else return 0;    
	}
	else return 0;								//Wenn nicht gedrückt, übergibt 0
}

// Funktion Endlagenschalter Dach, return 1 wenn betätigt
boolean readLimitSwitch(int pin)
{
    if(digitalRead(pin) == HIGH)				//Prüft ob Endlagenschalter betätigt ist
	{
		delay(10);							    //Wenn 10ms vergangen && Endlagenschalter immer noch betätigt --> übergibt 1
		if(digitalRead(pin) == HIGH) return 1;
		else return 0;    
	}
	else return 0;								//Wenn nicht betätigt, übergibt 0
}
