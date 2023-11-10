// Mosfet Hauptschalter: Stromversorgung des Systems 
#define PIN_MOSFET_SYSTEM_POWER 39
// Mosfet Stromversorgung Motortreiber
#define PIN_MOSFET_MOTOR_DRIVERS 41
// Mosfet Stromversorgung analoge Sensoren
#define PIN_MOSFET_ANALOGS 43

// Pins Buttons/Endlagenschalter
#define PIN_UI_BUTTON_RIGHT 32
#define PIN_UI_BUTTON_LEFT 30
#define PIN_UI_BUTTON_TOP 28
#define PIN_UI_BUTTON_BOTTOM 26
#define PIN_ROOF_TOP_LIMIT_SWITCH 22
#define PIN_ROOF_BOTTOM_LIMIT_SWITCH 24
#define PIN_POTI A3


// Anschluss Spannungsmesser
#define PIN_SENSOR_BATTERY_VOLTAGE A0
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

// Zeitdauer, wie lange Messwerte aus der Vergangenheit für Durchschnittswerte miteinbezogen werden sollen
#define AVERAGE_VALUE_INCLUDE_TIME 1800000

// Zeitintervall zwischen Durchschnittsmessungen und tatsächlichen Messungen für Display
#define ARRAY_MEASURE_INTERVAL 10000
#define ACTUAL_MEASURE_INTERVAL 1000

// Eine Periodenlänge des Bewässerungsvorgangs
#define IRRIGATION_CYCLE_LENGTH 4000

// Parameter Widerstände
#define REFERENCE_VOLTAGE 5.04  //Versorgungsspannung Arduino
#define RESISTOR_R1 82000       //Ohm
#define RESISTOR_R2 22000       //Ohm
#define RESISTOR_R3 10000       //Ohm
#define NTC_RN   10000          //Widerstand NTC bei 25°
#define NTC_TN   25             //Temperatur, bei welcher der Widerstand gemessen wurde (25°)
#define NTC_B   3380            //Thermistorkonstante B von NTC


// Parameter Display
#define KEY_NONE 0
#define KEY_ENTER 1
#define KEY_NEXT 2
#define KEY_UP 3
#define KEY_DOWN 4
#define DISPLAY_PUFFER_ON_TIME 10000            //[ms]
#define DISPLAY_REDRAW_TIME_MEASURE 1000        //Zeit, wann Display aktualisiert werden soll wenn Seite mit aktuellen Messdaten angezeigt [ms]
#define DISPLAY_NUMBER_PAGES 6                  //Anzahl der Seiten
#define DISPLAY_NUMBER_LINES_2 2                //Anzahl der möglichen bearbeitbaren Zeilen Seite 2
#define DISPLAY_NUMBER_LINES_3 1                //Anzahl der möglichen bearbeitbaren Zeilen Seite 3
#define DISPLAY_NUMBER_LINES_6 0                //Anzahl der möglichen bearbeitbaren Zeilen Seite 6
