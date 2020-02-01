#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <x264.h>
#include <cairo/cairo.h>
#include <stdio.h>

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


void tz_gfx_init(tz_graphics *gfx, int w, int h, int fps)
{
    /* setup cairo */
    {
        cairo_surface_t *surface;
        cairo_t *cr;
        int stride;
        int format;

        format = CAIRO_FORMAT_RGB24;

        stride = cairo_format_stride_for_width(format, w);
        gfx->cairo_buf= calloc(1, stride * h);
        printf("size is %d\n", stride);
        surface = cairo_image_surface_create_for_data(
            (unsigned char *)gfx->cairo_buf,
            format,
            w, h,
            stride);

        cr = cairo_create(surface);

        gfx->cr = cr;
        gfx->surface = surface;
    }

    /* setup x264 */
    {

        unsigned int sz;
        unsigned int szd4;
        x264_param_t *p;

        sz = w * h;
        szd4 = sz/4;
        printf("sz: %d, sd4: %d\n", sz, szd4);

        gfx->sz = sz;
        gfx->szd4 = szd4;
        p = &gfx->param;
        gfx->width = w;
        gfx->height = h;
        gfx->fp = fopen("tiziku.h264", "w");

        gfx->i_frame = 0;
        gfx->ybuf = calloc(1, sz);
        gfx->ubuf = calloc(1, szd4);
        gfx->vbuf = calloc(1, szd4);

        if(x264_param_default_preset(p, "veryfast", NULL) < 0)
            return;

        /* p->i_bitdepth = 8; */
        p->i_csp = X264_CSP_I420;
        p->i_width  = w;
        p->i_height = h;
        p->b_vfr_input = 0;
        p->b_repeat_headers = 1;
        p->b_annexb = 1;
        p->i_fps_num = fps;

        if(x264_param_apply_profile(p, "high") < 0 )
            return;


        if(x264_picture_alloc(&gfx->pic, p->i_csp, p->i_width, p->i_height) < 0 )
            return;

        gfx->h = x264_encoder_open(p);
        if(!h) return;
    }
}

void tz_gfx_del(tz_graphics *gfx)
{
    /* cairo cleanup */
    cairo_destroy(gfx->cr);
    cairo_surface_destroy (gfx->surface);
    free(gfx->cairo_buf);

    /* x264 cleanup */
    {
        int i_frame_size;
        while(x264_encoder_delayed_frames(gfx->h))
        {
            i_frame_size = x264_encoder_encode(
                gfx->h,
                &gfx->nal,
                &gfx->i_nal,
                NULL,
                &gfx->pic_out);
            if( i_frame_size ) {
                fwrite(gfx->nal->p_payload,
                       i_frame_size,
                       1,
                       gfx->fp);
            }
        }

        x264_encoder_close(gfx->h);
        x264_picture_clean(&gfx->pic);
        free(gfx->ybuf);
        free(gfx->ubuf);
        free(gfx->vbuf);
        fclose(gfx->fp);
    }
}


/* source: https://www.fourcc.org/fccyvrgb.php */

void rgb2yuv(uint8_t r, uint8_t g, uint8_t b,
             uint8_t *y, uint8_t *u, uint8_t *v)
{
    double Ey;
    double Ecr;
    double Ecb;
    double norm;

    norm = 1.0/255;

    Ey = (0.299*r + 0.587*g + 0.114*b)*norm;
    Ecr = 0.713 * (r*norm - Ey);
    Ecb = 0.564 * (b*norm - Ey);


    *y = Ey * 255;
    *u = (0.5 + Ecb) * 255;
    *v = (0.5 + Ecr) * 255;
}

/* yuv function designed to be applied to cairo surfaces
 * This assumes the format is CAIRO_FORMAT_RGB24
 */

void cairo2yuv(uint32_t *pix,
               unsigned int w, unsigned int h,
               uint8_t *ybuf,
               uint8_t *ubuf,
               uint8_t *vbuf)
{
    unsigned int x, y;
    unsigned int pos;
    unsigned int posB;
    uint8_t yv, uv, vv;
    unsigned char r, g, b;
    uint32_t tmp;

    pos = 0;
    posB = 0;
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            tmp = pix[y * w + x];

            b = tmp & 0xff;
            g = (tmp >> 8) & 0xff;
            r = (tmp >> 16) & 0xff;
            rgb2yuv(r, g, b, &yv, &uv, &vv);
            ybuf[pos] = yv;
            pos++;
            if(x % 2 == 0 && y % 2 == 0) {
                ubuf[posB] = uv;
                vbuf[posB] = vv;
                posB++;
            }
        }
    }
}

void tz_gfx_append(tz_graphics *gfx)
{
    int i_frame_size;

    cairo2yuv(gfx->cairo_buf,
              gfx->width, gfx->height,
              gfx->pic.img.plane[0],
              gfx->pic.img.plane[1],
              gfx->pic.img.plane[2]);

    gfx->pic.i_pts = gfx->i_frame;

    gfx->i_frame++;


    i_frame_size = x264_encoder_encode(gfx->h,
                                       &gfx->nal,
                                       &gfx->i_nal,
                                       &gfx->pic,
                                       &gfx->pic_out);

    if(i_frame_size < 0) return;
    else if(i_frame_size) {
        fwrite(gfx->nal->p_payload, i_frame_size, 1, gfx->fp);
    }
}
