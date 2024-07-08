#include "disk.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE 4096

Disk *disk_open(char *path, size_t blocks) {
  int fd = open(path, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    printf("error opening file %s. err code: %d", path, errno);
  }

  Disk *disk = malloc(sizeof(Disk));
  disk->blocks = blocks;
  disk->fd = fd;

  for (int i = 0; i < blocks; i++) {
    char buf[BLOCK_SIZE];
    memset(buf, 0, 4096);
    write(fd, buf, BLOCK_SIZE);
  }

  return disk;
}

void disk_close(Disk *disk) {
  int n = close(disk->fd);
  if (n == -1) {
    printf("error closing file. err code: %d", errno);
  }
}

ssize_t disk_read(Disk *disk, size_t block, char *data) {
  if (disk->blocks - 1 < block) {
    return -1;
  }

  int n = lseek(disk->fd, block * BLOCK_SIZE, SEEK_SET);
  if (n == -1) {
    printf("error lseek: %s\n", strerror(errno));
    return -1;
  }

  n = read(disk->fd, data, BLOCK_SIZE);
  if (n == -1) {
    printf("error read: %s\n", strerror(errno));
    return -1;
  }

  return n;
}

ssize_t disk_write(Disk *disk, size_t block, char *data) {
  if (disk->blocks < block) {
    return -1;
  }

  int n = lseek(disk->fd, BLOCK_SIZE * block, SEEK_SET);
  if (n == -1) {
    printf("error lseek: %s\n", strerror(errno));
    return -1;
  }

  // curious things happen when this uncommencted
  /* n = write(disk->fd, data, BLOCK_SIZE); */

  char buf[BLOCK_SIZE];
  memset(buf, 0, 4096);

  n = write(disk->fd, buf, BLOCK_SIZE);
  if (n == -1) {
    printf("error writing: %s\n", strerror(errno));
    return -1;
  }

  n = lseek(disk->fd, BLOCK_SIZE * block, SEEK_SET);
  if (n == -1) {
    printf("error lseek: %s\n", strerror(errno));
    return -1;
  }

  n = write(disk->fd, data, strlen(data));
  if (n == -1) {
    printf("error writing: %s\n", strerror(errno));
    return -1;
  }

  return 0;
}
