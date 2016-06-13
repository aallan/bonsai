#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <curl/curl.h>

#include "potentiometer.h"
#include "http.h"

int numberofpots = 0;

PI_THREAD (potThread) {

   uint32_t sleep = 50; // in millisec

   uint8_t buf[2];
   float ma_new;
   float ma_old;
   float alpha = 0.25;
   int margin = 2500;

   while(1) {
       guaranteedSleep(sleep);

       struct pot *pot = pots;
       for (; pot < pots + numberofpots; pot++) {
           buf[0] = 0x60 + pot->channel;
	   buf[1] = 0x00;
	   wiringPiSPIDataRW(pot->channel, buf, 2);

           // fading memory (exponential) moving average
           ma_old = (float)pot->lastValue;
           ma_new = (float)(buf[1] + (buf[0] & 0x03) << 8);
           ma_new = alpha * ma_new + (1-alpha) * ma_old;

           pot->lastValue = pot->value;
           pot->value = (unsigned int)ma_new;
//           printf("last_value = %d, value = %d, difference = %d\n", pot->lastValue, pot->value, pot->value - pot->lastValue );

           // lazy, need to do a real low pass filter
           int significant_change = 0;
           if ( pot->value > ((pot->lastValue)+margin) ||
                pot->value < ((pot->lastValue)-margin) ) {
               significant_change = 1;
           }
           if ( significant_change ) {
              significant_change = 0;
              char *str;
              int size = asprintf(&str, "vol/%u", pot->value);
              if(size == -1) {
                 fprintf(stderr, "asprintf() failed: cannot malloc memory.");
              }
              makehttprequest( str );
           } else {
           }
       }
   }
}

struct pot *setuppot(int channel) {
    if (numberofpots > max_pots)
    {
        printf("Maximum number of potentiometers exceded: %i\n", max_pots);
        return NULL;
    }

    struct pot *newpot = pots + numberofpots++;
    newpot->channel = channel;
    newpot->value = 0;
    newpot->lastValue = 0;
    if (wiringPiSPISetup(newpot->channel, 4000000) < 0) {
		fprintf (stderr, "SPI Setup failed: %s\n", strerror (errno));
		exit(errno);
    }
    return newpot;
}

int startpotsample( struct pot *newpot ) {
    int x = piThreadCreate (potThread) ;
    if (x != 0) {
        fprintf (stderr, "Potentiomenter thread failed to start: %i\n", x);
    }
    return x;
}

void guaranteedSleep(uint32_t msec) {
    struct timespec timeout0;
    struct timespec timeout1;
    struct timespec* tmp;
    struct timespec* t0 = &timeout0;
    struct timespec* t1 = &timeout1;

    t0->tv_sec = msec / 1000;
    t0->tv_nsec = (msec % 1000) * (1000 * 1000);

    while ((nanosleep(t0, t1) == (-1)) && (errno == EINTR)) {
        tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
}
