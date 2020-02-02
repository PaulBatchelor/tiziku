#include <stdlib.h>
#include "patchwerk.h"
#include "add.h"

static void setup(pw_node *node)
{
    add_d *add;

    add = malloc(sizeof(add_d));

    pw_node_cables_alloc(node, 3);
    pw_node_get_cable(node, 0, &add->in1);
    pw_node_get_cable(node, 1, &add->in2);
    pw_node_get_cable(node, 2, &add->out);
    pw_node_set_block(node, 2);

    pw_cable_set_constant(add->in1, 0);
    pw_cable_set_constant(add->in2, 0);

    pw_node_set_data(node, add);
}

static void compute(pw_node *node)
{
    int i;
    int blksize;
    PWFLT in1;
    PWFLT in2;
    add_d *add;

    add = pw_node_get_data(node);
    blksize = pw_node_blksize(node);
    for(i = 0; i < blksize; i++) {
        in1 = pw_cable_get(add->in1, i);
        in2 = pw_cable_get(add->in2, i);
        pw_cable_set(add->out, i, in1 + in2);
    }
}

static void destroy(pw_node *node)
{
    add_d *add;

    add = (add_d *) pw_node_get_data(node);

    pw_node_cables_free(node);

    free(add);
}

void node_add(pw_node *node)
{
    pw_node_set_setup(node, setup);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);
}
