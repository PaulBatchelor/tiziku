#include <stdlib.h>
#include <jack/jack.h>
#include <stdio.h>
#include <soundpipe.h>
#include <sporth.h>

#include "patchwerk.h"
#include "audio.h"
#include "wavout.h"
#include "pwsporth.h"
#include "glitch.h"
#include "add.h"

/* static void jack_shutdown (void *arg) */
/* { */
/*     exit (1); */
/* } */

void node_chooser(pw_node *node, plumber_data *pd);

void tz_run_audio(tz_audio *audio, void *ud, int (*callback)(jack_nframes_t, void *))
{
    /* const char *client_name = "tiziku"; */
    /* const char *server_name = NULL; */
    /* int chan; */
    /* int *nchan = &audio->nchan; */
    /* *nchan = MY_CHANNELS_OUT; */
    /* jack_options_t options = JackNullOption; */
    /* jack_status_t status; */

    /* audio->output_port = malloc(sizeof(jack_port_t *) * *nchan); */
    /* audio->client = malloc(sizeof(jack_client_t *)); */
    /* audio->client[0] = jack_client_open (client_name, options, &status, server_name); */

    /* if (audio->client[0] == NULL) { */
    /*     fprintf (stderr, "jack_client_open() failed, " */
    /*          "status = 0x%2.0x\n", status); */
    /*     if (status & JackServerFailed) { */
    /*         fprintf (stderr, "Unable to connect to JACK server\n"); */
    /*     } */
    /*     exit (1); */
    /* } */

    /* if (status & JackServerStarted) { */
    /*     fprintf (stderr, "JACK server started\n"); */
    /* } */

    /* if (status & JackNameNotUnique) { */
    /*     client_name = jack_get_client_name(audio->client[0]); */
    /*     fprintf (stderr, "unique name `%s' assigned\n", client_name); */
    /* } */

    /* jack_set_process_callback (audio->client[0], callback, ud); */
    /* jack_on_shutdown (audio->client[0], jack_shutdown, 0); */

    /* char chan_name[50]; */
    /* for(chan = 0; chan < *nchan; chan++) { */
    /*     sprintf(chan_name, "output_%d", chan); */
    /*     printf("registering %s\n", chan_name); */
    /*     audio->output_port[chan] = jack_port_register (audio->client[0], chan_name, */
    /*                       JACK_DEFAULT_AUDIO_TYPE, */
    /*                       JackPortIsOutput, chan); */

    /*     if (audio->output_port[chan] == NULL) { */
    /*         fprintf(stderr, "no more JACK ports available\n"); */
    /*         exit (1); */
    /*     } */

    /*     if (jack_activate (audio->client[0])) { */
    /*         fprintf (stderr, "cannot activate client"); */
    /*         exit (1); */
    /*     } */
    /* } */

    /* audio->ports = jack_get_ports (audio->client[0], NULL, NULL, */
    /*             JackPortIsPhysical|JackPortIsInput); */

    /* if (audio->ports == NULL) { */
    /*     fprintf(stderr, "no physical playback ports\n"); */
    /*     exit (1); */
    /* } */

    /* for(chan = 0; chan < *nchan; chan++) { */
    /*     if (jack_connect (audio->client[0], jack_port_name (audio->output_port[chan]), audio->ports[chan])) { */
    /*         fprintf (stderr, "cannot connect output ports\n"); */
    /*     } */
    /* } */
}

void tz_stop_audio(tz_audio* audio)
{
    /* free (audio->ports); */
    /* jack_client_close (audio->client[0]); */
    /* free(audio->output_port); */
    /* free(audio->client); */
}

void tz_pw_init(tz_audio *audio)
{
    sp_data *sp;
    audio->patch = malloc(pw_patch_size());
    pw_patch_init(audio->patch, 64);
    pw_patch_alloc(audio->patch, 8, 10);
    sp_create(&sp);

    sp->sr = pw_patch_srate_get(audio->patch);
    pw_patch_data_set(audio->patch, sp);
}

