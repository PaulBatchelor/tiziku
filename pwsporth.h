#ifndef PWSPORTH_H
#define PWSPORTH_H
typedef struct {
    plumber_data *pd;
    pw_cable *out;
} pwsporth;

pwsporth * node_pwsporth(pw_node *node, plumber_data *pd);
#endif
