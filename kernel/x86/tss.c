#include "tss.h"
#include "../libk/string.h"

tss_entry_t tss;

void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp)
{
    uint32_t base = (uint32_t)&tss;
    gdt_set_entry(idx, base, base + sizeof(tss_entry_t), 0xE9, 0);

    memset(&tss, 0, sizeof(tss_entry_t));
    tss.ss0 = kss;

    tss.esp0 = kesp;
    tss.cs = 0x0b;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
    tss.ss = 0x13;
    tss_flush();
}

void tss_set_stack(uint32_t kss, uint32_t kesp) {
    tss.ss0 = kss;
    tss.esp0 = kesp;
}