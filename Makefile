UNAME_S := $(shell uname -s)

CFLAGS_COMMON=-D_FILE_OFFSET_BITS=64
ifeq ($(UNAME_S),Darwin)
  CFLAGS=$(CFLAGS_COMMON) \
				 -I/usr/local/include/osxfuse \
				 -D_FILE_OFFSET_BITS=64
  LDFLAGS=-L/usr/local/lib \
					-L/usr/local/opt/osxfuse \
					-mmacosx-version-min=10.5
  LIBS=-lfuse -framework Cocoa
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
