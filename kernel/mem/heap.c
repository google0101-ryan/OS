#include "heap.h"


/** Magic number that indicates that a heap control block follows.  */
#define HEAP_MAGIC_HEAD 0x51514949

/** Magic value that indicates that the current memory block is free.  */
#define HEAP_MAGIC_FREE 0x40404040

/** Magic value that indicates that the current memory block is allocated.  */
#define HEAP_MAGIC_USED 0xEFEFEFEF

/** Points to a memory address related to the heap.  */
typedef unsigned char * HEAP_ADDR;

/** Used to indicate the size of a memory region in the heap.  */
typedef size_t HEAP_SIZE;

/**
 * \brief Header control block
 *
 * The header control block is a structure that's present at the beginning of a
 * memory segment in the heap.  It's used to mark metadata about the memory
 * region itself, such as whether the memory region is free and can be
 * allocated, or if it's currently in use.
 */
typedef struct heap_block {
	int magic, status;
	HEAP_ADDR next, prev;
	HEAP_SIZE size;
} heap_block_t;

/* This points to the root control block of the heap once it has been
 * initialised.  Control blocks are connected through a linked list.  The
 * allocator will traverse the linked list when looking for blocks marked as
 * free.  */
static volatile heap_block_t * heap_root;

/* Defined in ldscript.  This symbol is located at the bottom of the kernel
 * heap.  Heap allocations should always start at a memory address that is
 * equal or greater than the memory address of this symbol.  */
extern volatile const unsigned char heap_bottom;

/* Defined in ldscript.  This symbol is located at the top of the kernel heap.
 * Heap allocations should never touch any memory address whose value is equal
 * or greater than the memory address of this symbol.  */
extern volatile const unsigned char heap_top;
/**
 * \brief Attempt to merge the given heap control blocks.
 * \param head the first memory block to merge.
 * \param tail the second memory block to merge.
 * \return pointer to the merged block, or NULL if no merge was made.
 */
static inline heap_block_t *
heap_merge (heap_block_t * head, heap_block_t * tail)
{
	HEAP_ADDR head_addr = (HEAP_ADDR) head;
	HEAP_ADDR tail_addr = (HEAP_ADDR) tail;
	heap_block_t * bound;

	if (head->magic != HEAP_MAGIC_HEAD || head->magic != tail->magic) {
		/* Both headers must be valid.  */
		return 0;
	}
	if (head->status != HEAP_MAGIC_FREE || head->status != tail->status) {
		/* Both headers must be marked as free.  */
		return 0;
	}
	if (head->next != (HEAP_ADDR) tail || tail->prev != (HEAP_ADDR) head) {
		/* Memory regions must be linked.  */
		return 0;
	}
	if (head_addr + sizeof(heap_block_t) + head->size != tail_addr) {
		/* Memory regions must be consecutive. */
		return 0;
	}

	/* Unlink tail from chain. */
	if (tail->next) {
		bound = (heap_block_t *) tail->next;
		bound->prev = (HEAP_ADDR) head;
	}
	head->next = tail->next;

	/* Assignate the space for the control block and buffer to head. */
	head->size += (sizeof(heap_block_t) + tail->size);

	return head;
}

void
heap_init (void)
{
	HEAP_SIZE total;
	HEAP_ADDR top, bottom;

	/* Extract the heap size.  Just remember that heap_top and heap_bottom
	 * are symbols defined in the linkerscript, therefore we should only
	 * access the memory address, not the values themselves.  Also, top is
	 * at the end of the heap because memory maps go bottom to up.  */
	bottom = (HEAP_ADDR) &heap_bottom;
	top = (HEAP_ADDR) &heap_top;
	total = (HEAP_SIZE) (top - bottom);

	/* Create the root control block for the heap.  */
	heap_root = (heap_block_t *) bottom;
	heap_root->magic = HEAP_MAGIC_HEAD;
	heap_root->status = HEAP_MAGIC_FREE;
	heap_root->size = total - sizeof(heap_block_t);
	heap_root->prev = 0;
	heap_root->next = 0;
}

void *kmalloc(size_t size)
{
    heap_block_t *root, *block, *next_block;
    HEAP_ADDR blockptr, buffer, after_buffer;

    root = (heap_block_t*)heap_root;
    buffer = 0;

    for (block = root; block; block = (heap_block_t *) block->next) {
		blockptr = (HEAP_ADDR) block;

		if (block->magic != HEAP_MAGIC_HEAD) {
			/* This is not a heap control block!  Ackchyually,
			 * yeah, nothing guarantees us that this is a rogue
			 * control block with valid magic numbers.  */
			goto _cleanup;
		}

		if (block->status != HEAP_MAGIC_FREE) {
			/* Block cannot be used because it's allocated.  */
			continue;
		}

		if (block->size < size) {
			/* Block is not big enough for this allocation.  */
			continue;
		}

		/* Okay, so if we are here, we can allocate.  */
		block->status = HEAP_MAGIC_USED;
		buffer = (HEAP_ADDR) (blockptr + sizeof(heap_block_t));

		/* Early return if there is not enough space for split.  */
		if (block->size <= (size + sizeof(heap_block_t) + 1)) {
			goto _cleanup; /* No.  */
		}

		after_buffer = (HEAP_ADDR) (buffer + size);
		next_block = (heap_block_t *) after_buffer;

		/* Mark the bounds of the buffer as a new block.  */
		next_block->magic = HEAP_MAGIC_HEAD;
		next_block->status = HEAP_MAGIC_FREE;
		next_block->size = block->size - size - sizeof(heap_block_t);
		next_block->prev = (HEAP_ADDR) block;
		next_block->next = block->next;

		/* Shrink the current block.  */
		block->size = size;
		block->next = (HEAP_ADDR) next_block;

		/* No iterations required anymore.  */
		break;
	}
_cleanup:
    return buffer;
}

void
heap_free (void * ptr)
{
	heap_block_t * bufheader, * nextblock, * prevblock;

	/* If this is an allocated buffer, it should have a header.  */
	bufheader = (heap_block_t *) (ptr - sizeof(heap_block_t));
	if (bufheader->magic != HEAP_MAGIC_HEAD) {
		/* Hey wait a second!  */
		goto _cleanup;
	}

	/* It should be used.  */
	if (bufheader->status != HEAP_MAGIC_USED) {
		goto _cleanup;
	}

	/* Mark the block as free.  */
	bufheader->status = HEAP_MAGIC_FREE;

	/* Test for merge. */
	if (bufheader->next) {
		heap_merge(bufheader, (heap_block_t *) bufheader->next);
	}
	if (bufheader->prev) {
		heap_merge((heap_block_t *) bufheader->prev, bufheader);
	}
_cleanup:
    return;
}