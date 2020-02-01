#include <stdlib.h>
#include <soundpipe.h>
#include "patchwerk.h"
#include "wavout.h"

static void destroy(pw_node *node)
{
    wavout_d *wavout;

    wavout = (wavout_d *) pw_node_get_data(node);

    sp_wavout_destroy(&wavout->wavout);

    pw_node_cables_free(node);

    free(wavout);
}

static void compute(pw_node *node)
{
    int i;
    int blksize;
    wavout_d *wavout;
    SPFLOAT in;
    SPFLOAT out;

    wavout = pw_node_get_data(node);
    blksize = pw_node_blksize(node);

    for(i = 0; i < blksize; i++) {
        in = pw_cable_get(wavout->in, i);
        sp_wavout_compute(wavout->sp, wavout->wavout, &in, &out);
        pw_cable_set(wavout->out, i, out);
    }
}

void node_wavout(sp_data *sp, pw_node *node, const char* filename)
{
    wavout_d *wavout;

    wavout = malloc(sizeof(wavout_d));
    wavout->sp = sp;

    sp_wavout_create(&wavout->wavout);
    sp_wavout_init(sp, wavout->wavout, filename);
    pw_node_cables_alloc(node, 2);
    pw_node_get_cable(node, 0, &wavout->in);

    pw_node_get_cable(node, 1, &wavout->out);
    pw_node_set_block(node, 1);

    pw_node_set_data(node, wavout);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);
}
