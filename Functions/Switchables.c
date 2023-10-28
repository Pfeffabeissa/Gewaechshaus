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