void tz_pw_del(tz_audio *audio)
{
    sp_data *sp;
    sp = pw_patch_data_get(audio->patch);
    pw_patch_destroy(audio->patch);
    pw_patch_free_nodes(audio->patch);
    free(audio->patch);
    sp_destroy(&sp);
}

plumber_data the_pd;

void tz_sporth_init(tz_audio *audio)
{
    plumber_data *pd;
    sp_data *sp;
    int rc;

    pd = &the_pd;
    sp = pw_patch_data_get(audio->patch);

    plumber_register(pd);
    plumber_init(pd);
    pd->sp = sp;
    plumber_open_file(pd, "drone.sp");
    rc = plumber_parse(pd);

    if (rc != PLUMBER_OK) {
        exit(1);
    }
    plumber_compute(pd, PLUMBER_INIT);
    plumber_close_file(pd);
}

void tz_sporth_del(tz_audio *audio)
{
    plumber_clean(&the_pd);
}

void tz_pw_mkpatch(tz_audio *audio)
{
    pw_patch *patch;
    sp_data *sp;
    pw_node *node;
    pw_stack *stack;
    wavout_d *wavout;
    pwsporth *sporth;
    add_d *adder[2];
    glitch_d *glitch[4];

    patch = audio->patch;
    stack = pw_patch_stack(patch);
    sp = pw_patch_data_get(patch);

    /* create chooser (no outputs/inputs) */
    pw_patch_new_node(patch, &node);
    node_chooser(node, &the_pd);

    /* glitch 0 */
    pw_patch_new_node(patch, &node);
    glitch[0] = node_glitch(node, 0);

    /* glitch 1 */
    pw_patch_new_node(patch, &node);
    glitch[1] = node_glitch(node, 1);


    /* glitch0 + glitch 1 -> add0 */
    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    pw_patch_new_node(patch, &node);
    node_add(node);
    pw_node_setup(node);
    adder[0] = pw_node_get_data(node);
    pw_cable_connect(glitch[0]->out, adder[0]->in1);
    pw_cable_connect(glitch[1]->out, adder[0]->in2);


    /* glitch2 */
    pw_patch_new_node(patch, &node);
    glitch[2] = node_glitch(node, 2);


    /* add0 + glitch2 -> add1*/
    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    pw_patch_new_node(patch, &node);
    node_add(node);
    pw_node_setup(node);
    adder[1] = pw_node_get_data(node);
    pw_cable_connect(adder[0]->out, adder[1]->in1);
    pw_cable_connect(glitch[2]->out, adder[1]->in2);


    /* glitch3 */
    pw_patch_new_node(patch, &node);
    glitch[3] = node_glitch(node, 3);

    /* add1 + glitch3 -> add0 */
    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    pw_patch_new_node(patch, &node);
    node_add(node);
    pw_node_setup(node);
    adder[0] = pw_node_get_data(node);
    pw_cable_connect(adder[1]->out, adder[0]->in1);
    pw_cable_connect(glitch[3]->out, adder[0]->in2);


    /* sporth patch */
    pw_patch_new_node(patch, &node);
    sporth = node_pwsporth(node, &the_pd);


    /* add0 + sporth -> add1 */
    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    pw_patch_new_node(patch, &node);
    node_add(node);
    pw_node_setup(node);
    adder[1] = pw_node_get_data(node);

    pw_cable_connect(adder[0]->out, adder[1]->in2);
    pw_cable_connect(sporth->out, adder[1]->in1);

    /* add1 -> wavout */
    pw_stack_pop(stack, NULL);
    pw_patch_new_node(patch, &node);
    node_wavout(sp, node, "tiziku.wav");
    wavout = pw_node_get_data(node);

    pw_cable_connect(adder[1]->out, wavout->in);
    pw_stack_pop(stack, NULL);
}
