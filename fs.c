#include "fs.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

int min(int a, int b) {
  if (a > b) {
    return b;
  }

  return a;
}

// blocks have to be indexed starting from 1, to be able to differentiate from
// uninitialized pointers
ssize_t find_free_block(FileSystem *fs) {
  for (int i = 0; i < fs->meta_data.bitmap_blocks; i++) {
    char buf[4096];

    if (disk_read(fs->disk, 1 + fs->meta_data.inode_blocks + i, buf) == -1) {
      printf("err disk_read\n");
    }

    for (int j = 0;
         min(4096, (fs->meta_data.blocks - 1 - fs->meta_data.inode_blocks -
                    fs->meta_data.bitmap_blocks) -
                       (i * 4096)) > j;
         j++) {
      if (buf[j] == '1') {
        buf[j] = '0';

        if (disk_write(fs->disk, 1 + fs->meta_data.inode_blocks + i, buf) ==
            -1) {
          printf("err disk_write\n");
        }

        return (i * 4096) + j + 1;
      }
    }
  }

  return -1;
}

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

  for (int i = 0; i < sp.inode_blocks; i++) {
    printf(">> INODE_BLOCK #%d\n", i);

    char inode_block_buf[4096];
    disk_read(disk, 1 + i, inode_block_buf);

    for (int j = 0; j < INODES_PER_BLOCK; j++) {
      Inode inode;
      inode = *(Inode *)(inode_block_buf + (sizeof(Inode) * j));

      printf("> INODE #%d\n", j);
      printf("Valid: %d\n", inode.valid);
      printf("Size: %d\n", inode.size);

      for (int j = 0; j < POINTERS_PER_INODE; j++) {
        printf("Pointer[%d]: %d\n", j, inode.direct[j]);
      }
    }
  }

  /* for (int i = 0; i < sp.bitmap_blocks; i++) { */
  /*   char buf[4096]; */
  /*   disk_read(disk, 1 + sp.inode_blocks + i, buf); */

  /*   for (int j = 0; */
  /*        j < min(4096, (sp.blocks - 1 - sp.inode_blocks - sp.bitmap_blocks) -
   */
  /*                          (i * 4096)); */
  /*        j++) { */
  /*     printf("%c", buf[j]); */
  /*   } */
  /* } */
}

// Inodes are indexed startin from 0
// ______________  ________________  _________________ _________________
// | SUPERBLOCK |  | INODES TABLE |  | BITMAP BLOCKS | |  DATA BLOCKS  |
// |____________|  |______________|  |_______________| |_______________|
//
// [  1  BLOCK  ]  [ inode_blocks ]  [ bitmap_blocks ]

// instead of 4096 use macro. how?
bool fs_format(FileSystem *fs, Disk *disk) {
  SuperBlock sp;
  int inode_blocks = ceil((disk->blocks - 1) * 0.1);
  int data_blocks = disk->blocks - 1 - inode_blocks;
  int bitmap_blocks = ceil(data_blocks / 4096.0);
  data_blocks -= bitmap_blocks;

  sp.magic_number = 0x309;
  sp.blocks = disk->blocks;
  sp.inode_blocks = inode_blocks;
  sp.bitmap_blocks = bitmap_blocks;
  sp.inodes = inode_blocks * 4096 / sizeof(Inode);

  printf(">>>>> FORMAT\n");
  printf("magic number: %d\n", sp.magic_number);
  printf("blocks: %d\n", sp.blocks);
  printf("inode_blocks: %d\n", sp.inode_blocks);
  printf("bitmap_blocks: %d\n", sp.bitmap_blocks);
  printf("inodes: %d\n", sp.inodes);
  printf("data blocks: %d\n", data_blocks);

  int n = disk_write(disk, 0, (char *)&sp);
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

  for (int i = 0; i < sp.bitmap_blocks; i++) {
    char buf[4096];

    // data_blocks % (i * 4096) ??
    for (int j = 0; j < min(4096, data_blocks - (i * 4096)); j++) {
      buf[j] = '1';
    }

    disk_write(disk, 1 + inode_blocks + i, buf);
  }

  fs->disk = disk;
  fs->meta_data = sp;

  return true;
}

ssize_t fs_create(FileSystem *fs) {
  for (int i = 0; i < fs->meta_data.inode_blocks; i++) {
    char buf[4096];
    disk_read(fs->disk, i + 1, buf);

    for (int j = 0; j < INODES_PER_BLOCK; j++) {
      Inode *inode = (Inode *)(buf + (sizeof(Inode) * j));

      if (inode->valid == 0) {
        inode->valid = 1;
        disk_write(fs->disk, i + 1, buf);

        return (i + 1) * j;
      }
    }
  }

  return -1;
}

ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length,
                 size_t offset) {
  char buf[4096];
  int inode_block = sizeof(Inode) * inode_number / 4096;

  if (inode_block >= fs->meta_data.inode_blocks) {
    printf("inode_number out of bounds\n");
    return -1;
  }

  int inode_number_in_block = inode_number % INODES_PER_BLOCK;

  disk_read(fs->disk, 1 + inode_block, buf);

  Inode *inode = (Inode *)(buf + (inode_number_in_block * sizeof(Inode)));
  inode->size = length;

  int n = 0;
  for (int i = 0; i < INODES_PER_BLOCK; i++) {
    char buf[4096];
    int save = min(4096, length);
    printf("saving: %d\n", save);
    int free_block = find_free_block(fs);

    memcpy(buf, &data[n], save);
    disk_write(fs->disk,
               free_block + fs->meta_data.inode_blocks +
                   fs->meta_data.bitmap_blocks,
               buf);

    inode->direct[i] = free_block;

    length -= save;
    n += save;
    if (length <= 0) {
      break;
    }
  }

  disk_write(fs->disk, 1 + inode_block, buf);

  return 0;
}

ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length,
                size_t offset) {
  int inode_block = inode_number / INODES_PER_BLOCK;
  int inode_number_in_block = inode_number - (inode_block * INODES_PER_BLOCK);
  char buf[4096];

  disk_read(fs->disk, 1 + inode_block, buf);
  Inode *inode = (Inode *)(buf + (inode_number_in_block * sizeof(Inode)));

  printf("block: %d inode: %d\n", inode_block, inode_number_in_block);

  int n = 0;
  for (int i = 0; i < POINTERS_PER_INODE; i++) {
    char block[4096];
    int data_block = inode->direct[i];
    int save = min(4096, length);
    if (data_block == 0) {
      break;
    }

    printf("reading from n: %d, reading bytes: %d\n", n, save);

    // problem taki ze disk read czyta prosto do data, ktore moze byc mniejsze!!!
    if (disk_read(fs->disk, data_block + fs->meta_data.inode_blocks + fs->meta_data.bitmap_blocks, block) == -1) {
    /* if (disk_read(fs->disk, data_block + fs->meta_data.inode_blocks + fs->meta_data.bitmap_blocks, &data[n]) == -1) { */
      printf("error disk_read\n");
      return -1;
    }

    memcpy(&data[n], block, save);

    length -= save;
    n += save;

    if (length <= 0) {
      break;
    }
  }

  return 0;
}
