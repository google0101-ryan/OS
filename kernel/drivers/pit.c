#include "pit.h"
#include "vga.h"

int timer_tick;

void timer_init() {
    // IRQ0 will fire 100 times per second
    set_frequency(100);
    register_interrupt_handler(32, timer_handler);
}

void sleep(int sec) {
    uint32_t end = jiffies + sec * hz;
    while(jiffies < end);
}

/*
 * Originally, the frequency is 18 hz, u can change it to whatever you want
 * */
void set_frequency(uint16_t h) {
    hz = h;
    uint16_t divisor = INPUT_CLOCK_FREQUENCY / h;
    // Init, Square Wave Mode, non-BCD, first transfer LSB then MSB
    outportb(TIMER_COMMAND, TIMER_ICW);
    outportb(TIMER_DATA, divisor & 0xFF);
    outportb(TIMER_DATA, (divisor >> 8) & 0xFF);

}

void timer_handler(register_t * reg) {
    jiffies++;
    
    if (timer_tick == 0)
    {
        print_char_at('|', 79, 0);
        timer_tick++;
    }
    else if (timer_tick == 1)
    {
        print_char_at('/', 79, 0);
        timer_tick++;
    }
    else if (timer_tick == 2)
    {
        print_char_at('-', 79, 0);
        timer_tick++;
    }
    else if (timer_tick == 3)
    {
        print_char_at('\\', 79, 0);
        timer_tick = 0;
    }
}