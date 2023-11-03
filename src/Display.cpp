#include <Constants.h>
#include <Header.h>
#include <Arduino.h>
#include <U8glib.h>
#include <Wire.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

float targetSoilMoisture;

static uint32_t lastTimeButtonPressed = 0;    //Zeit vom letzten Tastendruck
static uint32_t nextTimeMeasureRedrawed = 0;
static uint8_t displayPage = 0;
static uint8_t displayLine = 0;
static uint8_t uiKeyCode;
static uint8_t uiKeyCodeFirst;
static uint8_t uiKeyCodeLast;
static uint8_t displaySettingState = 0;        // 1: Einstellung möglich, 2: Einstellung wurde bestätigt

static bool displayIsOn = 0;
static bool displayRedrawRequired = 0;

static uint8_t temporaryTargetRoofPosition = 0;

void initializeDisplay() {
    u8g.begin();
}

// Funktion zum zusammenfügen aller Display-Unterfunktionen
void manageDisplayfunctions(void)
{
    switchDisplayOnOff();
    
    if(displayIsOn)
    {
        checkDisplayUserInput();
        setRewdrawDisplayMeasure();
        
        //Wenn uiKeyCode == 0, keine Tasteränderung im letzten Programmdurchlauf
        if(uiKeyCode)
        {
            setDisplayParameters();
            setSettings();
        }

        if(displayRedrawRequired)
        {  
            printDisplayPage();
            displayRedrawRequired = false;
        }
    }
}


// Schaltet Display ein und aus
void switchDisplayOnOff(void)
{   
    // Einschalten
    if(readButton(PIN_UI_BUTTON_RIGHT) && !displayIsOn)
    {   
        displayIsOn = true;
        lastTimeButtonPressed = millis();
        displayPage = 1;
        displayLine = 0;
        displaySettingState = 0;
        uiKeyCodeFirst = KEY_NONE;
        uiKeyCodeLast = KEY_ENTER;         //da sonst direkt in checkDisplayUserInput KEY_ENTER gesetzt wird
        uiKeyCode = KEY_NONE;
    }

    else if((lastTimeButtonPressed + DISPLAY_PUFFER_ON_TIME) < millis())
    {
        displayIsOn = false;
        lastTimeButtonPressed = 0;
        nextTimeMeasureRedrawed = 0;
        displayPage = 0;
        displayRedrawRequired = false;
        printDisplayPage();         //um Display auszuschalten

    }
}


// Lest Tastereingaben aus und speichert diese in uiKeyCode ab
void checkDisplayUserInput()
{
    if(readButton(PIN_UI_BUTTON_RIGHT) == HIGH) uiKeyCodeFirst = KEY_ENTER;
    else if(readButton(PIN_UI_BUTTON_LEFT) == HIGH) uiKeyCodeFirst = KEY_NEXT;
    else if(readButton(PIN_UI_BUTTON_TOP) == HIGH) uiKeyCodeFirst = KEY_UP;
    else if(readButton(PIN_UI_BUTTON_BOTTOM) == HIGH) uiKeyCodeFirst = KEY_DOWN;
    else uiKeyCodeFirst = KEY_NONE;

    // Wenn aktueller Tastendruck nicht der gleiche wie vorher ist
    if(uiKeyCodeFirst != uiKeyCodeLast) 
    {
        uiKeyCode = uiKeyCodeFirst;
        lastTimeButtonPressed = millis();
    }
    // Wenn sich Tastendruck nicht verändert, mit 0 belegen, um keine weiteren Anweisungen am Display auszuführen
    else uiKeyCode = KEY_NONE;

    uiKeyCodeLast = uiKeyCodeFirst;
}


