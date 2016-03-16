#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <math.h>
#include <string.h>

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

    float buf[nframes * 2];
    memset(buf, 0, sizeof(float) * nframes * 2); 


    for(chan = 0; chan < audio->nchan; chan++) {
        out[chan] = jack_port_get_buffer (audio->output_port[chan], nframes);
    }

    int bufcount = 0; 
    chuckwrap_compute(&world->cw, buf, nframes);
    for(i = 0; i < nframes; i++) {
        out[0][i] = buf[bufcount++];
        out[1][i] = buf[bufcount++];
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
    float maxRad = 200;

    tz_world *world = ud;

    lua_State *L = world->L;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    NVGcolor bgcolor =  nvgRGBAf(239, 250, 180, 255);
    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    pxRatio = (float)fbWidth / (float)winWidth;

    glViewport(0, 0, fbWidth, fbHeight);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

    lua_pushnumber(L, winWidth);
    lua_setglobal(L, "width");
    lua_pushnumber(L, winHeight);
    lua_setglobal(L, "height");

    lua_getglobal(L, "run");
    lua_pcall(L, 0, 0, 0);

    nvgEndFrame(vg);
    usleep(8000);
    glfwSwapBuffers(window);
    glfwPollEvents();
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

static int rect(lua_State *L)
{
    float x1 = lua_tonumber(L, 1);
    float y1 = lua_tonumber(L, 2);
    float x2 = lua_tonumber(L, 3);
    float y2 = lua_tonumber(L, 4);

    NVGcontext *vg = g_tz->graphics.vg;
    float *rgb = g_tz->graphics.rgb;
    unsigned char r = rgb[0];
    unsigned char g = rgb[1];
    unsigned char b = rgb[2];
    unsigned char a = rgb[3];

    nvgBeginPath(vg);
    nvgRect(vg, x1, y1, x2, y2);
    nvgFillColor(vg, nvgRGBA(r, g, b, a));
    nvgFill(vg);

    return 0;
}

static int setrgba(lua_State *L)
{
    float r = lua_tonumber(L, 1);
    float g = lua_tonumber(L, 2);
    float b = lua_tonumber(L, 3);
    float a = lua_tonumber(L, 4);

    float *rgb = g_tz->graphics.rgb;

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    rgb[3] = a;

    return 0;
}

static int get_chan(lua_State *L)
{
    int pos = lua_tonumber(L, 1);
    float *stack = g_tz->cw.stack;
    lua_pushnumber(L, stack[pos]);
    return 1;
}

static int my_rand(lua_State *L)
{
    lua_pushnumber(L, rand());
    return 1;
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
    /* for the os library */
    luaL_requiref(L, "os", luaopen_os, 1);

    luaopen_base(L);
    luaopen_string(L);
    luaopen_math(L);

    lua_register(L, "test", testfunc);
    lua_register(L, "rect", rect);
    lua_register(L, "rgba", setrgba);
    lua_register(L, "get_chan", get_chan);
    lua_register(L, "rand", my_rand);

    if(luaL_loadfile(L, "run.lua") || lua_pcall(L, 0, 0, 0))
        fprintf(stderr, "cannot run file %s", lua_tostring(L, -1));

    the_chuckwrap *cw = &world.cw;
    chuckwrap_init(cw, MY_SRATE, MY_BUFFERSIZE, MY_CHANNELS_IN, MY_CHANNELS_OUT);
    chuckwrap_compile(cw, "run.ck");
    tz_run_audio(&world.audio, &world, jack_cb);
    tz_run_graphics(&world.graphics, draw, &world);
    tz_stop_audio(&world.audio);
    tz_stop_graphics(&world.graphics);
    chuckwrap_destroy(cw);

    lua_close(L);
    return 0;
}


