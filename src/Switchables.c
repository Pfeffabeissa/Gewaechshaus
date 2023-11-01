#include <Constants.h>
#include <Header.h>
#include <Arduino.h>


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
    }
    else {
        digitalWrite(PIN_MOSFET_MOTOR_DRIVERS, LOW);
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

// Checkt Requests an Sensoren und erteilt Erlaubnis
void managePowerSupplies() {
    static uint8_t previousArrayMeasureRequest;
    static uint8_t previousDisplayMeasureRequest;
    static uint32_t analogSensorsTurnOnTime;

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
        if (stateArrayMeasureRequest & 2 || stateDisplayMeasureRequest & 2 || stateArrayMeasureRequest & 4 || stateDisplayMeasureRequest & 4) {
            setAnalogSensorsPowerSupply(true);
            analogSensorsTurnOnTime = millis();
        }
        else {
            setAnalogSensorsPowerSupply(false);
            analogSensorsTurnOnTime = -1;
        }
        previousArrayMeasureRequest = stateArrayMeasureRequest;
        previousDisplayMeasureRequest = stateDisplayMeasureRequest;
    }
    
    if (millis() - analogSensorsTurnOnTime >= ANALOG_SENSORS_PUFFER_TIME) {
        stateMeasureAllowed |= 2;
        stateMeasureAllowed |= 4;
    }
    else if (analogSensorsTurnOnTime == -1) {
        stateMeasureAllowed &= ~2;
        stateMeasureAllowed &= ~4;
    }
}