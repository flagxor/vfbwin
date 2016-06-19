#include "vfbwin.h"
#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define FBWIN_DEV_PATH "/fb0"

static char *vfbwin_data = 0;
static int vfbwin_width = 0;
static int vfbwin_height = 0;
static volatile int *vfbwin_dirty = 0;
static volatile int *vfbwin_vsync = 0;

static int vfbwin_getattr(const char *path, struct stat *stbuf) {
	memset(stbuf, 0, sizeof(*stbuf));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, FBWIN_DEV_PATH) == 0) {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = vfbwin_width * vfbwin_height * 4;
	} else {
		return -ENOENT;
  }
	return 0;
}

static int vfbwin_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0) {
		return -ENOENT;
  }

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, FBWIN_DEV_PATH + 1, NULL, 0);

	return 0;
}

static int vfbwin_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, FBWIN_DEV_PATH) != 0) {
		return -ENOENT;
  }
	return 0;
}

static int vfbwin_read(
    const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) {
  size_t sz = vfbwin_width * vfbwin_height * 4;
  if (offset < 0 || offset >= sz) {
    return -EIO;
  }
  if (offset + size > sz) {
    size = sz - offset;
  }
  memcpy(buf, vfbwin_data + offset, size);
	return size;
}

static int vfbwin_write(
    const char *path, const char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) {
  size_t sz = vfbwin_width * vfbwin_height * 4;
  if (offset < 0 || offset >= sz) {
    return -EIO;
  }
  if (offset + size > sz) {
    size = sz - offset;
  }
  memcpy(vfbwin_data + offset, buf, size);
  *vfbwin_dirty = 1;
	return size;
}

static int vfbwin_flush(const char *filename, struct fuse_file_info *fi) {
  return 0;
}

static int vfbwin_fsetattr_x(
    const char *name, struct setattr_x *attr,
    struct fuse_file_info *fi) {
  return 0;
}

static struct fuse_operations vfbwin_ops = {
	.getattr = vfbwin_getattr,
	.fsetattr_x = vfbwin_fsetattr_x,
	.readdir = vfbwin_readdir,
	.open	= vfbwin_open,
	.read	= vfbwin_read,
	.write = vfbwin_write,
	.flush = vfbwin_flush,
};

static void *vfbwin_thread(void *arg) {
  char *argv[] = {"vfbwin", "-f", "dev/"};
  exit(fuse_main(3, argv, &vfbwin_ops, NULL));
  return 0;
}

void vfbwin_start(
    void* data, int w, int h, volatile int *dirty, volatile int *vsync) {
  vfbwin_data = data;
  vfbwin_width = w;
  vfbwin_height = h;
  vfbwin_dirty = dirty;
  vfbwin_vsync = vsync;
  pthread_t pth;
  pthread_create(&pth, NULL, vfbwin_thread, NULL);
}
