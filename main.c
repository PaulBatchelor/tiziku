#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <math.h>

#include "chuckwrap.h"
#include "audio.h"
#include "graphics.h"
#include "world.h"

static int jack_cb(jack_nframes_t nframes, void *arg)
{
    int i, chan;
    tz_world *world = arg;
    tz_audio *audio = &world->audio;
    jack_default_audio_sample_t  *out[audio->nchan];
    int bufcount = 0; 

    float buf[nframes * audio->nchan];
    
    chuckwrap_compute(&world->cw, buf, nframes * audio->nchan);

    for(chan = 0; chan < audio->nchan; chan++) {
        out[chan] = jack_port_get_buffer (audio->output_port[chan], nframes);
    }
    for(i = 0; i < nframes; i++) {
        for(chan = 0; chan < audio->nchan; chan++) {
            out[chan][i] = buf[bufcount++];
            //out[chan][i] = 0;
        }
    }

    return 0;
}

static void draw(NVGcontext *vg, GLFWwindow *window, void *ud)
{
    double mx, my, t, dt;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    float pxRatio;
    int x, y;

    tz_world *world = ud;

    the_chuckwrap *cw = &world->cw;

    float scale = cw->stack[0];

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    NVGcolor bgcolor =  nvgRGBAf(252, 251, 227, 255);
    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    pxRatio = (float)fbWidth / (float)winWidth;

    glViewport(0, 0, fbWidth, fbHeight);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
    glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
    nvgBeginPath(vg);
    nvgArc(vg, fbWidth/2, fbHeight/2, 200 * scale, 0, 2 * M_PI, NVG_CCW);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBA(22, 147, 165, 255));
    nvgFill(vg);

    nvgEndFrame(vg);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main()
{
    tz_world world;
    the_chuckwrap *cw = &world.cw;
    chuckwrap_init(cw, MY_SRATE, MY_BUFFERSIZE, MY_CHANNELS_IN, MY_CHANNELS_OUT);
    chuckwrap_compile(cw, "test.ck");
    tz_run_audio(&world.audio, &world, jack_cb);
    tz_run_graphics(&world.graphics, draw, &world);
    tz_stop_graphics(&world.graphics);
    tz_stop_audio(&world.audio);
    return 0;
}


