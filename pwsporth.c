#include <soundpipe.h>
#include <sporth.h>
#include <stdlib.h>

#include "patchwerk.h"
#include "pwsporth.h"

static void compute(pw_node *node)
{
    int n;
    int blksize;
    pwsporth *pws;
    PWFLT tmp;

    blksize = pw_node_blksize(node);
    pws = pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        plumber_compute(pws->pd, PLUMBER_COMPUTE);
        tmp = sporth_stack_pop_float(&pws->pd->sporth.stack);
        pw_cable_set(pws->out, n, tmp);
    }
}

static void destroy(pw_node *node)
{
    pwsporth *sporth;
    pw_node_cables_free(node);

    sporth = pw_node_get_data(node);

    free(sporth);
}

pwsporth * node_pwsporth(pw_node *node, plumber_data *pd)
{
    pwsporth *sporth;
    sporth = calloc(1, sizeof(pwsporth));
    sporth->pd = pd;
    pw_node_cables_alloc(node, 1);
    pw_node_set_block(node, 0);
    pw_node_get_cable(node, 0, &sporth->out);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);
    pw_node_set_data(node, sporth);
    return sporth;
}
