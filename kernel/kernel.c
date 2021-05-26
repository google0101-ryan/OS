#include "util/multiboot.h"
#include "drivers/printf.h"
#include "drivers/vga.h"
#include "drivers/pci.h"
#include "x86/gdt.h"
#include "x86/tss.h"
#include "drivers/pit.h"
#include "libk/string.h"
#include "drivers/idt.h"
#include "mem/heap.h"
#include "mem/pmm.h"

void print_ok()
{
    set_curr_color(GREEN);

    print_string("[ OK ]\n");

    set_curr_color(WHITE);
}



int kmain()
{
    video_init();

    set_curr_color(GREEN);

    printf("Welcome to Rynix v1.0!\n");

    set_curr_color(WHITE);

    printf("Installing GDT...\t\t");

    gdt_init();

    print_ok();

    printf("GDT Installed at 0x%04x\n", &gdt_entries);

    printf("Installing TSS...\t\t");

    tss_init(5, 0x10, 0);

    print_ok();

    printf("Setting up IDT...\t\t");

    idt_init();

    print_ok();

    printf("IDT Installed at 0x%04x\n", &idt_entries);

    asm volatile("sti");

    printf("Installing PIT...\t\t");

    timer_init();

    print_ok();

    printf("Setting up the heap..\t");

    heap_init();

    print_ok();

    printf("Setting up the PMM...\t");

    pmm_init();

    print_ok();

    printf("Setting up PCI...\t\t");

    pci_init();

    print_ok();

    for (;;);

    return 0;
}