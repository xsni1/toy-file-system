#include "fs.h"
#include <fcntl.h>
#include <unistd.h>

// TODO: memcpy() could be avoided in most of cases. unions could be used.
int main() {
  FileSystem fs;
  Disk *disk = disk_open("file2", 8000);
  char buf[4096];

  fs_format(&fs, disk);
  fs_debug(fs.disk);

  int inode = fs_create(&fs);
  printf("%d\n", inode);

  int inode2 = fs_create(&fs);
  printf("%d\n", inode2);

  int inode3 = fs_create(&fs);
  printf("%d\n", inode3);
  /* disk_read(disk, 0, buf); */

  disk_close(fs.disk);
}
