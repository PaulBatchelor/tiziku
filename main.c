#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "chuckwrap.h"
#include "audio.h"
#include "graphics.h"
#include "world.h"

static tz_world *g_tz;

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

static float scale_prev = 0.0;
static float my_rad = 0;

static void draw(NVGcontext *vg, GLFWwindow *window, void *ud)
{
    //my_rad = fmod(my_rad + 0.0001, 2 * M_PI);
    my_rad = fmod(my_rad + 0.001, 2 * M_PI);
    double mx, my, t, dt;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    float pxRatio;
    int x, y;
    float maxRad = 200;

    tz_world *world = ud;

    the_chuckwrap *cw = &world->cw;

    lua_State *L = world->L;

    float scale = 0.005 * cw->stack[0] + 0.995 * scale_prev;

    scale_prev = scale;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    NVGcolor bgcolor =  nvgRGBAf(239, 250, 180, 255);
    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    pxRatio = (float)fbWidth / (float)winWidth;

    glViewport(0, 0, fbWidth, fbHeight);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
    //glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);

    //nvgBeginPath(vg);
    //nvgArc(vg, 
    //    ((fbWidth - maxRad * sin(my_rad))/2), 
    //    (fbHeight /2) ,     
    //    maxRad * scale, 0, 2 * M_PI, NVG_CCW);
    //nvgFillColor(vg, nvgRGBA(22, 147, 165, 128));
    //nvgFill(vg);
    //
    //nvgBeginPath(vg);
    //nvgArc(vg, 
    //    (fbWidth + maxRad * sin(my_rad))/2, 
    //    fbHeight/2,     
    //    maxRad * scale, 0, 2 * M_PI, NVG_CCW);
    //nvgFillColor(vg, nvgRGBA(22, 147, 165, 128));
    //nvgFill(vg);
    //
    //nvgBeginPath(vg);
    //nvgArc(vg, 
    //    fbWidth/2, 
    //    (fbHeight - maxRad * sin(my_rad))/2,     
    //    maxRad * scale, 0, 2 * M_PI, NVG_CCW);
    //nvgFillColor(vg, nvgRGBA(245, 105, 145, 128));
    //nvgFill(vg);
    //
    //nvgBeginPath(vg);
    //nvgArc(vg, 
    //    fbWidth/2, 
    //    (fbHeight + maxRad * sin(my_rad))/2,     
    //    maxRad * scale, 0, 2 * M_PI, NVG_CCW);
    //nvgFillColor(vg, nvgRGBA(245, 105, 145, 128));
    //nvgFill(vg);

    lua_getglobal(L, "run");
    lua_pcall(L, 0, 0, 0);

    nvgEndFrame(vg);
    glfwSwapBuffers(window);
    glfwPollEvents();
    usleep(8000);
}

static int testfunc(lua_State *L)
{
    int pos_x = lua_tonumber(L, 1);
    int pos_y = lua_tonumber(L, 2);
    float rad = lua_tonumber(L, 3);

    NVGcontext *vg = g_tz->graphics.vg;
    nvgBeginPath(vg);
    nvgArc(vg, pos_x, pos_y, rad, 0, 2 * M_PI, NVG_CCW);
    nvgFillColor(vg, nvgRGBA(245, 105, 145, 128));
    nvgFill(vg);
    return 0;
}

int main()
{
    tz_world world;

    g_tz = &world;
    world.L = luaL_newstate();
    lua_State *L = world.L;

    /* for the io library */
    luaL_requiref(L, "io", luaopen_io, 1);
    /* for the string library */
    luaL_requiref(L, "string", luaopen_string, 1);
    /* for the math  library */
    luaL_requiref(L, "math", luaopen_math, 1);

    luaopen_base(L);
    luaopen_string(L);
    luaopen_math(L);

    lua_register(L, "test", testfunc);

    if(luaL_loadfile(L, "run.lua") || lua_pcall(L, 0, 0, 0))
        error(L, "cannot run file %s", lua_tostring(L, -1));

    the_chuckwrap *cw = &world.cw;
    chuckwrap_init(cw, MY_SRATE, MY_BUFFERSIZE, MY_CHANNELS_IN, MY_CHANNELS_OUT);
    chuckwrap_compile(cw, "test.ck");
    tz_run_audio(&world.audio, &world, jack_cb);
    tz_run_graphics(&world.graphics, draw, &world);
    tz_stop_graphics(&world.graphics);
    tz_stop_audio(&world.audio);
    chuckwrap_destroy(cw);

    lua_close(L);
    return 0;
}


