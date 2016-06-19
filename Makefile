UNAME_S := $(shell uname -s)

CFLAGS_COMMON=-D_FILE_OFFSET_BITS=64
ifeq ($(UNAME_S),Darwin)
  OSXFUSE_LIB_DIR := /usr/local/Cellar/osxfuse/2.8.3/lib
  ifneq ("$(wildcard $(OSXFUSE_LIB_DIR))","")
	  FUSE_LIB_DIR := $(OSXFUSE_LIB_DIR)
    LIBS=-losxfuse -framework Cocoa
  else
	  FUSE_LIB_DIR := /usr/local/lib
    LIBS=-lfuse -framework Cocoa
  endif
  CFLAGS=$(CFLAGS_COMMON) \
				 -I/usr/local/include/osxfuse \
				 -D_FILE_OFFSET_BITS=64
  LDFLAGS=-L$(FUSE_LIB_DIR) \
					-mmacosx-version-min=10.5
	MAIN=main_osx.m
else
  CFLAGS=$(CFLAGS_COMMON)
  LDFLAGS=
  LIBS=-lfuse -lX11 -pthread
	MAIN=main_x11.c
endif

vfbwin: vfbwin.c $(MAIN)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

mount: vfbwin
	mkdir -p dev/ && ./vfbwin

clean:
	rm -f vfbwin
	if [ -d dev ]; then rmdir dev; fi
