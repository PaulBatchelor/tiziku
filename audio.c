#include <stdlib.h>
#include <jack/jack.h>
#include <stdio.h>
#include "audio.h"

static void jack_shutdown (void *arg)
{
    exit (1);
}

void tz_run_audio(tz_audio *audio, void *ud, int (*callback)(jack_nframes_t, void *))
{
    const char *client_name = "tiziku";
    const char *server_name = NULL;
    int chan;
    int *nchan = &audio->nchan;
    *nchan = MY_CHANNELS_OUT;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    
    audio->output_port = malloc(sizeof(jack_port_t *) * *nchan);
    audio->client = malloc(sizeof(jack_client_t *));
    audio->client[0] = jack_client_open (client_name, options, &status, server_name);

    if (audio->client[0] == NULL) {
        fprintf (stderr, "jack_client_open() failed, "
             "status = 0x%2.0x\n", status);
        if (status & JackServerFailed) {
            fprintf (stderr, "Unable to connect to JACK server\n");
        }
        exit (1);
    }

    if (status & JackServerStarted) {
        fprintf (stderr, "JACK server started\n");
    }

    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(audio->client[0]);
        fprintf (stderr, "unique name `%s' assigned\n", client_name);
    }

    jack_set_process_callback (audio->client[0], callback, ud);
    jack_on_shutdown (audio->client[0], jack_shutdown, 0);

    char chan_name[50];
    for(chan = 0; chan < *nchan; chan++) {
        sprintf(chan_name, "output_%d", chan);
        printf("registering %s\n", chan_name);
        audio->output_port[chan] = jack_port_register (audio->client[0], chan_name,
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput, chan);

        if (audio->output_port[chan] == NULL) {
            fprintf(stderr, "no more JACK ports available\n");
            exit (1);
        }

        if (jack_activate (audio->client[0])) {
            fprintf (stderr, "cannot activate client");
            exit (1);
        }
    }

    audio->ports = jack_get_ports (audio->client[0], NULL, NULL,
                JackPortIsPhysical|JackPortIsInput);

    if (audio->ports == NULL) {
        fprintf(stderr, "no physical playback ports\n");
        exit (1);
    }

    for(chan = 0; chan < *nchan; chan++) {
        if (jack_connect (audio->client[0], jack_port_name (audio->output_port[chan]), audio->ports[chan])) {
            fprintf (stderr, "cannot connect output ports\n");
        }
    }
}

void tz_stop_audio(tz_audio* audio)
{
    free (audio->ports);
    jack_client_close (audio->client[0]);
    free(audio->output_port);
    free(audio->client);
}
