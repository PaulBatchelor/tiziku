#include <stdio.h>
#ifdef NANOVG_GLEW
#  include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

double g_time = 0;

void errorcb(int error, const char* desc)
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

int main()
{
    GLFWwindow *window;
    NVGcontext *vg = NULL;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "NanoVG", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key);
	glfwSetErrorCallback(errorcb);
	glfwMakeContextCurrent(window);

#ifdef NANOVG_GLEW
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
#endif

#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
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
        double rad = 0.1 * (g_time * (2 * M_PI / 1200));
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		pxRatio = (float)fbWidth / (float)winWidth;

		glViewport(0, 0, fbWidth, fbHeight);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
			glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
        for(y = 0; y < 16; y++) {
            for(x = 0; x < 16; x++)  {
                double scale = 0.5 * (sin(rad + 5 * x + 2 * y) + 1);
                nvgBeginPath(vg);
                nvgArc(vg, 20 + 40 + x * 60 + 40 * scale, 20 + 40 + y * 60 + 40 * scale, 40 * scale, 0, 2 * M_PI, NVG_CCW);
                nvgClosePath(vg);
                if(x % 2== 0) {
                    nvgFillColor(vg, nvgRGBA(22 * scale, 147 * scale, 165 * scale, 255 * scale));
                } else {
                    nvgFillColor(vg, nvgRGBA(255 * scale, 64 * scale, 64 * scale, 255 * scale));

                }
                nvgFill(vg);
            }
        }

        nvgEndFrame(vg);
		glfwSwapBuffers(window);
		glfwPollEvents();
        g_time++;
    }

	nvgDeleteGL2(vg);

	glfwTerminate();

    return 0;
}
