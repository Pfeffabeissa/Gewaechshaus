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