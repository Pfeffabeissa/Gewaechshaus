#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

float averageBatteryVoltage;
float averageTemperatureOutside;
float averageTemperatureInside;
float averageSoilMoisture;
float averageAirHumidity;

void actualMeasure() {

}




// Ruft alle für Arraymessungen nötigen Funktionen auf
void arrayMeasure() {
    static bool isArrayMeasurePending = false;

    if (!isArrayMeasurePending && checkArrayMeasureSchedule()) {
        setArrayMeasureRequests(true);
        isArrayMeasurePending = true;
    }

    if (isArrayMeasurePending && isArrayMeasureAllowed()) {
        // Messungen hierher
        setArrayMeasureRequests(false);
        isArrayMeasurePending = false;
    }

}

// Überprüft, ob das Arraymessungszeitintervall abgelaufen ist
bool checkArrayMeasureSchedule() {
    static uint8_t nextArrayMeasureTime = 0;
    if (millis() >= nextArrayMeasureTime) {
        nextArrayMeasureTime += ARRAY_MEASURE_INTERVAL;
        return true;
    }
    else {
        return false;
    }
}

// Setzt in stateArrayMeasureRequests alle relevanten Bits auf den mitgebenen Wert
void setArrayMeasureRequests(bool isRequested) {
    if (isRequested) {
        stateArrayMeasureRequest |= 1;
        stateArrayMeasureRequest |= 2;
        stateArrayMeasureRequest |= 4;
        stateArrayMeasureRequest |= 64;
        stateArrayMeasureRequest |= 128;
    }
    else {
        stateArrayMeasureRequest &= ~1;
        stateArrayMeasureRequest &= ~2;
        stateArrayMeasureRequest &= ~4;
        stateArrayMeasureRequest &= ~64;
        stateArrayMeasureRequest &= ~128;
    }
}

// Checkt in stateMeasureAllowed, ob alle Bedingungen für eine Arraymessung gegeben sind
bool isArrayMeasureAllowed() {
    if (stateMeasureAllowed & 1 &&
        stateMeasureAllowed & 2 &&
        stateMeasureAllowed & 4 &&
        stateMeasureAllowed & 64 &&
        stateMeasureAllowed & 128) {
        return true;
        }
    else {
        return false;
    }
}