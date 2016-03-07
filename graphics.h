#ifndef TZ_GRAPHICS
#define TZ_GRAPHICS

#include <GLFW/glfw3.h>
#include "nanovg.h"

typedef struct {
    GLFWwindow *window;
    NVGcontext *vg;
    float g_time;
    float rgb[4];
} tz_graphics;

void tz_run_graphics(tz_graphics *gfx, void (*draw)(NVGcontext*,GLFWwindow*,void*), void *ud);
void tz_stop_graphics(tz_graphics *gfx);

#endif
