#include <stdlib.h>
#include <math.h>
#include <soundpipe.h>
#include "patchwerk.h"
#include "glitch.h"
#include "reg.h"

/* inline SAMPLE tick_rlpf( SAMPLE in ) */
/* { */
/*     SAMPLE y0, result; */

/*     // go: adapated from SC3's RLPF */
/*     y0 = m_a0 * in + m_b1 * m_y1 + m_b2 * m_y2; */
/*     result = y0 + 2 * m_y1 + m_y2; */
/*     m_y2 = m_y1; */
/*     m_y1 = y0; */

/*     // be normal */
/*     CK_DDN(m_y1); */
/*     CK_DDN(m_y2); */

/*     return result; */
/* } */

/* inline void set_rlpf( t_CKFLOAT freq, t_CKFLOAT Q ) */
/* { */
/*     t_CKFLOAT qres = ck_max( .001, 1.0/Q ); */
/*     t_CKFLOAT pfreq = freq * g_radians_per_sample; */

/*     t_CKFLOAT D = ::tan(pfreq * qres * 0.5); */
/*     t_CKFLOAT C = (1.0 - D) / (1.0 + D); */
/*     t_CKFLOAT cosf = ::cos(pfreq); */
/*     t_CKFLOAT next_b1 = (1.0 + C) * cosf; */
/*     t_CKFLOAT next_b2 = -C; */
/*     t_CKFLOAT next_a0 = (1.0 + C - next_b1) * 0.25; */

/*     m_freq = freq; */
/*     m_Q = 1.0 / qres; */
/*     m_a0 = (SAMPLE)next_a0; */
/*     m_b1 = (SAMPLE)next_b1; */
/*     m_b2 = (SAMPLE)next_b2; */
/* } */

/* g_radians_per_sample = TWO_PI / (t_CKFLOAT)g_srate; */

static struct {
    PWFLT freq;
    PWFLT gain;
    PWFLT Q;
} params [] = {
    /* originally left channel */
    {1000, 0.4, 10},
    {4000, 0.4, 10},
    /* originally right channel */
    {800, 0.8, 20},
    {600, 0.9, 40},
};

static void set_random_time(pw_node *node, glitch_d *g)
{
    pw_patch *patch;
    int sr;
    int rate;
    int rpt;
    PWFLT wait;
    sp_data *sp;

    pw_node_get_patch(node, &patch);
    sr = pw_patch_srate_get(patch);
    sp = pw_patch_data_get(patch);

    rate = floor(20 + (40 * (sp_rand(sp) / (PWFLT)SP_RANDMAX)));
    rpt = floor(2 + (6 * (sp_rand(sp) / (PWFLT)SP_RANDMAX)));
    wait = 5 + 25 * (sp_rand(sp) / (PWFLT)SP_RANDMAX);

    g->count = floor(wait * sr);
    g->rpt = rpt;
    g->rate = floor(rate * sr * 0.001);
    g->rate_count = 0;
    g->blink = 0;
}

static void compute(pw_node *node)
{
    glitch_d *g;
    int blksize;
    int n;
    PWFLT out;
    PWFLT tick;

    PWFLT y0;

    g = pw_node_get_data(node);
    blksize = pw_node_blksize(node);

    for (n = 0; n < blksize; n++) {
        out = 0;
        tick = 0;

        if (g->count <= 0) {
            if (g->rpt > 0) {
                if (g->rate_count == 0) {
                    tick = 1;
                    g->rpt--;
                    if (g->blink) {
                        g->blink = 0;
                    } else {
                        g->blink = 1;
                    }
                    reg_set(g->num + 1, g->blink);
                }
                g->rate_count++;
                g->rate_count %= g->rate;
            } else {
                set_random_time(node, g);
            }
        } else {
            g->count--;
        }

        out = tick;
        y0 = g->m_a0 * tick + g->m_b1 * g->m_y1 + g->m_b2 * g->m_y2;
        out = y0 + 2 * g->m_y1 + g->m_y2;
        g->m_y2 = g->m_y1;
        g->m_y1 = y0;

        pw_cable_set(g->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    glitch_d *g;
    g = pw_node_get_data(node);
    pw_node_cables_free(node);
    free(g);
}

static void lpf_set(glitch_d *g,
                    int sr,
                    PWFLT freq,
                    PWFLT Q)
{
    PWFLT qres;
    PWFLT pfreq;
    PWFLT D;
    PWFLT C;
    PWFLT cosf;
    PWFLT next_b1;
    PWFLT next_b2;
    PWFLT next_a0;

    qres = 1.0 / Q;

    if (qres < 0.001) qres = 0.001;

    pfreq = freq * (2 * M_PI / (PWFLT) sr);

    D = tan(pfreq * qres * 0.5);
    C = (1.0 - D) / (1.0 + D);
    cosf = cos(pfreq);
    next_b1 = (1.0 + C) * cosf;
    next_b2 = -C;
    next_a0 = (1.0 + C - next_b1) * 0.25;

    g->m_a0 = next_a0;
    g->m_b1 = next_b1;
    g->m_b2 = next_b2;
}

glitch_d * node_glitch(pw_node *node, int num)
{
    glitch_d *g;
    int sr;
    pw_patch *patch;

    g = calloc(1, sizeof(glitch_d));
    g->num = num;
    pw_node_get_patch(node, &patch);
    sr = pw_patch_srate_get(patch);

    pw_node_cables_alloc(node, 1);
    pw_node_set_block(node, 0);
    pw_node_get_cable(node, 0, &g->out);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);
    pw_node_set_data(node, g);

    lpf_set(g, sr, params[num].freq, params[num].Q);

    g->gain = params[num].gain;

    set_random_time(node, g);

    return g;
}
