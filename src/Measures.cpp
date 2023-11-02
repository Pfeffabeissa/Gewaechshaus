#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

float averageBatteryVoltage;
float averageTemperatureOutside;
float averageTemperatureInside;
float averageSoilMoisture;
float averageAirHumidity;

float actualBatteryVoltage;
float actualTemperatureOutside;
float actualTemperatureInside;
float actualSoilMoisture;
float actualAirHumidity;

// Ruft alle für Arraymessungen nötigen Funktionen auf
void arrayMeasure() {
    static bool isArrayMeasurePending = false;

    if (!isArrayMeasurePending && checkArrayMeasureSchedule()) {
        setArrayMeasureRequests(true);
        isArrayMeasurePending = true;
    }

    if (isArrayMeasurePending && isArrayMeasureAllowed()) {
        executeArrayMeasures();
        setArrayMeasureRequests(false);
        isArrayMeasurePending = false;
    }

}

// Ruft Messung solange auf, bis sie ausgeführt werden konnte
void actualMeasure() {
    static bool isActualMeasurePending = false;
    if (!isActualMeasurePending && stateDisplayMeasureRequest > 0) {
        isActualMeasurePending = true;
    }
    
    if (isActualMeasurePending) {
        if (executeActualMeasures()) isActualMeasurePending = false;
    }
}

// Ruft Messung solange auf, bis sie ausgeführt werden konnte
void actualMeasure() {
    static bool isActualMeasurePending = false;
    if (!isActualMeasurePending && stateDisplayMeasureRequest > 0) {
        isActualMeasurePending = true;
    }
    
    if (isActualMeasurePending) {
        if (executeActualMeasures()) isActualMeasurePending = false;
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

// Führt Messungen aus, speichert die Werte bis zum gewünschten Zeitpunkt ab, und setzt den Durchschnittswert in die globalen Variablen
void executeArrayMeasures() {
    static uint8_t currentIndex = 0;
    static uint8_t amountOfMeasurements = 0;
    static float batteryVoltageValues[(int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL];
    static float soilMoistureValues[(int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL];
    static float temperatureOutsideValues[(int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL];
    static float temperatureInsideValues[(int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL];
    static float airHumidityValues[(int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL];

    batteryVoltageValues[currentIndex] = readBatteryVoltage();
    soilMoistureValues[currentIndex] = readSoilMoisture();
    temperatureOutsideValues[currentIndex] = readNtcTemperature();
    temperatureInsideValues[currentIndex] = readBmeTemperature();
    airHumidityValues[currentIndex] = readBmeHumidity();

    if (currentIndex == amountOfMeasurements) {
        amountOfMeasurements++;
    }

    averageBatteryVoltage = average(batteryVoltageValues, amountOfMeasurements);
    averageSoilMoisture = average(soilMoistureValues, amountOfMeasurements);
    averageTemperatureOutside = average(temperatureOutsideValues, amountOfMeasurements);
    averageTemperatureInside = average(temperatureInsideValues, amountOfMeasurements);
    averageAirHumidity = average(airHumidityValues, amountOfMeasurements);

    if (currentIndex >= (int)AVERAGE_VALUE_INCLUDE_TIME/ARRAY_MEASURE_INTERVAL - 1) {
        currentIndex = 0;
    }
    else {
        currentIndex++;
    }
}

// Führt alle angeforderten tatsächlichen Messungen aus, returnt true wenn alle ausgeführt werden konnten
bool executeActualMeasures() {
    uint8_t counter = 0;
    if (stateDisplayMeasureRequest & 1) {
        if (stateMeasureAllowed & 1) {
            actualBatteryVoltage = readBatteryVoltage();
            counter++;
        }
    }
    else counter++;
    if (stateDisplayMeasureRequest & 2) {
        if (stateMeasureAllowed & 2) {
            actualTemperatureOutside = readNtcTemperature();
           counter++;
        }
    }
    else counter++;
    if (stateDisplayMeasureRequest & 4) {
        if (stateMeasureAllowed & 4) {
            actualTemperatureInside = readBmeTemperature();
            counter++;
        }
    }
    else counter++;
    if (stateDisplayMeasureRequest & 64) {
        if (stateMeasureAllowed & 64) {
            actualSoilMoisture = readSoilMoisture();
            counter++;
        }
    }
    else counter++;
    if (stateDisplayMeasureRequest & 128) {
        if (stateMeasureAllowed & 128) {
            actualAirHumidity = readBmeHumidity();
            counter++;
        }
    }
    else counter++;

    if (counter == 5) return true;
    else return false;
}

// Berechnet das arithmetische Mittel aus den ersten amoutOfMeasurements Werten im mitgegebenen Array
float average(float array[], uint8_t amountOfMeasurements) {
    float sum = 0;
    for (int i = 0; i < amountOfMeasurements; i++) {
        sum += array[i];
    }
    return sum/amountOfMeasurements;
}