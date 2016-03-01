#include <math.h>

#include "graphics.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

static void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void tz_run_graphics(tz_graphics *gfx)
{
    GLFWwindow *window = gfx->window;
    gfx->vg = NULL;
    float *g_time = &gfx->g_time;

    *g_time = 0;


	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		exit(-1);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "NanoVG", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(-1);
	}

	glfwSetKeyCallback(window, key);
	glfwSetErrorCallback(errorcb);
	glfwMakeContextCurrent(window);

#ifdef NANOVG_GLEW
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		exit(-1);
	}
#endif

	gfx->vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    
    NVGcontext *vg = gfx->vg;

	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		exit(-1);
	}

	glfwSwapInterval(0);
	glfwSetTime(0);


    NVGcolor bgcolor =  nvgRGBAf(252, 251, 227, 255);

    while(!glfwWindowShouldClose(window)){
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
        int x, y;
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		pxRatio = (float)fbWidth / (float)winWidth;

		glViewport(0, 0, fbWidth, fbHeight);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
			glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
                nvgBeginPath(vg);
                nvgArc(vg, fbWidth/2, fbHeight/2, 40, 0, 2 * M_PI, NVG_CCW);
                nvgClosePath(vg);
                nvgFillColor(vg, nvgRGBA(22, 147, 165, 255));
                nvgFill(vg);

        nvgEndFrame(vg);
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

}

void tz_stop_graphics(tz_graphics *gfx)
{
	nvgDeleteGL2(gfx->vg);
	glfwTerminate();
}

