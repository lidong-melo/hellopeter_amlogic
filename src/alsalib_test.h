#ifndef _ALSALIB_TEST_H
#define _ALSALIB_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"

#include <time.h>
#include <sys/time.h>

#define AUDIO_PLAY 1
#define AUDIO_RECORD 2


#define PLAY_IN_DEVICE_NAME "hw:1,0"
#define RECORD_IN_DEVICE_NAME "hw:0,3"


#define PLAY_OUT_DEVICE_NAME "hw:0,2"
#define RECORD_OUT_DEVICE_NAME "hw:1,0"

#define PLAY_RATE 48000
#define PLAY_CHANNELS 2


#define RECORD_RATE 16000
#define RECORD_CHANNELS 8

#define BUFFER_TIME	64000
#define PERIODS 4

int alsa_test(int dir);

#endif