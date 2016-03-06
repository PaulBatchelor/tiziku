typedef struct {
    /* Chuck stuff */
    the_chuckwrap cw;
    /* Jack stuff */
    tz_audio audio;
    /* Nanovg stuff */
    tz_graphics graphics;
    /* lua stuff */
    lua_State *L;
} tz_world;

