#ifndef TZ_AUDIO_H
#define TZ_AUDIO_H
typedef struct {
    /* jack_port_t **output_port; */
    /* jack_client_t **client; */
    /* const char **ports; */
    /* int nchan; */
    pw_patch *patch;
} tz_audio;

/* void tz_run_audio(tz_audio *audio, void *ud, int (*callback)(jack_nframes_t, void *)); */
/* void tz_stop_audio(tz_audio *audio); */
void tz_pw_init(tz_audio *audio);
void tz_pw_mkpatch(tz_audio *audio);
void tz_pw_del(tz_audio *audio);
#endif
