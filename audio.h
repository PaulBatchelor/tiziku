typedef struct {
    jack_port_t **output_port;
    jack_client_t **client;
    const char **ports;
    int nchan;
} tz_audio;

void tz_run_audio(tz_audio *audio, void *ud, int (*callback)(jack_nframes_t, void *));
void tz_stop_audio(tz_audio *audio);
