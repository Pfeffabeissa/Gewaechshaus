#include <Constants.h>
#include <Header.h>
#include <Arduino.h>

uint8_t targetRoofPosition;
uint8_t actualRoofPosition;
uint8_t stateMotorRequest;
uint8_t stateMotorAllowed;
bool isPumpRunning;
bool isIrrigationRequired;

// Ruft alle Funktionen für Motor und Pumpensteuerung auf
void manageMotors() {
    manageRoofMotor();
    managePump();
}

// Steuert Initialisierung, Start und Stopp der Dachbewegung
void manageRoofMotor() {
    static uint32_t expectedTargetPositionTime;
    static bool isDriving = false;
    static bool ignoreLimitSwitches = false;

    if (targetRoofPosition != actualRoofPosition) {
        stateMotorRequest |= 1;
        if (stateMotorAllowed & 1) {
            if (!isDriving) {
                float deltaRope = 10 * abs((sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - targetRoofPosition * 0.85 * 3.14 / 180))) - sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - actualRoofPosition * 0.85 * 3.14 / 180)))));
                if (targetRoofPosition == 100) {
                    regulateRoofMotor(100);
                }
                else if (targetRoofPosition == 0) {
                    regulateRoofMotor(-100);
                }
                else if (targetRoofPosition > actualRoofPosition) {
                    expectedTargetPositionTime = millis() + deltaRope / (796 / (0.45 * pow(averageBatteryVoltage, 2) - 13.81 * averageBatteryVoltage + 140.89)) * 1000;
                    regulateRoofMotor(100);
                }
                else {
                    expectedTargetPositionTime = millis() + deltaRope / (796 / (0.32 * pow(averageBatteryVoltage, 2) - 11.12 * averageBatteryVoltage + 123.98)) * 1000;
                    regulateRoofMotor(-100);
                }
                if (readLimitSwitch(PIN_ROOF_TOP_LIMIT_SWITCH) || readLimitSwitch(PIN_ROOF_BOTTOM_LIMIT_SWITCH)) {
                    ignoreLimitSwitches = true;
                    analogWrite(7, 100);
                }
                isDriving = true;
            }
        }
        else if (isDriving) {
            regulateRoofMotor(0);
        }
    }
    if (ignoreLimitSwitches && !(readLimitSwitch(PIN_ROOF_TOP_LIMIT_SWITCH) || readLimitSwitch(PIN_ROOF_BOTTOM_LIMIT_SWITCH))) {
        ignoreLimitSwitches = false;
        analogWrite(7, 0);
    }

    if (isDriving) {
        if (!ignoreLimitSwitches) {
            if (readLimitSwitch(PIN_ROOF_TOP_LIMIT_SWITCH)) {
                regulateRoofMotor(0);
                actualRoofPosition = 100;
                isDriving = false;
                if (targetRoofPosition == 100) stateMotorRequest &= ~1;
                }
            if (readLimitSwitch(PIN_ROOF_BOTTOM_LIMIT_SWITCH)) {
                regulateRoofMotor(0);
                actualRoofPosition = 0;
                isDriving = false;
                if (targetRoofPosition == 0) stateMotorRequest &= ~1;
            }
        }
        if (millis() >= expectedTargetPositionTime && !(targetRoofPosition == 0 || targetRoofPosition == 100)) {
            regulateRoofMotor(0);
            actualRoofPosition = targetRoofPosition;
            isDriving = false;
            stateMotorRequest &= ~1;
        }
    }
}

// Steuert Ausführung des Beässerungszyklus bei manueller Anfrage
void managePump() {
    static uint32_t cycleStartTime = 0;
    if (!isPumpRunning) {
        if (isIrrigationRequired) {
            stateMotorRequest |= 2;
            if(stateMotorAllowed & 2)
            {
                isPumpRunning = true;
                Serial.println("Pumpe gestartet");
                cycleStartTime = millis();
            }
        }
    }
    else {
        if  (millis() - cycleStartTime <= IRRIGATION_CYCLE_LENGTH) {
            if (millis() - cycleStartTime <= (IRRIGATION_CYCLE_LENGTH / 2)) {
                regulatePump((millis() - cycleStartTime)*100 / (IRRIGATION_CYCLE_LENGTH / 2));          // (0...1) * 100 : enspricht Pumpengeschwindigkeit
                Serial.print("Pumpengeschwindigkeit: ");
                Serial.println((millis() - cycleStartTime)*100 / (IRRIGATION_CYCLE_LENGTH / 2));
            }
            else {
                regulatePump(100 - ((millis() - (cycleStartTime + IRRIGATION_CYCLE_LENGTH / 2))*100 / (IRRIGATION_CYCLE_LENGTH / 2)));
                Serial.print("Pumpengeschwindigkeit: ");
                Serial.println(100 - ((millis() - (cycleStartTime + IRRIGATION_CYCLE_LENGTH / 2))*100 / (IRRIGATION_CYCLE_LENGTH / 2)));
            }
        }
        else {
            regulatePump(0);
            isPumpRunning = false;
            stateMotorRequest &= ~2;
            Serial.println("Pumpe ausgeschaltet");
        }
    }
}

// Steuert alle Pins verantwortlich für die Steuerung des Dachs an
// ]0; 100] öffnen das Dach, [-100; 0[ schließen das Dach, 0 hält an
void regulateRoofMotor(int8_t speed) {
    if (speed > 0 && speed <= 100) {
        digitalWrite(PIN_ROOF_OPENING, HIGH);
        digitalWrite(PIN_ROOF_CLOSING, LOW);
        analogWrite(PIN_ROOF_SPEED, speed*2.55);
    }
    else if (speed < 0 && speed >= -100) {
        digitalWrite(PIN_ROOF_OPENING, LOW);
        digitalWrite(PIN_ROOF_CLOSING, HIGH);
        analogWrite(PIN_ROOF_SPEED, speed*-2.55);
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