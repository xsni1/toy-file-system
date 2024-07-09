#include "fs.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

void fs_debug(Disk *disk) {
  SuperBlock sp;
  char sp_buf[4096];

  disk_read(disk, 0, sp_buf);

  sp = *(SuperBlock *)sp_buf;

  printf("\n>>>>> DEBUG\n");
  printf(">> SUPERBLOCK\n");
  printf("Magic number: %d\n", sp.magic_number);
  printf("Blocks: %d\n", sp.blocks);
  printf("Inode blocks: %d\n", sp.inode_blocks);
  printf("Inodes: %d\n", sp.inodes);
}

// instead of 4096 use macro. how?
bool fs_format(Disk *disk) {
  SuperBlock sp;
  int inode_blocks = ceil((disk->blocks - 1) * 0.1);

  sp.magic_number = 0x309;
  sp.blocks = disk->blocks;
  sp.inode_blocks = inode_blocks;
  sp.inodes = inode_blocks * 4096 / 16;

  printf(">>>>> FORMAT\n");
  printf("magic number: %d\n", sp.magic_number);
  printf("blocks: %d\n", sp.blocks);
  printf("inode_blocks: %d\n", sp.inode_blocks);
  printf("inodes: %d\n", sp.inodes);

  char sp_buf[sizeof(SuperBlock)];
  memcpy(sp_buf, &sp, sizeof(SuperBlock));




  int n = disk_write(disk, 0, sp_buf);
  if (n == -1) {
    return false;
  }

  for (int i = 0; i < sp.inode_blocks; i++) {
    char inode_buf[sizeof(Inode)];
    Inode inode;

    inode.valid = 0;
    inode.size = 0;
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
      inode.direct[i] = 0;
    }
    inode.indirect = 0;

    memcpy(inode_buf, &inode, sizeof(Inode));

    disk_write(disk, i + 1, inode_buf);
    if (n == -1) {
      return false;
    }
  }

  return true;
}
