#ifndef _vfbwin_h
#define _vfbwin_h

#ifdef __cplusplus
extern "C" {
#endif

extern void vfbwin_start(
    void* data, int w, int h, volatile int *dirty, volatile int *vsync);

#ifdef __cplusplus
}
#endif

#endif
