#ifndef VFS_H
#define VFS_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FS_FILE 0
#define FS_DIR 1
#define FS_CHARDEV 2
#define FS_BLOCKDEV 3

    struct fs_node;

    typedef struct dirent
    {
        char name[128];
        uint32_t ino;
    } dirent_t;

    typedef struct fs_node
    {
        char name[128];
        uint32_t mask;
        uint32_t uid;
        uint32_t gid;
        uint32_t flags;
        uint32_t inode;
        uint32_t length;
        uint32_t impl;
        struct fs_node *ptr;
        struct fs_node *parent;
        struct fs_node *child;
        struct fs_node *next;

        uint32_t (*read)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
        uint32_t (*write)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
        void (*open)(struct fs_node *);
        void (*close)(struct fs_node *);
        dirent_t *(*readdir)(struct fs_node *, uint32_t);
        struct fs_node *(*finddir)(struct fs_node *, char *name);
    } fs_node_t;

    extern fs_node_t *fs_root;

    void vfs_init(void);
    uint32_t vfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
    uint32_t vfs_write(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
    void vfs_open(fs_node_t *node);
    void vfs_close(fs_node_t *node);
    dirent_t *vfs_readdir(fs_node_t *node, uint32_t index);
    fs_node_t *vfs_finddir(fs_node_t *node, char *name);

#ifdef __cplusplus
}
#endif
#endif