#include "fs.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
  Disk *disk = disk_open("file2", 10);
  char buf[4096];

  fs_format(disk);
  fs_debug(disk);

  /* disk_read(disk, 0, buf); */

  disk_close(disk);
}
