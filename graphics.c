#include <math.h>
#include <stdint.h>
#include <x264.h>
#include <cairo/cairo.h>

#include "graphics.h"
/* #define NANOVG_GL2_IMPLEMENTATION */
/* #include "nanovg_gl.h" */

/* static void errorcb(int error, const char* desc) */
/* { */
/* 	printf("GLFW error %d: %s\n", error, desc); */
/* } */

/* static void key(GLFWwindow* window, int key, int scancode, int action, int mods) */
/* { */
/* 	NVG_NOTUSED(scancode); */
/* 	NVG_NOTUSED(mods); */
/* 	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) */
/* 		glfwSetWindowShouldClose(window, GL_TRUE); */
/* } */

/* void tz_run_graphics( */
/*         tz_graphics *gfx, */
/*         void (*draw)(NVGcontext*,GLFWwindow*,void*), */
/*         void *ud) */
/* { */
/*     /\* GLFWwindow *window = gfx->window; *\/ */
/*     /\* gfx->vg = NULL; *\/ */
/*     /\* float *g_time = &gfx->g_time; *\/ */

/*     /\* *g_time = 0; *\/ */


/* 	/\* if (!glfwInit()) { *\/ */
/* 	/\* 	printf("Failed to init GLFW."); *\/ */
/* 	/\* 	exit(-1); *\/ */
/* 	/\* } *\/ */

/*     /\* glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); *\/ */
/* 	/\* glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); *\/ */

/*     /\* window = glfwCreateWindow(800, 600, "NanoVG", NULL, NULL); *\/ */

/* 	/\* if (!window) { *\/ */
/* 	/\* 	glfwTerminate(); *\/ */
/* 	/\* 	exit(-1); *\/ */
/* 	/\* } *\/ */

/* 	/\* glfwSetKeyCallback(window, key); *\/ */
/* 	/\* glfwSetErrorCallback(errorcb); *\/ */
/* 	/\* glfwMakeContextCurrent(window); *\/ */
/*     /\* glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); *\/ */

/* 	/\* gfx->vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG); *\/ */

/*     /\* NVGcontext *vg = gfx->vg; *\/ */

/* 	/\* if (vg == NULL) { *\/ */
/* 	/\* 	printf("Could not init nanovg.\n"); *\/ */
/* 	/\* 	exit(-1); *\/ */
/* 	/\* } *\/ */

/* 	/\* glfwSwapInterval(0); *\/ */
/* 	/\* glfwSetTime(0); *\/ */


/*     /\* while(!glfwWindowShouldClose(window)){ *\/ */
/*     /\*     draw(vg, window, ud); *\/ */
/*     /\* } *\/ */

/* } */

/* void tz_stop_graphics(tz_graphics *gfx) */
/* { */
/* 	/\* nvgDeleteGL2(gfx->vg); *\/ */
/* 	/\* glfwTerminate(); *\/ */
/* } */
