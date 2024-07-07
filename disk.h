#include <stdio.h>

typedef struct Disk Disk;
struct Disk {
  int fd;
  size_t blocks;
};

Disk *disk_open(char *path, size_t blocks);
void disk_close(Disk *disk);

ssize_t disk_read(Disk *disk, size_t block, char *data);
ssize_t disk_write(Disk *disk, size_t block, char *data);
