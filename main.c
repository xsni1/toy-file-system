#include "fs.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// TODO: memcpy() could be avoided in most of cases. unions could be used.
int main() {
  FileSystem fs;
  Disk *disk = disk_open("file2", 20);
  char buf[4096];

  fs_format(&fs, disk);
  /* fs_debug(fs.disk); */

  int inode = fs_create(&fs);
  printf("%d\n", inode);

  int inode2 = fs_create(&fs);
  printf("%d\n", inode2);

  int inode3 = fs_create(&fs);
  printf("%d\n", inode3);

  srand(time(NULL));
  char save[5000];
  for (int i = 0; i < 5000; i++) {
      save[i] = 'A' + rand() % 26;
  }

  fs_write(&fs, 255, save, 5000, 1);

  /* fs_debug(fs.disk); */

  disk_close(fs.disk);
}
