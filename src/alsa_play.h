#ifndef _ALSA_PLAY_H
#define _ALSA_PLAY_H

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"



#define PLAY_IN_DEVICE_NAME "hw:1,0"
#define PLAY_OUT_DEVICE_NAME "hw:0,2"

int alsa_play(void);
int alsa_play_init(snd_pcm_t * play_out_handle);
int alsa_play_uninit(snd_pcm_t * play_out_handle);


#endif