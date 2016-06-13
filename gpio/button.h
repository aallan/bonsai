#define max_buttons 17

struct button {
    int pin_c;
    volatile int value;
    volatile int lastValue;
};

struct button buttons[max_buttons];
struct button *setupbutton(int pin_c); 
