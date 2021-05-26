#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include "../util/system.h"

#define INPUT_CLOCK_FREQUENCY 1193180
#define TIMER_COMMAND 0x43
#define TIMER_DATA 0x40
#define TIMER_ICW 0x36

uint32_t jiffies;
uint16_t hz;

void timer_init();
void sleep(int sec);
void set_frequency(uint16_t hz);
void timer_handler(register_t * reg);

#endif