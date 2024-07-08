/* #include "disk.h" */
#include "fs.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
  Disk *disk = disk_open("file2", 10);
  char buf[4096];

  /* disk_write(disk, 1, "129312893129"); */
  /* disk_read(disk, 1, buf); */
  /* disk_write(disk, 2, "kjsdsjhdjhas"); */
  /* disk_write(disk, 1, "dupa"); */

  fs_format(disk);

  disk_read(disk, 0, buf);

  disk_close(disk);
}
