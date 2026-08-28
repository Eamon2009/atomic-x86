/* mm/kmalloc.c
 *simple bump-pointer + free-list heap allocator.
 * heap starts just above the PMM bitmap [after kernel_end]
 * each allocation is prefixed with a header storing its size. */

#include "kmalloc.h"
#include "pmm.h"
#include "paging.h"
#include "../kernel/panic.h"
#include "../lib/string.h"
#include "../lib/kprintf.h"

#define HEAP_START 0x400000 /* 4 MB — above identity-mapped region */
#define HEAP_MAX 0x800000   /* 8 MB ceiling */
#define MAGIC_FREE 0xDEADBEEF
#define MAGIC_USED 0xC0FFEE00

typedef struct block
{
       uint32_t magic;
       size_t size; /* usable bytes (not including header) */
       struct block *next;
} block_t;

static block_t *heap_head = NULL;
static uint32_t heap_brk = HEAP_START;

static block_t *extend_heap(size_t size)
{
       size_t need = ALIGN_UP(sizeof(block_t) + size, PMM_FRAME_SIZE);
       if (heap_brk + need > HEAP_MAX)
              return NULL;

       for (uint32_t off = 0; off < need; off += PMM_FRAME_SIZE)
       {
              uint32_t frame = pmm_alloc_frame();
              if (!frame)
                     return NULL;
              paging_map(heap_brk + off, frame, PAGE_PRESENT | PAGE_WRITE);
       }

       block_t *b = (block_t *)heap_brk;
       b->magic = MAGIC_FREE;
       b->size = need - sizeof(block_t);
       b->next = NULL;
       heap_brk += need;
       return b;
}

void kmalloc_init(void)
{
       heap_head = extend_heap(4096);
       if (!heap_head)
              panic("kmalloc_init: failed to initialise heap");
       kprintf("[HEAP] kernel heap ready at 0x%x\n", HEAP_START);
}

void *kmalloc(size_t size)
{
       if (!size)
              return NULL;
       size = ALIGN_UP(size, 8); /* 8-byte alignment */

       for (block_t *b = heap_head; b; b = b->next)
       {
              if (b->magic == MAGIC_FREE && b->size >= size)
              {
                     b->magic = MAGIC_USED;
                     return (void *)(b + 1);
              }
       }

       block_t *b = extend_heap(size);
       if (!b)
       {
              kprintf("[HEAP] OOM!\n");
              return NULL;
       }

       b->magic = MAGIC_USED;
       if (!heap_head)
              heap_head = b;
       else
       {
              block_t *tail = heap_head;
              while (tail->next)
                     tail = tail->next;
              tail->next = b;
       }
       return (void *)(b + 1);
}

void kfree(void *ptr)
{
       if (!ptr)
              return;
       block_t *b = (block_t *)ptr - 1;
       if (b->magic != MAGIC_USED)
       {
              kprintf("[HEAP] kfree: bad magic at %p\n", ptr);
              return;
       }
       b->magic = MAGIC_FREE;
}
