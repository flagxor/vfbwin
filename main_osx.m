#import <Cocoa/Cocoa.h>
#include "vfbwin.h"

#define WIDTH 1024
#define HEIGHT 768


@interface ImageGeneratorView : NSView {
 @private
  NSImage *lastImage_;
  NSBitmapImageRep *imageRep_;
  char* data_;
  volatile int dirty_;
  int vsync_;
}

@end

@implementation ImageGeneratorView

- (id)init {
  data_ = (char *) malloc(WIDTH * HEIGHT * 4);
  imageRep_ = [[NSBitmapImageRep alloc]
      initWithBitmapDataPlanes:NULL
                    pixelsWide:WIDTH
                    pixelsHigh:HEIGHT
                 bitsPerSample:8
               samplesPerPixel:4
                      hasAlpha:YES
                      isPlanar:NO
                colorSpaceName:NSCalibratedRGBColorSpace
                   bytesPerRow:WIDTH*4
                  bitsPerPixel:0];
  if (vfbwin_start(data_, WIDTH, HEIGHT, &dirty_, &vsync_)) {
    fprintf(stderr, "fuse mount failed\n");
    exit(1);
    return self;
  }

  if (self = [super init]) {
    [NSTimer scheduledTimerWithTimeInterval:1.0/60.0 target:self
        selector:@selector(redraw) userInfo:nil repeats:YES];
  }
  return self;
}

- (void)dealloc {
  free(data_);
  [imageRep_ release];
  [lastImage_ release];
  [super dealloc];
}

- (void)viewDidEndLiveResize {
  [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect {
  if (!lastImage_ || ![self inLiveResize]) {
    [lastImage_ release];
    lastImage_ = [[NSImage alloc] init];
    [lastImage_ addRepresentation:imageRep_];
  }

  memcpy([imageRep_ bitmapData], data_, WIDTH * HEIGHT * 4);

  [lastImage_ drawInRect:rect fromRect:NSZeroRect
      operation:NSCompositeCopy fraction:1.0];
}

- (void)redraw {
  if (dirty_) {
    dirty_ = 0;
    [self setNeedsDisplay:YES];
  }
}

- (void) mouseDown: (NSEvent*) theEvent {
  [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}

@end


@interface MyApplicationDelegate :
    NSObject <NSApplicationDelegate, NSWindowDelegate> {
  NSWindow* window;
}
@end

@implementation MyApplicationDelegate : NSObject
- (id)init {
  if (self = [super init]) {
    NSUInteger windowStyle = NSTitledWindowMask |
                             NSClosableWindowMask |
                             NSResizableWindowMask |
                             NSMiniaturizableWindowMask;
    NSRect windowRect = NSMakeRect(100, 100, 1024, 768);
    window = [[NSWindow alloc] initWithContentRect:windowRect
        styleMask:windowStyle
        backing:NSBackingStoreBuffered defer:NO];
    [window setTitle:@"vfbwin"];
    NSWindowController* windowController =
        [[NSWindowController alloc] initWithWindow:window];
    ImageGeneratorView* view = [[ImageGeneratorView alloc] init];
    [window setContentView:view];
    [window setOrderedIndex:0];
    [window makeKeyAndOrderFront:NSApp];
  }
  return self;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
  [window makeKeyAndOrderFront:self];
}

- (void)dealloc {
  vfbwin_stop();
  [window release];
  [super dealloc];
}

@end


int main(int argc, char * argv[]) {
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  NSApplication* application = [NSApplication sharedApplication];
  MyApplicationDelegate* appDelegate =
      [[[MyApplicationDelegate alloc] init] autorelease];
  [application setDelegate:appDelegate];
  [application run];
  [pool drain];
  return 0;
}
