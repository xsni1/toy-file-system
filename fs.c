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
  printf("Bitmap blocks: %d\n", sp.bitmap_blocks);
  printf("Inodes: %d\n", sp.inodes);

  for (int i = 0; i < sp.bitmap_blocks; i++) {
    char buf[4096];
    disk_read(disk, 1 + sp.inode_blocks + i, buf);
    for (int j = 0; j < 4096; j++) {
        printf("%c", buf[j]);
    }
  }

  /* for (int i = 0; i < sp.inode_blocks; i++) { */
  /*   printf(">> INODE_BLOCK #%d\n", i); */

  /*   char inode_block_buf[4096]; */
  /*   disk_read(disk, 1 + i, inode_block_buf); */

  /*   for (int j = 0; j < INODES_PER_BLOCK; j++) { */
  /*     Inode inode; */
  /*     inode = *(Inode *)(inode_block_buf + (sizeof(Inode) * j)); */

  /*     printf("> INODE #%d\n", j); */
  /*     printf("Valid: %d\n", inode.valid); */
  /*     printf("Size: %d\n", inode.size); */
  /*   } */
  /* } */
}

// instead of 4096 use macro. how?
bool fs_format(Disk *disk) {
  SuperBlock sp;
  int inode_blocks = ceil((disk->blocks - 1) * 0.1);
  int data_blocks = disk->blocks - 1 - inode_blocks;
  int bitmap_blocks = ceil(data_blocks / 4096.0);

  sp.magic_number = 0x309;
  sp.blocks = disk->blocks;
  sp.inode_blocks = inode_blocks;
  sp.bitmap_blocks = bitmap_blocks;
  sp.inodes = inode_blocks * 4096 / 16;

  printf(">>>>> FORMAT\n");
  printf("magic number: %d\n", sp.magic_number);
  printf("blocks: %d\n", sp.blocks);
  printf("inode_blocks: %d\n", sp.inode_blocks);
  printf("bitmap_blocks: %d\n", sp.bitmap_blocks);
  printf("inodes: %d\n", sp.inodes);

  char sp_buf[sizeof(SuperBlock)];
  memcpy(sp_buf, &sp, sizeof(SuperBlock));

  int n = disk_write(disk, 0, sp_buf);
  if (n == -1) {
    return false;
  }

  for (int i = 0; i < sp.inode_blocks; i++) {
    char block[4096];

    for (int j = 0; j < INODES_PER_BLOCK; j++) {
      Inode inode;

      inode.valid = 0;
      inode.size = 0;

      for (int i = 0; i < POINTERS_PER_INODE; i++) {
        inode.direct[i] = 0;
      }
      inode.indirect = 0;

      memcpy(block + sizeof(Inode) * j, &inode, sizeof(Inode));
    }

    disk_write(disk, i + 1, block);
    if (n == -1) {
      return false;
    }
  }

  // seems ugly af
  for (int i = 0; i < sp.bitmap_blocks; i++) {
    char buf[4096];

    for (int j = 0; j < 4096; j++) {
      if (data_blocks <= 0) {
        break;
      }

      buf[j] = '1';
      data_blocks--;
    }

    disk_write(disk, 1 + inode_blocks + i, buf);
  }

  return true;
}
