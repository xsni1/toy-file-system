#include "disk.h"
#include <stdbool.h>
#include <stdint.h>

#define MAGIC_NUMBER (0xf0f03410)
#define INODES_PER_BLOCK (128)
#define POINTERS_PER_INODE (5)
#define POINTERS_PER_BLOCK (1024)

typedef struct SuperBlock SuperBlock;
struct SuperBlock {
  uint32_t magic_number;
  uint32_t blocks;
  uint32_t inode_blocks;
  uint32_t bitmap_blocks;
  uint32_t inodes;
};

typedef struct Inode Inode;
struct Inode {
  uint32_t valid;
  uint32_t size;
  uint32_t direct[POINTERS_PER_INODE];
  uint32_t indirect;
};

typedef struct FileSystem FileSystem;
struct FileSystem {
  Disk *disk;
  bool *free_blocks;
  SuperBlock meta_data;
};

void fs_debug(Disk *disk);
bool fs_format(Disk *disk);

bool fs_mount(FileSystem *fs, Disk *disk);
void fs_unmount(FileSystem *fs);

ssize_t fs_create(FileSystem *fs);
bool fs_remove(FileSystem *fs, size_t inode_number);
ssize_t fs_stat(FileSystem *fs, size_t inode_number);

ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length,
                size_t offset);
ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length,
                 size_t offset);
