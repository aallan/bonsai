#include "stdio.h"
#include "limits.h"
#include "rotaryencoder.h"
#include "button.h"
#include "potentiometer.h"

int main() {

    printf("Bonsai Hardware Handler V0.1\n");
    wiringPiSetup () ;

    /*using pin 22*/
    printf("Setting up button on pin 22...\n");
    struct button *button = setupbutton(3);

    /*using pins 23/24*/
    printf("Setting up rotary encoder on pins 23 and 24...\n");
    struct encoder *encoder = setupencoder(4,5);

    /*on SPI channel 0*/
    printf("Setting up rotary potentiometer on SPI bus...\n");
    struct pot *pot = setuppot(0);
    int x = startpotsample( pot );
    if (x != 0 ) {
        fprintf (stderr, "Shutting down...\n");
        return x;
    }

    printf("Waiting for interrupts...\n");
    for (;;) {
        sleep(UINT_MAX);
    }
    return(0);
}
