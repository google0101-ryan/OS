#ifndef PMM_H
#define PMM_H

#include <stdint.h>

typedef uint32_t physaddr_t;

/**
 * \brief Initialize the Physical Memory Manager
 * */
void pmm_init();

/**
 * \brief Allocate a 4 KB page in physical memory.
 * \return The allocated physical address, 0 if no free page is found.
 * */
physaddr_t pmm_alloc_page();

/**
 * \brief Free a 4 KB page from the physical memory of the computer.
 * \param page the memory address of the page to free up from memory.
 */
void pmm_free_page(physaddr_t page);

#endif