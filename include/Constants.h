// Mosfet Hauptschalter: Stromversorgung des Systems 
#define PIN_MOSFET_SYSTEM_POWER 39
// Mosfet Stromversorgung Motortreiber
#define PIN_MOSFET_MOTOR_DRIVERS 41
// Mosfet Stromversorgung analoge Sensoren
#define PIN_MOSFET_ANALOGS 43

// Anschluss NTC
#define PIN_SENSOR_NTC A1
// Anschluss Bodenfeuchtesensor
#define PIN_SENSOR_SOIL_HUMIDITY A2



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


// Parameter Widerstände
#define REFERENCE_VOLTAGE 5.04  //Versorgungsspannung Arduino
#define RESISTOR_R1 82000       //Ohm
#define RESISTOR_R2 22000       //Ohm
#define RESISTOR_R3 10000       //Ohm
#define NTC_RN   10000          //Widerstand NTC bei 25°
#define NTC_TN   25             //Temperatur, bei welcher der Widerstand gemessen wurde (25°)
#define NTC_B   3380            //Thermistorkonstante B von NTC