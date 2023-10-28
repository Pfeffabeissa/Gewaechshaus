#include "Header.h"
#include "Constants.h"

// Ändert den Stromversorgungsmodus, true entspricht Batteriestromversorgung, false entspricht button
void setSystemPowerSupply(boolean newState) {
    if(newState) {
        digitalWrite(MOSFET_K1, HIGH);
    }
    else {
        digitalWrite(MOSFET_K1, LOW);
    }
}

// Schaltet die Stromversorgung der analogen Sensoren, true = an, false = aus
void setAnalogSensorsPowerSupply(boolean newState) {
    if(newState) {
        digitalWrite(MOSFET_K3, HIGH);
    }
    else {
        digitalWrite(MOSFET_K3, LOW);
    }
}