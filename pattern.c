#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define WIDTH 1024
#define HEIGHT 768


static int clip(double x) {
  x *= 255;
  if (x < 0) {
    return 0;
  } else if (x >= 255.0f) {
    return 255;
  } else {
    return (int) x;
  }
}


int main(int argc, char *argv[]) {
  int fd = open("dev/fb0", O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "cant open device\n");
    return 1;
  }

  unsigned char *buffer;

  int map = 0;
  if (argc > 1 && strcmp(argv[1], "mmap") == 0) {
    map = 1;
    buffer = mmap(0, WIDTH * HEIGHT * 4, PROT_WRITE, MAP_SHARED, fd, 0);
  } else if (argc > 1 && strcmp(argv[1], "write") == 0) {
    buffer = (unsigned char *) malloc(WIDTH * HEIGHT * 4);
  } else {
    fprintf(stderr, "usage: 4spire mmap/write\n");
    return 1;
  }
  assert(buffer);

  int i, j;
  int offset = 0;
  for (;;) {
    uint32_t *pos = (uint32_t*) buffer;
    for (j = 0; j < HEIGHT; ++j) {
      for (i = 0; i < WIDTH; ++i) {
        *pos = (i * (j + offset)) | 0xff000000;
        ++pos;
      }
    }
    if (map) {
      msync(buffer, WIDTH * HEIGHT * 4, MS_SYNC);
    } else {
      lseek(fd, 0, SEEK_SET);
      if (write(fd, buffer, WIDTH * HEIGHT * 4) < 0) {
        return 0;
      }
    }
    offset += 1;
  }

  return 0;
}
