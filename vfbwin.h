#ifndef _vfbwin_h
#define _vfbwin_h

#ifdef __cplusplus
extern "C" {
#endif

extern int vfbwin_start(
    void* data, int w, int h, volatile int *dirty, volatile int *vsync);
extern void vfbwin_stop(void);

#ifdef __cplusplus
}
#endif

#endif
