#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

uint8_t stateArrayMeasureRequest = 0;
uint8_t stateDisplayMeasureRequest = 0;
uint8_t stateMeasureAllowed = 0;


// Ändert den Stromversorgungsmodus, true entspricht Batteriestromversorgung, false entspricht button
void setSystemPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(PIN_MOSFET_SYSTEM_POWER, HIGH);
    }
    else {
        digitalWrite(PIN_MOSFET_SYSTEM_POWER, LOW);
    }
}

// Schaltet die Stromversorgung der Wasserpumpe und des Dachmotors, true = an, false = aus
void setMotorDriverPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(PIN_MOSFET_MOTOR_DRIVERS, HIGH);
        Serial.println("Motorstrom AN");
    }
    else {
        digitalWrite(PIN_MOSFET_MOTOR_DRIVERS, LOW);
        Serial.println("Motorstrom AUS");
    }
}

// Schaltet die Stromversorgung der analogen Sensoren, true = an, false = aus
void setAnalogSensorsPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(PIN_MOSFET_ANALOGS, HIGH);
    }
    else {
        digitalWrite(PIN_MOSFET_ANALOGS, LOW);
    }
}

// Ruft alle stromversorgungsverwaltenden Funktionen auf. Sollte in Zukunft auch Abschalten des gesamten Systems anhand von Batteriespannung beinhalten
void managePowerSupplies() {
    manageSensorPowerSupplies();
    manageMotorPowerSupplies();
}

// Checkt Requests an Sensoren und erteilt Erlaubnis
void manageSensorPowerSupplies() {
    static uint8_t previousArrayMeasureRequest;
    static uint8_t previousDisplayMeasureRequest;
    static uint32_t analogSensorsTurnOnTime;
    static bool hasRequests;

    if (previousArrayMeasureRequest != stateArrayMeasureRequest || previousDisplayMeasureRequest != stateDisplayMeasureRequest) {
        if (stateArrayMeasureRequest & 128 || stateDisplayMeasureRequest & 128) {
            stateMeasureAllowed |= 128;
        }
        else {
            stateMeasureAllowed &= ~128;
        }
        if (stateArrayMeasureRequest & 64 || stateDisplayMeasureRequest & 64) {
            stateMeasureAllowed |= 64;
        }
        else {
            stateMeasureAllowed &= ~64;
        }
        if (stateArrayMeasureRequest & 1 || stateDisplayMeasureRequest & 1) {
            stateMeasureAllowed |= 1;
        }
        else {
            stateMeasureAllowed &= ~1;
        }
        if (stateArrayMeasureRequest & 2 || stateDisplayMeasureRequest & 2 || stateArrayMeasureRequest & 4 || stateDisplayMeasureRequest & 4) {
            hasRequests = true;
            setAnalogSensorsPowerSupply(true);
            analogSensorsTurnOnTime = millis();
        }
        else {
            hasRequests = false;
            setAnalogSensorsPowerSupply(false);
        }
        previousArrayMeasureRequest = stateArrayMeasureRequest;
        previousDisplayMeasureRequest = stateDisplayMeasureRequest;
    }
    
    if (!hasRequests) {
        stateMeasureAllowed &= ~2;
        stateMeasureAllowed &= ~4;
    }
    else if (millis() - analogSensorsTurnOnTime >= ANALOG_SENSORS_PUFFER_TIME) {
        stateMeasureAllowed |= 2;
        stateMeasureAllowed |= 4;
    }
}

// Checkt Requests an Motoren und erteilt Erlaubnis
void manageMotorPowerSupplies() {
    static uint8_t previousMotorRequest;
    static uint32_t motorDriverTurnOnTime;
    static bool hasRequests;

    // Wenn Motortreiber nicht an und keine Änderung bzgl Motoranfrage
    if(!hasRequests && (previousMotorRequest == stateMotorRequest))
        return;

    // Wenn Motor verändert werden soll
    if (previousMotorRequest != stateMotorRequest) {
        if (stateMotorRequest & 1 || stateMotorRequest & 2) {
            hasRequests = true;
            setMotorDriverPowerSupply(true);
            motorDriverTurnOnTime = millis();
        }
        else {
            hasRequests = false;
            setMotorDriverPowerSupply(false);
        }
        previousMotorRequest = stateMotorRequest;
    }

    // Einschaltverzögerung aufgrund Pufferzeit
    if (hasRequests && (millis() - motorDriverTurnOnTime >= MOTOR_DRIVER_PUFFER_TIME)) {
        stateMotorAllowed |= 1;
        stateMotorAllowed |= 2;
    }
    // Zurücksetzen von stateMotorAllowed
    else if (!hasRequests) {
        stateMotorAllowed &= ~1;
        stateMotorAllowed &= ~2;
    }
}