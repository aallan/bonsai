#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "button.h"
#include "http.h"

int numberofbuttons = 0;

void updateButtons()
{
    struct button *button = buttons;
    for (; button < buttons + numberofbuttons; button++)
    {
        int VAL = digitalRead(button->pin_c);
        button->lastValue = button->value;
	button->value = VAL;
        if ( button->lastValue != button->value ) {
           if ( button->value == 1 ) {
//              printf("Button pressed\n");
              char str[] = "button";
              makehttprequest( str );

           } else {
//              printf("Button released\n");
           }
        }
    }
}

struct button *setupbutton(int pin_c)
{
    if (numberofbuttons > max_buttons)
    {
        printf("Maximum number of buttons exceded: %i\n", max_buttons);
        return NULL;
    }

    struct button *newbutton = buttons + numberofbuttons++;
    newbutton->pin_c = pin_c;
    newbutton->value = 0;
    newbutton->lastValue = 0;

    pinMode(pin_c, INPUT);
    pullUpDnControl(pin_c, PUD_DOWN);
    wiringPiISR(pin_c,INT_EDGE_BOTH, updateButtons);

    return newbutton;
}
