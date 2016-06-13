#include <stdint.h>

#define max_pots 2

struct pot {
    int channel;
    volatile unsigned int value;
    volatile unsigned int lastValue;
};

struct pot pots[max_pots];
struct pot *setuppot(int channel);
int startpotsample(struct pot *newpot);
void guaranteedSleep(uint32_t msec);
