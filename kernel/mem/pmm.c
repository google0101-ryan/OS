#include "../util/multiboot.h"
#include "heap.h"
#include "pmm.h"
#include "../drivers/printf.h"

typedef uint32_t frame_idx_t;

static uint32_t *frames_map;

static frame_idx_t frames_count;

#define FRAME_NUMBER(memaddr) (memaddr >> 12) /* divide by 4096 */
#define PHYSICAL_ADDR(idx) (idx << 12) /* multiply by 4096 */

#define BIT_INDEX(frame_idx) (frame_idx >> 5) /* divide by 32 */
#define BIT_OFFSET(frame_idx) (frame_idx & 0x1F) /* modulus 32 */

#define OFFSET_MASK(bit) (((unsigned int) 0x1) << bit)

static inline void frame_set(frame_idx_t idx)
{
    frames_map[BIT_INDEX(idx)] |= OFFSET_MASK(BIT_OFFSET(idx));
}

static inline uint32_t frame_test(frame_idx_t idx)
{
    return frames_map[BIT_INDEX(idx)] & OFFSET_MASK(BIT_OFFSET(idx));
}

static inline void frame_clear(frame_idx_t idx)
{
    frames_map[BIT_INDEX(idx)] &= ~OFFSET_MASK(BIT_OFFSET(idx));
}

static frame_idx_t frame_find_free ()
{
	frame_idx_t idx, offset;
	unsigned int map;

	for (idx = 0; idx < frames_count; idx += 32) {
		if (frames_map[BIT_INDEX(idx)] != (unsigned int) -1) {
			map = frames_map[BIT_INDEX(idx)];
			for (offset = 0; offset < 32; offset++) {
				if (!(map & OFFSET_MASK(BIT_OFFSET(offset)))) {
					return idx | offset;
				}
			}
		}
	}

	return -1;
}

static void allocate_frames()
{
    uint32_t mapsize, memsize = (mbi->mem_upper + 1) << 10;

    frames_count = FRAME_NUMBER(memsize);

    mapsize = BIT_INDEX(frames_count);
    if (BIT_OFFSET(frames_count) != 0)
    {
        mapsize++;
    }
    frames_map = (uint32_t*)kmalloc(mapsize);
}

static void reserve_system_block(multiboot_mmap_t *mblock)
{
    physaddr_t page;
    uint32_t f, frame_offset = FRAME_NUMBER(mblock->base_addr);

    for (f = frame_offset, page = 0;
        f < frames_count && page < mblock->length;
        f++, page += 0x1000)
    {
        frame_set(f);
    }
}

static void reserve_system()
{
    multiboot_mmap_t * mblock;
	physaddr_t mmap_end;
		mblock = (multiboot_mmap_t *) mbi->mmap_addr;
		mmap_end = (physaddr_t) mblock + mbi->mmap_length;

		while ((physaddr_t) mblock < mmap_end) {
			/* If the block is reserved, mark the pages. */
			if (mblock->type != 1) {
                printf("Reserving block with start 0x%x and size %d Bytes\n", mblock->base_addr, mblock->length);
				reserve_system_block(mblock);
			}
            else
            {
                printf("Free block with start 0x%x and size %d Bytes\n", mblock->base_addr, mblock->length);
            }

			/* Skips to the next block (forgive weird math). */
			mblock = (multiboot_mmap_t *) ((unsigned int) mblock +
				mblock->size + sizeof(mblock->size));
		}
}

static void reserve_kernel()
{
    extern char __start, __end;
    physaddr_t addr;
    frame_idx_t frame;

    addr = ((physaddr_t)&__start) & 0xFFFFF000;

    for (frame = FRAME_NUMBER(addr);
        addr < (physaddr_t)&__end;
        addr += 4096, frame++)
    {
        frame_set(frame);
    }
}

void pmm_init()
{
    allocate_frames();
    reserve_system();
    reserve_kernel();
}

physaddr_t
pmm_alloc_page ()
{
	frame_idx_t frame = frame_find_free();
	if (frame == (unsigned int) -1) {
		return 0;
	} else {
		frame_set(frame);
		return PHYSICAL_ADDR(frame);
	}
}

void
pmm_free_page (physaddr_t page)
{
	frame_clear(FRAME_NUMBER(page));
}