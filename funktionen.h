#include <stdint.h>
#include "Arduino.h"
#include "U8glib.h"
#include <BME280I2C.h>
#include <Wire.h>

BME280I2C::Settings settings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_Off,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_Off,
  BME280::SpiEnable_False,
  BME280I2C::I2CAddr_0x76  // I2C address. I2C specific.
);
BME280I2C bme(settings);


U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI
                                             //Defines

//Parameter Widerstände
#define refVoltage 5.04  //Versorgungsspannung Arduino
#define r1 82000         //Ohm
#define r2 22000         //Ohm
#define r3 10000         //Ohm
#define r4_ntcRn 10000   //Widerstand NTC bei 25°
#define r4_ntcTn 25      //Temperatur, bei welcher der Widerstand gemessen wurde (25°)
#define r4_ntcB 3380     //Thermistorkonstante B von NTC
  //Input Sensoren
#define pinVoltage A0
#define pinNtc A1
#define pinHumidity A2
#define pinPoti A3
  //Input Taster
#define s1 22
#define s2 24
#define uiKeyDown 26
#define uiKeyUp 28
#define uiKeyNext 30
#define uiKeyEnter 32

#define KEY_NONE 0
#define KEY_ENTER 1
#define KEY_NEXT 2
#define KEY_UP 3
#define KEY_DOWN 4

//Input Motortreiber
#define IN1 31
#define IN2 33
#define IN4 35
#define ENA 12  //A12
#define ENB 13  //A13
  //Mosfets
#define k1 39
#define k2 41
#define k3 43
#define k4 45
//Festgelegte Parameter

#define MAX 31  //Größe von Sensorarrays

//Globale Variablen
uint16_t dataSensorAktor = 0;
float batteryVoltageActual = 0;
float batteryVoltage[MAX] = { 0 };
float tempOutActual = 0;
float tempOut[MAX] = { 0 };
float tempInActual = 0;
float tempIn[MAX] = { 0 };
float soilHumidityTarget = 0;
float soilHumidityActual = 0;
float soilHumidity[MAX] = { 0 };  //Evtl nicht gebraucht
float airHumidityActual = 0;
float airHumidity[MAX] = { 0 };

uint8_t actual_roofPosition = 0;  //ISTwert Dachposition, 0 - 100%
uint8_t target_roofPosition = 0;  //SOLLwert Dachposition 0 - 100%
float VoltageLowerLimit = 11;


//Funktionsprototypen-Deklaration
void comparison(void);
void uiStep();
int buttonRead(int);
int limitSwitch(int);
int poti(int, int);
int soilHumiditySensor();
float voltage();
float ntcSensor();
void roof();
void irrigation();
void timeMeasurement();
float average(float *, uint8_t);

