#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curl/curl.h>

#include "rotaryencoder.h"
#include "http.h"

int numberofencoders = 0;

void updateEncoders()
{
    struct encoder *encoder = encoders;
    for (; encoder < encoders + numberofencoders; encoder++)
    {
        int MSB = digitalRead(encoder->pin_a);
        int LSB = digitalRead(encoder->pin_b);

        int encoded = (MSB << 1) | LSB;
        int sum = (encoder->lastEncoded << 2) | encoded;

        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
            encoder->value++;
            encoder->counter++;
            if ( encoder->counter == 4 ) {
                char str[] = "channel/clockwise";
                makehttprequest( str );
                encoder->counter = 0;
            } else {
//                printf("clockwise\n");
            }
        }
        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
            encoder->value--;
            encoder->counter++;
            if ( encoder->counter == 4 ) {
//                printf("CHANNEL\n");
                char str[] = "channel/counterclock";
                makehttprequest( str );
                encoder->counter = 0;
            } else {
//                printf("Counterclock\n");
            }
        }

        encoder->lastEncoded = encoded;

    }
}

struct encoder *setupencoder(int pin_a, int pin_b)
{
    if (numberofencoders > max_encoders)
    {
        printf("Maximum number of encodered exceded: %i\n", max_encoders);
        return NULL;
    }

    struct encoder *newencoder = encoders + numberofencoders++;
    newencoder->pin_a = pin_a;
    newencoder->pin_b = pin_b;
    newencoder->value = 0;
    newencoder->lastEncoded = 0;

    pinMode(pin_a, INPUT);
    pinMode(pin_b, INPUT);
    pullUpDnControl(pin_a, PUD_UP);
    pullUpDnControl(pin_b, PUD_UP);
    wiringPiISR(pin_a,INT_EDGE_BOTH, updateEncoders);
    wiringPiISR(pin_b,INT_EDGE_BOTH, updateEncoders);

    return newencoder;
}
