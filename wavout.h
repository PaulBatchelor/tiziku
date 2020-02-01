#ifndef WAVOUT_H
typedef struct {
    sp_data *sp;
    sp_wavout *wavout;
    pw_cable *in;
    pw_cable *out;
} wavout_d;
void node_wavout(sp_data *sp, pw_node *node, const char* filename);
#endif
