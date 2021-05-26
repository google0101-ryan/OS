#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#define K 1024
#define M (1024*K)
#define G (1024*M)

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

// Our kernel now loads at 0xC0000000, so what low memory address such as 0xb800 you used to access, should be LOAD_MEMORY_ADDRESS + 0xb800
#define LOAD_MEMORY_ADDRESS 1*M;

typedef struct registers
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
}register_t;

// Register structs for bios service
typedef struct register16 {
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;

    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    uint16_t eflags;
}register16_t;

void outportb(uint16_t port, uint8_t val);
uint8_t inportb(uint16_t port);
uint16_t inports(uint16_t _port);
void outports(uint16_t _port, uint16_t _data);
uint32_t inportl(uint16_t _port);
void outportl(uint16_t _port, uint32_t _data);

#endif