#ifndef GLITCH_H
#define GLITCH_H
typedef struct {
    int num;
    pw_cable *out;

    /* chuck LPF stuff */

    PWFLT freq, Q;
    PWFLT gain;

    PWFLT y0;
    PWFLT m_a0;
    PWFLT m_y1;
    PWFLT m_y2;
    PWFLT m_b2;
    PWFLT m_b1;

    long count;
    int rpt;
    int rate;
    int rate_count;
    int blink;
} glitch_d;

glitch_d * node_glitch(pw_node *node, int num);
#endif