// Seiten- und Zeilenparameter, Messbits für DisplayMeasure setzen
void setDisplayParameters(void)
{
    switch(uiKeyCode)
    {
        case KEY_NEXT:
            displayPage++;
            if(displayPage > DISPLAY_NUMBER_PAGES) displayPage = 1;
            displayLine = 0;
            displaySettingState = 0;
            displayRedrawRequired = true;
            break;

        case KEY_ENTER:
            if(displaySettingState == 0) displaySettingState = 1;
            else displaySettingState = 2;
            displayRedrawRequired = true;
            break;

        case KEY_DOWN:
            if(displaySettingState) 
                break;
            displayLine++;
            if(displayPage == 2 && displayLine > DISPLAY_NUMBER_LINES_2)
                displayLine = 1;
            if (displayPage == 3 && displayLine > DISPLAY_NUMBER_LINES_3)
                displayLine = 1;
            if(displayPage == 6 && displayLine > DISPLAY_NUMBER_LINES_6)
                displayLine = 1;
            displayRedrawRequired = true;
            break;

        case KEY_UP:
            if(displaySettingState)
                break;
            displayLine--;
            if(displayPage == 2 && displayLine == 0)
                displayLine = DISPLAY_NUMBER_LINES_2;
            if(displayPage == 3 && displayLine == 0)
                displayLine = DISPLAY_NUMBER_LINES_3;
            if(displayPage == 6 && displayLine == 0)
                displayLine = DISPLAY_NUMBER_LINES_6;  
            displayRedrawRequired = true;              
            break;
    }

    switch(displayPage)
    {   
        case 0:
            stateDisplayMeasureRequest = 0;
            break;

        case 1:
            stateDisplayMeasureRequest = 0;
            stateDisplayMeasureRequest |= 4;    // TempOut
            stateDisplayMeasureRequest |= 64;   // TempIn
            stateDisplayMeasureRequest |= 128;  // Luftfeuchtigkeit
            nextTimeMeasureRedrawed = millis() + DISPLAY_REDRAW_TIME_MEASURE;
            break;
    
        case 2:
            stateDisplayMeasureRequest = 0;
            stateDisplayMeasureRequest |= 2;    // Bodenfeuchte
            nextTimeMeasureRedrawed = millis() + DISPLAY_REDRAW_TIME_MEASURE;
            break;
        
        case 3:
            stateDisplayMeasureRequest = 0;
            break;
        
        case 4:
            stateDisplayMeasureRequest = 0;
            stateDisplayMeasureRequest |= 1;    //Spannung
            nextTimeMeasureRedrawed = millis() + DISPLAY_REDRAW_TIME_MEASURE;
            break;
        
        case 5: 
            stateDisplayMeasureRequest = 0;
            break;
        
        case 6:
            stateDisplayMeasureRequest = 0;
            break;
    }
}


// Abhängig von aktueller Zeit wird displayRedrawRequired gesetzt
void setRewdrawDisplayMeasure(void) 
{
    if(stateDisplayMeasureRequest && (millis() >= nextTimeMeasureRedrawed))
    {
        displayRedrawRequired = true;
        nextTimeMeasureRedrawed += DISPLAY_REDRAW_TIME_MEASURE;
    }

}


