#ifndef TZ_GRAPHICS
#define TZ_GRAPHICS

typedef struct {
    float g_time;
    float rgb[4];

    /* x264 stuff */
    FILE *fp;
    int width, height;
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t *h;
    int i_frame;
    x264_nal_t *nal;
    int i_nal;
    uint8_t *ybuf;
    uint8_t *ubuf;
    uint8_t *vbuf;
    unsigned int sz;
    unsigned int szd4;

    /* cairo stuff */
    cairo_surface_t *surface;
    cairo_t *cr;
    uint32_t *cairo_buf;
} tz_graphics;

/* void tz_run_graphics(tz_graphics *gfx, void (*draw)(NVGcontext*,GLFWwindow*,void*), void *ud); */
/* void tz_stop_graphics(tz_graphics *gfx); */

void tz_gfx_init(tz_graphics *gfx, int w, int h, int fps);
void tz_gfx_del(tz_graphics *gfx);
void tz_gfx_append(tz_graphics *gfx);
#endif
