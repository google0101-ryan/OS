#include "isr.h"
#include "pic.h"
#include "../drivers/printf.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(int num, isr_t hanlder)
{
    if (num < 256)
        interrupt_handlers[num] = hanlder;
}

void final_irq_handler(register_t * reg) {
    if(interrupt_handlers[reg->int_no] != NULL) {
        isr_t handler = interrupt_handlers[reg->int_no];
        handler(reg);
    }
    irq_ack(reg->int_no);
}