#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

// Ruft alle Funktionen für Motor und Pumpensteuerung auf
void manageMotors() {
    manageRoofMotor();
    managePump();
}

// Steuert Initialisierung, Start und Stopp der Dachbewegung
void manageRoofMotor() {
    static uint32_t expectedTargetPositionTime;
    static bool isDriving = false;
    if (targetRoofPosition != actualRoofPosition) {
        stateMotorRequest |= 1;
        if (stateMotorAllowed & 1) {
            if (!isDriving) {
                // Auffahren
                float deltaRope = 10 * abs((sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - targetRoofPosition * 0.85 * 3.14 / 180))) - sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - actualRoofPosition * 0.85 * 3.14 / 180)))));
                if (targetRoofPosition > actualRoofPosition) {
                    expectedTargetPositionTime = millis() + deltaRope / 796 / (0.45 * pow(averageBatteryVoltage, 2) - 13.81 * averageBatteryVoltage + 140.89) * 1000;
                    regulateRoofMotor(100);
                }
                // Zufahren
                else {
                    expectedTargetPositionTime = millis() + deltaRope / 796 / (0.32 * pow(averageBatteryVoltage, 2) - 11.12 * averageBatteryVoltage + 123.98) * 1000;
                    regulateRoofMotor(-100);
                }
                isDriving = true;
            }
        }
        else if (isDriving) {
            regulateRoofMotor(0);
        }
    }
    if (isDriving) {
        if (readLimitSwitch(PIN_ROOF_TOP_LIMIT_SWITCH)) {
            regulateRoofMotor(0);
            actualRoofPosition = 100;
            isDriving = false;
        }
        if (readLimitSwitch(PIN_ROOF_BOTTOM_LIMIT_SWITCH)) {
            regulateRoofMotor(0);
            actualRoofPosition = 0;
            isDriving = false;
        }
        if (targetRoofPosition != 0 && targetRoofPosition != 100 && millis() >= expectedTargetPositionTime) {
            regulateRoofMotor(0);
            actualRoofPosition = targetRoofPosition;
            isDriving = false;
        }
    }
}

// Steuert Ausführung des Beässerungszyklus bei manueller Anfrage
void managePump() {
    static bool isRunning = false;
    static uint32_t cycleStartTime = 0;
    if (!isRunning) {
        if (isIrrigationRequired) {
            isRunning = true;
            cycleStartTime = millis();
        }
    }
    else {
        if  (millis() - cycleStartTime <= IRRIGATION_CYCLE_LENGTH) {
            if (millis() - cycleStartTime <= IRRIGATION_CYCLE_LENGTH / 2) {
                regulatePump((millis() - cycleStartTime) / (IRRIGATION_CYCLE_LENGTH / 2));
            }
            else {
                regulatePump(100 - ((millis() - cycleStartTime) / IRRIGATION_CYCLE_LENGTH));
            }
        }
        else {
            regulatePump(0);
            isRunning = false;
        }
    }
}

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