#include <soundpipe.h>
#include <sporth.h>
#include <stdlib.h>
#include "patchwerk.h"
#include "reg.h"

typedef struct {
    plumber_data *pd;
    long counter;
    int val;
} chooser;

static void compute(pw_node *node)
{
    int blksize;
    int n;

    chooser *c;

    c = pw_node_get_data(node);
    blksize = pw_node_blksize(node);

    for (n = 0; n < blksize; n++) {

        if (c->counter == 0) {
            int sr;
            pw_patch *patch;
            int nsecs;
            sp_data *sp;

            sp = c->pd->sp;

            nsecs = 1 + (3*((SPFLOAT)sp_rand(sp) / SP_RANDMAX));
            nsecs *= 8;
            pw_node_get_patch(node, &patch);
            sr = pw_patch_srate_get(patch);
            c->counter = sr * nsecs;

            if (c->val) {
                c->val = 0;
                c->pd->p[1] = 0;
                reg_set(0, 0);
            } else {
                c->val = 1;
                c->pd->p[1] = 1;
                reg_set(0, 1);
            }
        }

        c->counter--;

    }
}

static void destroy(pw_node *node)
{
    chooser *c;

    c = pw_node_get_data(node);

    free(c);
}

void node_chooser(pw_node *node, plumber_data *pd)
{
    chooser *c;

    c = calloc(1, sizeof(chooser));
    c->val = 0;
    c->counter = 0;
    c->pd = pd;

    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);
    pw_node_set_data(node, c);
}
