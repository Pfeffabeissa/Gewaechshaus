#include <Constants.h>
#include <Header.h>
#include <Arduino.h>
#include <U8glib.h>
#include <Wire.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


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



// Funktion zum zusammenfügen aller Display-Unterfunktionen
void manageDisplayfunctions(void)
{
    switchDisplayOnOff();
    
    if(displayIsOn)
    {
        checkDisplayUserInput();

        //Wenn 0, keine Tasteränderung im letzten Programmdurchlauf
        if(uiKeyCode)
        {

        }

        switch(displayPage)
        {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
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
        //printDisplayPage auf 0 (ausschalten)
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


//Seiten- und Zeilenparameter, Messbits für DisplayMeasure setzen
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
            stateDisplayMeasureRequest |= 4;
            stateDisplayMeasureRequest |= 64;
            nextTimeMeasureRedrawed = millis() + DISPLAY_REDRAW_TIME_MEASURE;
            break;
    
        case 2:
            stateDisplayMeasureRequest = 0;
            stateDisplayMeasureRequest |= 2;
            nextTimeMeasureRedrawed = millis() + DISPLAY_REDRAW_TIME_MEASURE;
            break;
        
        case 3:
            stateDisplayMeasureRequest = 0;
            break;
        
        case 4:
            stateDisplayMeasureRequest = 0;
            stateDisplayMeasureRequest |= 1;
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


//Abhängig von aktueller Zeit wird displayRedrawRequired gesetzt
void setRewdrawDisplayMeasure(void) 
{
    if(stateDisplayMeasureRequest && (millis() >= nextTimeMeasureRedrawed))
    {
        displayRedrawRequired = true;
        nextTimeMeasureRedrawed += DISPLAY_REDRAW_TIME_MEASURE;
    }

}

void setSettings(void)
{
    switch(displayPage)
    {
        case 1:   //Seite 1
          break;

        case 2:   //Seite 2
          if(displayLine == 1)
            soilHumidityTarget = poti(0, 65);
          
          if((displayLine == 2) && (dataSensorAktor & 256) && ((uiKeyCode == KEY_DOWN) || (uiKeyCode == KEY_UP)))     //Wenn Pumpe an 
          {
            dataSensorAktor = dataSensorAktor & ~256;   //Pumpe ausschalten
          }
          else if((displayLine == 2) && !(dataSensorAktor & 256) && ((uiKeyCode == KEY_DOWN) || (uiKeyCode == KEY_UP)))   //Wenn Pumpe aus
              {
                dataSensorAktor = dataSensorAktor | 256;   //Pumpe einschalten
              }  
          
          redraw_required = 1;
          break;

        case 3:   //Seite 3
          if(displayLine == 1)
          {
            target_roofPositionTemporary = poti(0, 100);
            if(settingsOn == 2)
            {  
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

void printDisplayPage(void)
{

}