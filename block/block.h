#ifndef BLOCK_H
#define BLOCK_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLOCK_DEV_NAME_LEN 16
#define BLOCK_SIZE 512

typedef struct block_dev {
    char name[BLOCK_DEV_NAME_LEN];
    uint32_t block_size;
    uint64_t total_blocks;
    int (*read)(struct block_dev *dev, uint64_t block, uint32_t count, uint8_t *buf);
    int (*write)(struct block_dev *dev, uint64_t block, uint32_t count, const uint8_t *buf);
} block_dev_t;

void block_init(void);
int block_register(block_dev_t *dev);
block_dev_t *block_find(const char *name);
int block_read(const char *name, uint64_t block, uint32_t count, uint8_t *buf);
int block_write(const char *name, uint64_t block, uint32_t count, const uint8_t *buf);

#ifdef __cplusplus
}
#endif
#endif