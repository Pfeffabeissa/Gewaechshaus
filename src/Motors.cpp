#include <Arduino.h>
#include <header.h>
#include <Constants.h>


void blink(void)
{
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
}