//
void anzeige() {
  /*  Serial.print("Batteriespannung: ");
  Serial.println(batteryVoltage[0]);

  Serial.println(dataSensorAktor & 1024);
  */
}
void firstSetup() {


  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(uiKeyEnter, INPUT_PULLUP);
  pinMode(uiKeyNext, INPUT_PULLUP);
  pinMode(uiKeyUp, INPUT_PULLUP);
  pinMode(uiKeyDown, INPUT_PULLUP);
  pinMode(k1, OUTPUT);
  pinMode(k2, OUTPUT);
  pinMode(k3, OUTPUT);
  pinMode(k4, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  digitalWrite(k1, HIGH);  //Wird gebraucht, da Spannung noch nicht gleich gemessen

  dataSensorAktor &= 1;
  delay(100);
  Wire.begin();
  bme.begin();

  digitalWrite(k4, HIGH);
}
//FUNKTION comparison --------------------------------------------------------------------------------------------
void comparison(void) {
  //Wenn Spannung zu niedrig, k1 auf LOW setzen
  /*  if(batteryVoltage[0] < VoltageLowerLimit) 
  {
  /*  digitalWrite(k1, LOW);   
    dataSensorAktor &= ~1;     
  }           */
  //Display
  uiStep();
  //Sensoren auslesen: normal und Displayspezifisch
  timeMeasurement();
  //Bewässerung, Entscheidung in Funktion ob Bewässerung
  //  irrigation();
  //Dach
  if (abs(target_roofPosition - actual_roofPosition) > 0.1) roof();
}
//Funktion Display / Settings -------------------------------------------------------------------------------------
void uiStep() {
#define DISPLAYTIME 25000         //Ausschaltzeit Display [ms]
#define REDRAW_TIME_MEASURE 1000  //Zeit, wann Display aktualisiert werden soll wenn Seite mit aktuellen Messdaten angezeigt [ms]
#define NUMBER_PAGES 6            //Anzahl der Seiten
#define NUMBER_LINES_2 2          //Anzahl der möglichen bearbeitbaren Zeilen Seite 2
#define NUMBER_LINES_3 1          //Anzahl der möglichen bearbeitbaren Zeilen Seite 3
#define NUMBER_LINES_6 0          //Anzahl der möglichen bearbeitbaren Zeilen Seite 6

  static uint32_t lastButtonTime = 0;
  static uint32_t auxTimeMeasure = 0;  //Zeit für Messungen
  static uint32_t potiValue = 0;
  static uint8_t target_roofPositionTemporary = 0;  //Noch nicht festgelegte Dachposition (nur für Display behilfsmäßig)
  static uint8_t displayPage = 1;                   //angezeigte Seite auf Display(Start bei Seite 1)
  static uint8_t displayPageBefore = 0;
  static uint8_t displayLine = 0;  //DropDown Menü Dispay
  static uint8_t settingsOn = 0;   //Wenn ENTER gedrückt, Wert kann verändert werden; 1: Wert kann verändert werden, 2: Wert wurde bestätigt und aus Eingabe wurde wiederherausgesprungen
  static uint8_t uiKeyCodeFirst = KEY_NONE;
  static uint8_t uiKeyCodeLast = KEY_ENTER;
  static uint8_t uiKeyCode = KEY_NONE;
  static uint8_t redraw_required = 0;





  //Einschalten
  if (buttonRead(uiKeyEnter) && !lastButtonTime)  //Wenn ENTER gedrückt und lastButtonTime == 0 (noch keine Zeit gespeichert)
  {

    Serial.println("Einschaltvorgang Display");

    digitalWrite(k4, LOW);
    delay(100);
    lastButtonTime = millis();
    displayPage = 1;  //Alle Parameter zurücksetzen
    displayLine = 0;
    settingsOn = 0;
    uiKeyCodeFirst = KEY_NONE;
    uiKeyCodeLast = KEY_ENTER;
    uiKeyCode = KEY_NONE;
    dataSensorAktor |= 512;  //Globale Variable Bit 9 setzen (Display AN)
    redraw_required = 1;
    return;
  }


  //Ausschalten
  if (((millis() - lastButtonTime) > DISPLAYTIME) && !(dataSensorAktor & 256) && dataSensorAktor & 512)  //Wenn Display länger als DISPLAYTIME an und kein Taster mehr gedrückt
  {
    Serial.println("Ausschaltvorgang Display");

    u8g.firstPage();
    do {
    } while (u8g.nextPage());

    digitalWrite(k4, HIGH);
    dataSensorAktor &= ~512;  //Globale Variable Bit 9 löschen (Display AUS);
    dataSensorAktor &= ~1024;
    lastButtonTime = 0;
    return;
  }
  //Sensorabfrage
  if (dataSensorAktor & 512)  //Wenn Bit 9 TRUE
  {
    if (buttonRead(uiKeyEnter) == HIGH)
      uiKeyCodeFirst = KEY_ENTER;
    else if (buttonRead(uiKeyNext) == HIGH)
      uiKeyCodeFirst = KEY_NEXT;
    else if (buttonRead(uiKeyDown) == HIGH)
      uiKeyCodeFirst = KEY_DOWN;
    else if (buttonRead(uiKeyUp) == HIGH)
      uiKeyCodeFirst = KEY_UP;
    else
      uiKeyCodeFirst = KEY_NONE;

    if (uiKeyCodeLast != uiKeyCodeFirst) {
      uiKeyCode = uiKeyCodeFirst;
      lastButtonTime = millis();  //Aktuelle Tastendruckzeit speichern
    } else
      uiKeyCode = KEY_NONE;

    uiKeyCodeLast = uiKeyCodeFirst;
  }


  //Parameter für Page und Line
  if ((dataSensorAktor & 512) && (uiKeyCode == uiKeyCodeFirst))  //Wenn Bit 9 && uiKeyCode == uiKeyCodeFirst
  {
    switch (uiKeyCode) {
      case KEY_NEXT:
        displayPage++;  //mit 1 initialisiert
        settingsOn = 0;
        displayLine = 0;
        if (displayPage > NUMBER_PAGES) {
          displayPage = 1;
        }
        redraw_required = 1;
        break;

      case KEY_DOWN:
        if (!settingsOn) {
          displayLine++;
          if (displayPage == 2 && displayLine > NUMBER_LINES_2)  //Wenn auf Seite 2
            displayLine = 1;
          if (displayPage == 3 && displayLine > NUMBER_LINES_3)  //Wenn auf Seite 3
            displayLine = 1;
        }

        redraw_required = 1;
        break;

      case KEY_UP:
        if (!settingsOn) {
          displayLine--;
          if (displayPage == 2 && displayLine < 1)  //Wenn auf Seite 2
            displayLine = NUMBER_LINES_2;
          if (displayPage == 3 && displayLine < 1)  //Wenn auf Seite 3
            displayLine = NUMBER_LINES_3;
        }

        redraw_required = 1;
        break;

      case KEY_ENTER:
        if (settingsOn == 0 && displayLine != 0)
          settingsOn = 1;
        else if (settingsOn == 1) {
          settingsOn = 2;
        }

        redraw_required = 1;
        break;
    }
  }

  if (dataSensorAktor & 512) {

    switch (displayPage) {

      case 1:
        dataSensorAktor |= 1024;
        break;

      case 2:
        dataSensorAktor |= 1024;
        break;

      case 3:
        dataSensorAktor &= ~1024;
        break;

      case 4:
        dataSensorAktor |= 1024;
        break;

      case 5:
        dataSensorAktor |= 1024;
        break;

      case 6:
        dataSensorAktor &= ~1024;
        break;

      default:
        dataSensorAktor &= ~1024;
    }
  }

  //Abhängig von Seite sollen Messungen erfolgen --> jede Sekunde erneute Aktualisierung vom Display
  if ((dataSensorAktor & 1024) && (millis() >= (auxTimeMeasure + REDRAW_TIME_MEASURE))) {
    redraw_required = 1;
    auxTimeMeasure = millis();
  }

  //Wenn Werte bearbeitbar, Werte verändern
  if (settingsOn) {
    switch (displayPage) {
      case 1:  //Seite 1
        break;

      case 2:  //Seite 2
        if (displayLine == 1)
          soilHumidityTarget = poti(0, 65);

        if ((displayLine == 2) && ((uiKeyCode == KEY_DOWN) || (uiKeyCode == KEY_UP)))  //Wenn Pumpe an
        {
          dataSensorAktor = dataSensorAktor ^ 256;  //Pumpe togglen
        }

        redraw_required = 1;
        break;

      case 3:  //Seite 3
        if (displayLine == 1) {
          target_roofPositionTemporary = poti(0, 100);
          if (settingsOn == 2) {
            target_roofPosition = target_roofPositionTemporary;
            settingsOn = 1;
          }
        }

        redraw_required = 1;
        break;

      case 4:
        break;

      case 5:
        break;

      case 6:
        //NOCH FERTIG MACHEN
        break;
    }
  }

  //Seite 1 Temperatur
  if ((displayPage == 1) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "TEMPERATUR");  //Temperatur
      u8g.setPrintPos(13, 12);
      u8g.print("Innen: ");
      u8g.print(tempInActual);
      u8g.write(0xb0);
      u8g.print("C");
      u8g.setPrintPos(13, 23);
      u8g.print("Aussen: ");
      u8g.print(tempOutActual);
      u8g.write(0xb0);
      u8g.print("C");

      u8g.drawStr(3, 40, "LUFTFEUCHTIGKEIT");  //Luftfeuchtigkeit
      u8g.setPrintPos(13, 52);
      u8g.print("Innen: ");
      u8g.print(airHumidityActual);
      u8g.print("%");

    } while (u8g.nextPage());
  }


  //Seite 2 Bewässerung
  if ((displayPage == 2) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "BODENFEUCHTIGKEIT");
      u8g.drawStr(3, 43, "BEWAESSERUNG:");

      u8g.setFont(u8g_font_profont12);
      u8g.setFontPosTop();
      u8g.setPrintPos(13, 12);
      u8g.print("IST-WERT: ");
      if (soilHumidityActual >= 10) u8g.setPrintPos(88, 12);  //Wenn zwei Zahlen vorm Komma
      else u8g.setPrintPos(94, 12);                           //Wenn eine Zahl vorm Komma
      u8g.print(soilHumidityActual);
      u8g.print("%");
      u8g.setPrintPos(13, 23);
      u8g.print("SOLL-WERT: ");

      if (displayLine == 1)  //NOCH FERIGSTELLEN !!!       //Zum Menü durchswitchen -> wenn Taster gedrückt, weiße Box erscheint
      {
        u8g.drawBox(85, 24, u8g.getStrWidth("60.00%") + 6, 10);
        u8g.setDefaultBackgroundColor();
        if (soilHumidityTarget >= 10) u8g.setPrintPos(88, 23);  //Wenn zwei Zahlen vorm Komma
        else u8g.setPrintPos(94, 23);                           //Wenn eine Zahl vorm Komma
        u8g.print(soilHumidityTarget);
        u8g.print("%");
        u8g.setDefaultForegroundColor();
      } else  //Wenn aktuell nicht ausgewählt
      {
        if (soilHumidityTarget >= 10) u8g.setPrintPos(88, 23);  //Wenn zwei Zahlen vorm Komma
        else u8g.setPrintPos(94, 23);                           //Wenn eine Zahl vorm Komma
        u8g.print(soilHumidityTarget);
        u8g.print("%");
      }


      //Bewässerung ON/OFF

      if (displayLine == 2)  //NOCH FERIGSTELLEN !!!    //Zum Menü durchswitchen -> wenn Taster gedrückt, weiße Box erscheint
      {
        u8g.drawBox(101, 44, u8g.getStrWidth("OFF") + 8, 14);
        u8g.setDefaultBackgroundColor();
        if (dataSensorAktor & 256)  //Wenn Pumpe an -> ON auf Display (Farbe negiert)
        {
          u8g.setPrintPos(108, 45);
          u8g.print("ON");
        } else  //Wenn Pumpe aus -> OFF auf Display (Farbe negiert)
        {
          u8g.setPrintPos(104, 45);
          u8g.print("OFF");
        }
        u8g.setDefaultForegroundColor();
      } else  //Wenn aktuell nicht ausgewählt
      {
        u8g.drawFrame(101, 44, u8g.getStrWidth("OFF") + 8, 14);
        if (dataSensorAktor & 256)  //Wenn Pumpe an -> ON auf Display (Farbe normal)
        {
          u8g.setPrintPos(108, 45);
          u8g.print("ON");
        } else  //Wenn Pumpe aus -> OFF auf Display (Farbe normal)
        {
          u8g.setPrintPos(104, 45);
          u8g.print("OFF");
        }
      }
    } while (u8g.nextPage());
  }

  //Seite 3 Dach
  if ((displayPage == 3) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;

    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "DACH");

      u8g.setFont(u8g_font_profont12);
      u8g.setFontPosTop();
      u8g.setPrintPos(13, 12);
      u8g.print("IST-WERT: ");
      if (actual_roofPosition == 100) u8g.setPrintPos(88, 12);  //Wenn zwei Zahlen vorm Komma
      else if (actual_roofPosition >= 10) u8g.setPrintPos(94, 12);
      else u8g.setPrintPos(100, 12);  //Wenn eine Zahl vorm Komma
      u8g.print(actual_roofPosition);
      u8g.print("%");
      u8g.setPrintPos(13, 23);
      u8g.print("SOLL-WERT: ");

      if (displayLine == 1)  //NOCH FERIGSTELLEN !!!       //Zum Menü durchswitchen -> wenn Taster gedrückt, weiße Box erscheint
      {
        u8g.drawBox(85, 24, u8g.getStrWidth("100%") + 6, 10);
        u8g.setDefaultBackgroundColor();
        if (target_roofPositionTemporary >= 100) u8g.setPrintPos(88, 23);      //Wenn drei Zahlen vorm Komma
        else if (target_roofPositionTemporary >= 10) u8g.setPrintPos(94, 23);  //Wenn zwei Zahlen vorm Komma
        else u8g.setPrintPos(100, 23);                                         //Wenn eine Zahl vorm Komma
        u8g.print(target_roofPositionTemporary);
        u8g.print("%");
        u8g.setDefaultForegroundColor();
      } else  //Wenn aktuell nicht ausgewählt
      {
        if (target_roofPositionTemporary >= 100) u8g.setPrintPos(88, 23);      //Wenn zwei Zahlen vorm Komma
        else if (target_roofPositionTemporary >= 10) u8g.setPrintPos(94, 23);  //Wenn zwei Zahlen vorm Komma
        else u8g.setPrintPos(100, 23);                                         //Wenn eine Zahl vorm Komma
        if (target_roofPositionTemporary != 100) u8g.print(target_roofPositionTemporary);
        else u8g.print("100");
        u8g.print("%");
      }
    } while (u8g.nextPage());
  }



  //Seite 4 Prozessdaten
  if ((displayPage == 4) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "PROZESSDATEN");
      u8g.setPrintPos(13, 12);
      u8g.print("Spannung: ");
      u8g.print(batteryVoltageActual);
      u8g.print("V");
      u8g.setPrintPos(13, 23);
    } while (u8g.nextPage());
  }

  //Seite 5 Mittelwerte
  if ((displayPage == 5) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "MITTELWERTE 30 Minuten");
      u8g.setFont(u8g_font_profont12);
      u8g.setPrintPos(10, 22);
      u8g.print("Spannung: ");
      u8g.print(batteryVoltage[0]);
      u8g.print("V");
      u8g.setPrintPos(10, 32);
      u8g.print("Temp. A: ");
      u8g.print(tempOut[0]);
      u8g.print(0xb0);
      u8g.print("C");

      /*       u8g.setPrintPos(10, 23);
        u8g.print("Temp. innen: ");
        u8g.print(tempIn[0]);
        u8g.print(0xb0);
        u8g.print("C");
  */
      u8g.setPrintPos(10, 42);
      u8g.print("Bodenfeuchte: ");
      u8g.print(soilHumidity[0]);
      u8g.print("%");

      u8g.setPrintPos(10, 52);
      u8g.print("Luftfeuchte: ");
      u8g.print(airHumidity[0]);
      u8g.print("%");
    } while (u8g.nextPage());
  }

  //Seite 6 Einstellungen     DISPLAYSETTINGS NOCH FERTIG MACHEN
  if ((displayPage == 6) && (dataSensorAktor & 512) && redraw_required) {
    u8g.firstPage();
    redraw_required = 0;
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setFontPosTop();
      u8g.drawStr(3, 0, "EINSTELLUNGEN");
    } while (u8g.nextPage());
  }
}
//------------------------------------------------------------------------------------------------------------------
//Funktion Sensoren messen
void timeMeasurement() {
  #define Array_Measure_Interval 20000
  #define Actual_Measure_Interval 1000
  static uint8_t i = 1;
  static uint32_t lastMeasureArray = 0;
  static uint32_t lastMeasureActual = 0;
  static uint8_t MeasureRequest = 0;  //Bit 1 für ArrayMeasure, Bit 2 für ActualMeasure
  static uint32_t switchOnTime_k3 = 0;

  if (lastMeasureArray + Array_Measure_Interval < millis()) {  //Array Measure nötig?
    MeasureRequest |= 1;                                       //True => Array Measure wird requested
  } else {
    MeasureRequest &= ~1;  //False => Array Measure wird nicht requested
  }
  if (dataSensorAktor & 1024 && lastMeasureActual + Actual_Measure_Interval < millis()) {  //Actual Measure nötig?
    MeasureRequest |= 2;                                                                   //True => Actual Measure wird requested
  } else {
    MeasureRequest &= ~2;  //False => Actual Measure wird nicht requested
  }

  if (MeasureRequest & 1 || MeasureRequest & 2) {  //Wird k3 requested?

    if (!(dataSensorAktor & 2)) {  //Muss Einschaltvorgang eingeleitet werden?
      Serial.println("Einschaltvorgang Messung");
      digitalWrite(k3, HIGH);
      dataSensorAktor |= 2;
      switchOnTime_k3 = millis();
      Wire.begin();
    }

  } else if (dataSensorAktor & 2 && !(dataSensorAktor & 1024)) {  //Kann Ausschaltvorgang eingeleitet werden?
    Serial.println("Ausschaltvorgang Messung");
    digitalWrite(k3, LOW);
    dataSensorAktor &= ~2;
    Wire.end();
  }

  if (switchOnTime_k3 + 300 < millis()) {  //k3 Einschaltzeit vorbei?
    if (MeasureRequest & 1) {  //wird Array Measure angefordert?
      digitalWrite(k4, HIGH);
      bme.begin();

      Serial.println("Array Messung ");

      batteryVoltage[i] = voltage();
      tempOut[i] = ntcSensor();
      tempIn[i] = bme.temp();
      Serial.println("Temperaturmessung durchgeführt");
      soilHumidity[i] = soilHumiditySensor();
      airHumidity[i] = bme.hum();
      Serial.println("Luftfeuchtigkeitsmessung durchgeführt");
      batteryVoltage[0] = average(&batteryVoltage[0], i);
      tempOut[0] = average(&tempOut[0], i);
      tempIn[0] = average(&tempIn[0], i);
      soilHumidity[0] = average(&soilHumidity[0], i);
      airHumidity[0] = average(&airHumidity[0], i);

      Serial.println("Array Measure durchgeführt");

      lastMeasureArray = millis();
      i++;                 //Zähler um eins erhöht (max 30)
      if (i >= 31) i = 1;  //Wenn Zähler größer als 31 -> auf 0 setzen (eigentlich 30, wegen n++ aber 31)

      Serial.println("Array Messung fertig");
    }
    if (MeasureRequest & 2) {  //wird Actual Measure angefordert?
      bme.begin();

      batteryVoltageActual = voltage();
      tempOutActual = ntcSensor();
      tempInActual = bme.temp();
      soilHumidityActual = soilHumiditySensor();
      airHumidityActual = bme.hum();

      Serial.println("Actual Measure durchgeführt");

      lastMeasureActual = millis();
    }
  }
}

