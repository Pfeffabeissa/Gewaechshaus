#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

// Steuert alle Pins verantwortlich für die Steuerung des Dachs an, positive Werte öffnen das Dach, negative Werte schließen das Dach, 0 hält an
void regulateRoofMotor(int speed) {
    if (speed > 0) {
        digitalWrite(PIN_ROOF_OPENING, HIGH);
        digitalWrite(PIN_ROOF_CLOSING, LOW);
        analogWrite(PIN_ROOF_SPEED, speed*2.55);
    }
    else if (speed < 0) {
        digitalWrite(PIN_ROOF_OPENING, LOW);
        digitalWrite(PIN_ROOF_CLOSING, HIGH);
        analogWrite(PIN_ROOF_SPEED, speed*2.55);
    }
    else {
        digitalWrite(PIN_ROOF_OPENING, LOW);
        digitalWrite(PIN_ROOF_CLOSING, LOW);
        analogWrite(PIN_ROOF_SPEED, 0);
    }
} 