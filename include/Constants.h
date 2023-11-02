// Mosfet Hauptschalter: Stromversorgung des Systems 
#define PIN_MOSFET_SYSTEM_POWER 39
// Mosfet Stromversorgung Motortreiber
#define PIN_MOSFET_MOTOR_DRIVERS 41
// Mosfet Stromversorgung analoge Sensoren
#define PIN_MOSFET_ANALOGS 43

// Analoger Ausgang für Dachgeschwindigkeit
#define PIN_ROOF_SPEED 12

// Digitale Ausgänge für Dachrichtung
#define PIN_ROOF_CLOSING 33
#define PIN_ROOF_OPENING 31

// Digitaler Ausgang für Pumpenansteuerung
#define PIN_PUMP_CONTROL 35

// Analoger Ausgang für Pumpengeschwindigkeit
#define PIN_PUMP_SPEED 12

// Wartezeit bis zur ersten Messung der analogen Sensoren nach erster Stromversorgung
#define ANALOG_SENSORS_PUFFER_TIME 300
// Wartezeit bis zur ersten Motoraktion nach der ersten Stromversorgung des Motortreibers
#define MOTOR_DRIVER_PUFFER_TIME 300