/* mm/pmm.c - bitmap-based physical frame allocatoor */

#include "pmm.h"
#include "../kernel/panic.h"
#include "../lib/kprintf.h"
#include "../lib/string.h"

extern uint32_t kernel_end; /* symbol from linker.ld */

#define MAX_FRAMES (1024 * 256)
#define BMP_WORDS (MAX_FRAMES / 32)
#define PMM_MAX_ADDR ((uint64_t)MAX_FRAMES * PMM_FRAME_SIZE)

static uint32_t bitmap[BMP_WORDS];
static uint32_t total_frames = 0;
static uint32_t free_frames = 0;

static void bmp_set(uint32_t f) { bitmap[f / 32] |= BIT(f % 32); }
static void bmp_clear(uint32_t f) { bitmap[f / 32] &= ~BIT(f % 32); }
static int bmp_test(uint32_t f) { return !!(bitmap[f / 32] & BIT(f % 32)); }

static uint32_t alloc_frame_limit(uint32_t frame_limit)
{
       if (frame_limit > MAX_FRAMES)
              frame_limit = MAX_FRAMES;

       for (uint32_t frame = 0; frame < frame_limit; frame++)
       {
              if (!bmp_test(frame))
              {
                     bmp_set(frame);
                     free_frames--;
                     return frame * PMM_FRAME_SIZE;
              }
       }
       return 0;
}

void pmm_init(multiboot_info_t *mbi)
{
       total_frames = 0;
       free_frames = 0;

       if (!mbi || !(mbi->flags & BIT(6)))
              panic("PMM: multiboot memory map missing");
       memset(bitmap, 0xFF, sizeof(bitmap));
       mmap_entry_t *entry = (mmap_entry_t *)(uintptr_t)mbi->mmap_addr;
       mmap_entry_t *end = (mmap_entry_t *)(uintptr_t)(mbi->mmap_addr + mbi->mmap_length);

       while (entry < end)
       {
              if (entry->type == 1)
              {
                     uint64_t start64 = entry->base_addr;
                     uint64_t end64 = entry->base_addr + entry->length;

                     if (start64 < PMM_MAX_ADDR)
                     {
                            if (end64 > PMM_MAX_ADDR)
                                   end64 = PMM_MAX_ADDR;

                            uint32_t start = ALIGN_UP((uint32_t)start64, PMM_FRAME_SIZE);
                            uint32_t limit = ALIGN_DN((uint32_t)end64, PMM_FRAME_SIZE);

                            for (uint32_t addr = start; addr < limit; addr += PMM_FRAME_SIZE)
                            {
                                   uint32_t frame = addr / PMM_FRAME_SIZE;
                                   bmp_clear(frame);
                                   free_frames++;
                                   total_frames++;
                            }
                     }
              }

              entry = (mmap_entry_t *)((uint32_t)entry + entry->size + 4);
       }

       /* Re-mark frames occupied by the kernel image and static data. */
       uint32_t used_end = ALIGN_UP((uint32_t)&kernel_end, PMM_FRAME_SIZE);
       for (uint32_t addr = 0; addr < used_end; addr += PMM_FRAME_SIZE)
       {
              uint32_t frame = addr / PMM_FRAME_SIZE;
              if (frame < MAX_FRAMES && !bmp_test(frame))
              {
                     bmp_set(frame);
                     free_frames--;
              }
       }

       kprintf("[PMM] %u MB RAM, %u frames free\n",
               (total_frames * PMM_FRAME_SIZE) >> 20, free_frames);
}

uint32_t pmm_alloc_frame(void)
{
       return alloc_frame_limit(MAX_FRAMES);
}

uint32_t pmm_alloc_frame_below(uint32_t addr_limit)
{
       uint32_t frame_limit = addr_limit / PMM_FRAME_SIZE;
       if (frame_limit > MAX_FRAMES)
              frame_limit = MAX_FRAMES;
       return alloc_frame_limit(frame_limit);
}

void pmm_free_frame(uint32_t addr)
{
       uint32_t frame = addr / PMM_FRAME_SIZE;
       if (frame < MAX_FRAMES && bmp_test(frame))
       {
              bmp_clear(frame);
              free_frames++;
       }
}

uint32_t pmm_free_count(void)
{
       return free_frames;
}
