#include <soundpipe.h>
#include <sporth.h>
#include "patchwerk.h"

typedef struct {
    plumber_data *pd;
} chooser;

static void compute(pw_node *node)
{

}

static void destroy(pw_node *node)
{
    chooser *c;

    c = pw_node_get_data(node);

    free(c);
}

int node_chooser(pw_node *node, plumber_data *pd)
{
    chooser *c;

    c = calloc(1, sizeof(chooser));

    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

}
