#include "fs.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
  FileSystem fs;
  Disk *disk = disk_open("file2", 8000);
  char buf[4096];

  fs_format(&fs, disk);
  fs_debug(fs.disk);

  /* disk_read(disk, 0, buf); */

  disk_close(fs.disk);
}
