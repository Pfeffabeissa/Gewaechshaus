#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

// Steuert alle Pins verantwortlich für die Steuerung des Dachs an
// ]0; 100] öffnen das Dach, [-100; 0[ schließen das Dach, 0 hält an
void regulateRoofMotor(int speed) {
    if (speed > 0 && speed <= 100) {
        digitalWrite(PIN_ROOF_OPENING, HIGH);
        digitalWrite(PIN_ROOF_CLOSING, LOW);
        analogWrite(PIN_ROOF_SPEED, speed*2.55);
    }
    else if (speed < 0 && speed >= -100) {
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

// Steuert alle Pins verantwortlich für die Steuerung der Pumpe
// ]0; 100] regulieren die Geschwindigkeit, alles andere stoppt die Pumpe
void regulatePump(int speed) {
    if (speed > 0 && speed <= 100) {
        digitalWrite(PIN_PUMP_CONTROL, HIGH);
        analogWrite(PIN_PUMP_SPEED, speed*2.55);
    }
    else {
        digitalWrite(PIN_PUMP_CONTROL, LOW);
        analogWrite(PIN_PUMP_SPEED, 0);
    }
}