//Funktion Durchschnitt berechnen     FERTIG
float average(float array[], uint8_t i) {
  // static uint8_t n = i;
  float summe = 0;

  if (millis() <= 1800000)  //Wenn kleiner als 30min
  {
    for (int k = 1; k <= i; k++) {
      summe = summe + array[k];
    }
    return (summe / i);  //Summe / bereits gemessene Werte
  } else                 //Normaler Durchlauf ab 30min, wenn bereits jedes Array einmal beschrieben wurde
  {
    for (int k = 1; k < MAX; k++) summe += array[k];
    return (summe / (MAX - 1));  //Summe / MAX(31-1)
  }
}
//Funktion Taster
int buttonRead(int pin) {
  if (digitalRead(pin) == false)  //Prüft ob Taster gedrückt
  {
    delay(10);  //Wenn 10ms vergangen && Taster noch gedrückt --> übergibt 1
    if (digitalRead(pin) == false) return 1;
    else return 0;
  } else return 0;  //Wenn nicht gedrückt, übergibt 0
}
//Funktion Endlagesensor
int limitSwitch(int pin) {
  if (digitalRead(pin) == HIGH)  //Prüft ob Sensor gedrückt ist
  {
    delay(10);  //Wenn 10ms vergangen && Taster immer noch gedrückt --> übergibt 0
    if (digitalRead(pin) == HIGH) return 0;
    else return 1;
  } else return 1;  //Wenn nicht gedrückt, übergibt 1
}
//Funktion Potentiometer
int poti(int min, int max) {  // Maximaler und minimaler Bereich als Übergabeparameter
  int poti_wert = 0;

  poti_wert = analogRead(pinPoti);
  poti_wert = map(poti_wert, 0, 1020, min, max);  //normal 1023, aber sonst display bei 100% springt
  return poti_wert;
}
//Funktion Temperatur- und Feuchtigkeitssensoren IM GEWÄCHSHAUS einschalten, Messdaten abfragen
int soilHumiditySensor() {
  float soilHumidity = analogRead(pinHumidity);       //K3 wird global eingeschaltet
  soilHumidity = map(soilHumidity, 0, 1023, 0, 100);  //Feuchtigkeitswerte von 0% - 100%

  return soilHumidity;
}
//Funktion Batteriespannung messen
float voltage() {
  float analogValue = 0;  // Hilfsvariable zum Einlesen der Spannung
  float temp1 = 0;
  float temp2 = 0;

  analogValue = analogRead(A0);  //Einlesen der Spannung vom Spannungsteiler
  temp1 = (analogValue * refVoltage) / 1024;
  temp2 = (temp1 * (r1 + r2)) / r2;
  return temp2;  //Gemäß Spannungsteiler umrechnen und zurückgeben
}
//Funktion Temperatur NTC
float ntcSensor() {
  float analogVoltage = (analogRead(pinNtc) * refVoltage) / 1023;                                         //Einlesen der Spannung vom Spannungsteiler
  float rNtc = analogVoltage / (refVoltage - analogVoltage) * r3;                                         //Widerstand vom NTC
  float temperature = (1 / ((log(rNtc / r4_ntcRn) / r4_ntcB) + 1 / (r4_ntcTn + 273.15))) - 273.15 - 1.2;  // - 1.2 bereinigt Ungenauigkeit

  return temperature;
}
//Funktion Motorsteuerung Dach
//---------------------------------------------------------------------
void roof() {
  static float roofSpeed = 0;
  static uint32_t startTime = 0;
  static float timeRoofDrive = 0;

  float batteryVoltageActual = 12;

  if (target_roofPosition == actual_roofPosition) {
    return;
  }

  if (roofSpeed == 0) {

    float deltaRope = 10 * abs((sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - target_roofPosition * 0.85 * 3.14 / 180))) - sqrt(2 * 59 * 59 * (1 - cos(0.5 * 3.14 - actual_roofPosition * 0.85 * 3.14 / 180)))));  //in mm
    Serial.println(deltaRope);

    //Auffahren
    if (target_roofPosition > actual_roofPosition) {
      roofSpeed = 796 / (0.45 * pow(batteryVoltageActual, 2) - 13.81 * batteryVoltageActual + 140.89);  //Ergebnis in mm/s    //Spannung am Motortreiber um ca 1.5V kleiner

      timeRoofDrive = deltaRope / roofSpeed * 1000;

      digitalWrite(k2, HIGH);   //Mosfet Motortreiber EINS-Setzen
      digitalWrite(IN1, HIGH);  //Auffahren auf EINS-Setzen
      digitalWrite(IN2, LOW);   //Zufahren auf NULL-Setzen
      analogWrite(ENA, 255);    //Auffahren volle Geschwindigkeit
      startTime = millis();     //Anfangszeit von Dach speichern

      Serial.println(roofSpeed);
      Serial.println(timeRoofDrive);
    }

    //Zufahren
    if (target_roofPosition < actual_roofPosition) {
      roofSpeed = 796 / (0.32 * pow(batteryVoltageActual, 2) - 11.12 * batteryVoltageActual + 123.98);

      timeRoofDrive = deltaRope / roofSpeed * 1000;

      digitalWrite(k2, HIGH);   //Mosfet Motortreiber EINS-Setzen
      digitalWrite(IN1, LOW);   //Auffahren auf NULL-Setzen
      digitalWrite(IN2, HIGH);  //Zufahren auf EINS-Setzen
      analogWrite(ENA, 255);    //Zufahren volle Geschwindigkeit
      startTime = millis();

      Serial.println(roofSpeed);
      Serial.println(timeRoofDrive);
    }
  }

  if (roofSpeed != 0) {

    //Fahrzeit läuft ab
    if (millis() >= (startTime + timeRoofDrive) && target_roofPosition != 100 && target_roofPosition != 0) {
      digitalWrite(IN1, LOW);  //Pin 31 zum auffahren NULL-Setzen
      digitalWrite(IN2, LOW);  //Pin 33 zum zufahren NULL-Setzen
      analogWrite(ENA, 0);     //Motorgeschwindigkeit NULL-Setzen
      //digitalWrite(k2, LOW);       //Motortreiber AUS schalten
      actual_roofPosition = target_roofPosition;
      roofSpeed = 0;
      Serial.println("Zeit abgelaufen");
    }

    //Limitswitch oben beim auffahren
    if (limitSwitch(s1) == 0) {  //wenn s1 betätigt
      digitalWrite(IN1, LOW);    //Pin 31 zum auffahren NULL-Setzen
      digitalWrite(IN2, LOW);    //Pin 33 zum zufahren NULL-Setzen
      analogWrite(ENA, 0);       //Motorgeschwindigkeit NULL-Setzen
      //digitalWrite(k2, LOW);       //Motortreiber AUS schalten
      actual_roofPosition = 100;
      roofSpeed = 0;
      Serial.println("Sensor oben");
    }

    //Limitswitch unten beim zufahren
    if (limitSwitch(s2) == 0) {
      digitalWrite(IN1, LOW);  //Pin 31 zum auffahren NULL-Setzen
      digitalWrite(IN2, LOW);  //Pin 33 zum zufahren NULL-Setzen
      analogWrite(ENA, 0);     //Motorgeschwindigkeit NULL-Setzen
      //digitalWrite(k2, LOW);       //Motortreiber AUS schalten
      actual_roofPosition = 0;
      roofSpeed = 0;
      Serial.println("Sensor unten");
    }
  }
}
//Funktion Bewässerung
void irrigation() {
  //Zyklus 1 Pumpgang: 4sec = 4000ms

  #define PUMPCYCLETIME 4000           //Ein Pumpenzyklus, Druck rauf -> runter  [ms]
  #define MAX_TIME_LAST_IRRIGATION 30  //Früheste Zeit, wann automatisch Pumpe wieder einschalten darf [min]
  #define MAX_CYCLE 5                  //Durchläufe pro DELTA_IRRIGATION
  #define DELTA_IRRIGATION_ON 10       //Unterschied in % von SOLL und IST Bodenfeuchte

  static uint32_t startOfCycle = 0;
  static uint32_t lastIrrigationTime = 0;
  static uint8_t numberOfCycle = 0;

  //Wenn Pumpe nicht läuft
  if (!(dataSensorAktor & 4096)) {
    //Handbetrieb, wenn aktiviert, bewässert System in Schritten von 5 Zyklen (20s) bis deaktiviert
    if (dataSensorAktor & 256) {
      digitalWrite(k2, HIGH);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, 0);
      dataSensorAktor |= 4096;
      numberOfCycle = 1;
    }

    //Automatikbetrieb
    if ((((soilHumidityTarget - soilHumidity[0]) > DELTA_IRRIGATION_ON) && ((millis() - lastIrrigationTime) >= MAX_TIME_LAST_IRRIGATION * 60000)) && !(dataSensorAktor & 4096)) {
      digitalWrite(k2, HIGH);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, 0);
      dataSensorAktor |= 4096;
      numberOfCycle = MAX_CYCLE;
    }
  }

  //Wenn Pumpe läuft UND numberOfCycle < MAX_CYCLE
  else if (numberOfCycle > 0) {
    if ((millis() - startOfCycle) >= PUMPCYCLETIME) {
      startOfCycle = millis();
      numberOfCycle--;
    }

    //Erste Hälfte des Zyklus Analogwert erhöhen
    if ((millis() - startOfCycle) <= 0.5 * PUMPCYCLETIME) {
      analogWrite(ENB, (255 * (millis() - startOfCycle)) / 0.5 * PUMPCYCLETIME);
    }

    //Zweite Hälfte des Zyklus Analogwert erniedrigen
    else {
      analogWrite(ENB, 255 - (255 * (millis() - startOfCycle)) / PUMPCYCLETIME);
    }

  }

  else if (dataSensorAktor & 256) {
    numberOfCycle = 1;
  }

  else if (numberOfCycle == 0) {
    dataSensorAktor &= ~4096;
    //digitalWrite(k2, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
    lastIrrigationTime = millis();
  }
}