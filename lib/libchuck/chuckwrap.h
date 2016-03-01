typedef struct {
    void *chuck;
    float *stack;
} the_chuckwrap;

void chuckwrap_init(the_chuckwrap *cw, int sr, int bufsize, int in, int out);
void chuckwrap_compile(the_chuckwrap *cw, const char *filename);
void chuckwrap_compute(the_chuckwrap *cw, float *out, int bufsize);
