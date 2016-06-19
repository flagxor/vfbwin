CFLAGS=-I/usr/local/include/osxfuse -D_FILE_OFFSET_BITS=64
LDFLAGS=-L/usr/local/lib -mmacosx-version-min=10.5
LIBS=-lfuse -framework Cocoa

vfbwin: vfbwin.c main.m
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

mount: vfbwin
	mkdir -p dev/ && ./vfbwin

clean:
	rm -f vfbwin
	if [ -d dev ]; then rmdir dev; fi
