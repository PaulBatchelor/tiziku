#ifndef ADD_H
#define ADD_H
typedef struct {
    pw_cable *in1;
    pw_cable *in2;
    pw_cable *out;
} add_d;
void node_add(pw_node *node);
#endif
