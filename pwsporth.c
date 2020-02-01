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
        plumber_compute(&pws->pd, PLUMBER_COMPUTE);
        tmp = sporth_stack_pop_float(&pws->pd.sporth.stack);
        pw_cable_set(pws->out, n, tmp);
    }
}

static void destroy(pw_node *node)
{
    pwsporth *sporth;
    pw_node_cables_free(node);

    sporth = pw_node_get_data(node);

    plumber_clean(&sporth->pd);
    free(sporth);
}

pwsporth * node_pwsporth(pw_node *node,
                         sp_data *sp,
                         const char *filename)
{
    pwsporth *sporth;
    plumber_data *pd;
    sporth = calloc(1, sizeof(pwsporth));
    pd = &sporth->pd;
    pw_node_cables_alloc(node, 1);
    pw_node_set_block(node, 0);
    pw_node_get_cable(node, 0, &sporth->out);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);


    plumber_register(pd);
    plumber_init(pd);
    pd->sp = sp;
    plumber_open_file(pd, filename);
    plumber_parse(pd);
    plumber_compute(pd, PLUMBER_INIT);
    plumber_close_file(pd);

    pw_node_set_data(node, sporth);

    return sporth;
}