// Abhängig von displaySettingState werden Werte bearbeitet
void setSettings(void)
{
    switch(displayPage)
    {
        case 1:   //Seite 1
          break;

        case 2:   //Seite 2
          if(displayLine == 1)
            targetSoilMoisture = readPoti(0, 65);
          
          if((displayLine == 2) && isPumpRunning && ((uiKeyCode == KEY_DOWN) || (uiKeyCode == KEY_UP)))     //Wenn Pumpe an 
          {
            isIrrigationRequired = false;   //Pumpe ausschalten
          }
          else if((displayLine == 2) && !isPumpRunning && ((uiKeyCode == KEY_DOWN) || (uiKeyCode == KEY_UP)))   //Wenn Pumpe aus
              {
                isIrrigationRequired = true;   //Pumpe einschalten
              }  
          
          displayRedrawRequired = 1;
          break;

        case 3:   //Seite 3
          if(displayLine == 1)
          {
            temporaryTargetRoofPosition = readPoti(0, 100);
            if(displaySettingState == 2)
            {  
              targetRoofPosition = temporaryTargetRoofPosition;
              displaySettingState = 1;
            }
          }

          displayRedrawRequired = 1;  
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


// Führt den Schreibprozess auf das Display aus
void printDisplayPage(void) {
    u8g.firstPage();
    switch(displayPage) {
        case 0:
            do {
                
            } while (u8g.nextPage());
        
        case 1:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "TEMPERATUR");  //Temperatur
                u8g.setPrintPos(13, 12);
                u8g.print("Innen: ");
                u8g.print(actualTemperatureInside);
                u8g.write(0xb0);
                u8g.print("C");
                u8g.setPrintPos(13, 23);
                u8g.print("Aussen: ");
                u8g.print(actualTemperatureOutside);
                u8g.write(0xb0);
                u8g.print("C");

                u8g.drawStr(3, 40, "LUFTFEUCHTIGKEIT");  //Luftfeuchtigkeit
                u8g.setPrintPos(13, 52);
                u8g.print("Innen: ");
                u8g.print(actualAirHumidity);
                u8g.print("%");
            } while (u8g.nextPage());
            break;
        
        case 2:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "BODENFEUCHTIGKEIT");
                u8g.drawStr(3, 43, "BEWAESSERUNG:");

                u8g.setFont(u8g_font_profont12);
                u8g.setFontPosTop();
                u8g.setPrintPos(13, 12);
                u8g.print("IST-WERT: ");
                if (actualSoilMoisture >= 10) u8g.setPrintPos(88, 12);  //Wenn zwei Zahlen vorm Komma
                else u8g.setPrintPos(94, 12);                           //Wenn eine Zahl vorm Komma
                u8g.print(actualSoilMoisture);
                u8g.print("%");
                u8g.setPrintPos(13, 23);
                u8g.print("SOLL-WERT: ");
                if (displayLine == 1) {
                    u8g.drawBox(85, 24, u8g.getStrWidth("60.00%") + 6, 10);
                    u8g.setDefaultBackgroundColor();
                    if (targetSoilMoisture >= 10) u8g.setPrintPos(88, 23);  //Wenn zwei Zahlen vorm Komma
                    else u8g.setPrintPos(94, 23);                           //Wenn eine Zahl vorm Komma
                    u8g.print(targetSoilMoisture);
                    u8g.print("%");
                    u8g.setDefaultForegroundColor();
                }
                else {
                    if (targetSoilMoisture >= 10) u8g.setPrintPos(88, 23);  //Wenn zwei Zahlen vorm Komma
                    else u8g.setPrintPos(94, 23);                           //Wenn eine Zahl vorm Komma
                    u8g.print(targetSoilMoisture);
                    u8g.print("%");
                }
                if (displayLine == 2) {
                    u8g.drawBox(101, 44, u8g.getStrWidth("OFF") + 8, 14);
                    u8g.setDefaultBackgroundColor();
                    if (isPumpRunning)  //Wenn Pumpe an -> ON auf Display (Farbe negiert)
                    {
                    u8g.setPrintPos(108, 45);
                    u8g.print("ON");
                    } else  //Wenn Pumpe aus -> OFF auf Display (Farbe negiert)
                    {
                    u8g.setPrintPos(104, 45);
                    u8g.print("OFF");
                    }
                    u8g.setDefaultForegroundColor();
                } 
                else {
                    u8g.drawFrame(101, 44, u8g.getStrWidth("OFF") + 8, 14);
                    if (isPumpRunning)  //Wenn Pumpe an -> ON auf Display (Farbe normal)
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
            break;
        
        case 3:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "DACH");

                u8g.setFont(u8g_font_profont12);
                u8g.setFontPosTop();
                u8g.setPrintPos(13, 12);
                u8g.print("IST-WERT: ");
                if (actualRoofPosition == 100) u8g.setPrintPos(88, 12);  //Wenn zwei Zahlen vorm Komma
                else if (actualRoofPosition >= 10) u8g.setPrintPos(94, 12);
                else u8g.setPrintPos(100, 12);  //Wenn eine Zahl vorm Komma
                u8g.print(actualRoofPosition);
                u8g.print("%");
                u8g.setPrintPos(13, 23);
                u8g.print("SOLL-WERT: ");

                if (displayLine == 1) {
                    u8g.drawBox(85, 24, u8g.getStrWidth("100%") + 6, 10);
                    u8g.setDefaultBackgroundColor();
                    if (temporaryTargetRoofPosition >= 100) u8g.setPrintPos(88, 23);      //Wenn drei Zahlen vorm Komma
                    else if (temporaryTargetRoofPosition >= 10) u8g.setPrintPos(94, 23);  //Wenn zwei Zahlen vorm Komma
                    else u8g.setPrintPos(100, 23);                                         //Wenn eine Zahl vorm Komma
                    u8g.print(temporaryTargetRoofPosition);
                    u8g.print("%");
                    u8g.setDefaultForegroundColor();
                }
                else {
                    if (temporaryTargetRoofPosition >= 100) u8g.setPrintPos(88, 23);      //Wenn zwei Zahlen vorm Komma
                    else if (temporaryTargetRoofPosition >= 10) u8g.setPrintPos(94, 23);  //Wenn zwei Zahlen vorm Komma
                    else u8g.setPrintPos(100, 23);                                         //Wenn eine Zahl vorm Komma
                    if (temporaryTargetRoofPosition != 100) u8g.print(temporaryTargetRoofPosition);
                    else u8g.print("100");
                    u8g.print("%");
                }
            } while (u8g.nextPage());
            break;

        case 4:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "PROZESSDATEN");
                u8g.setPrintPos(13, 12);
                u8g.print("Spannung: ");
                u8g.print(actualBatteryVoltage);
                u8g.print("V");
                u8g.setPrintPos(13, 23);
            } while (u8g.nextPage());
            break;

        case 5:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "MITTELWERTE 30 Minuten");
                u8g.setFont(u8g_font_profont12);
                u8g.setPrintPos(10, 22);
                u8g.print("Spannung: ");
                u8g.print(averageBatteryVoltage);
                u8g.print("V");
                u8g.setPrintPos(10, 32);
                u8g.print("Temp. A: ");
                u8g.print(averageTemperatureOutside);
                u8g.print(0xb0);
                u8g.print("C");
                u8g.setPrintPos(10, 23);
                u8g.print("Temp. innen: ");
                u8g.print(averageTemperatureInside);
                u8g.print(0xb0);
                u8g.print("C");
                u8g.setPrintPos(10, 42);
                u8g.print("Bodenfeuchte: ");
                u8g.print(averageSoilMoisture);
                u8g.print("%");
                u8g.setPrintPos(10, 52);
                u8g.print("Luftfeuchte: ");
                u8g.print(averageAirHumidity);
                u8g.print("%");
            } while (u8g.nextPage());
            break;

        case 6:
            do {
                u8g.setFont(u8g_font_profont15);
                u8g.setFontPosTop();
                u8g.drawStr(3, 0, "EINSTELLUNGEN");
            } while (u8g.nextPage());
            break;
    }
}