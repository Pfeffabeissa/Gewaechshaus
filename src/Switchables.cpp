#include <Constants.h>
#include <Header.h>
#include <Arduino.h>


// Ändert den Stromversorgungsmodus, true entspricht Batteriestromversorgung, false entspricht button
void setSystemPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(MOSFET_K1, HIGH);
    }
    else {
        digitalWrite(MOSFET_K1, LOW);
    }
}

// Schaltet die Stromversorgung der Wasserpumpe und des Dachmotors, true = an, false = aus
void setMotorDriverPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(MOSFET_K2, HIGH);
    }
    else {
        digitalWrite(MOSFET_K2, LOW);
    }
}

// Schaltet die Stromversorgung der analogen Sensoren, true = an, false = aus
void setAnalogSensorsPowerSupply(bool newState) {
    if(newState) {
        digitalWrite(MOSFET_K3, HIGH);
    }
    else {
        digitalWrite(MOSFET_K3, LOW);
    }
}