#define max_encoders 8

struct encoder {
    int pin_a;
    int pin_b;
    volatile long value;
    volatile int counter;
    volatile int lastEncoded;
};

struct encoder encoders[max_encoders];
struct encoder *setupencoder(int pin_a, int pin_b);
