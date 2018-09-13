#ifndef _ALSALIB_TEST_H
#define _ALSALIB_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"



#define PLAY_IN_DEVICE_NAME "hw:1,0"
#define PLAY_OUT_DEVICE_NAME "hw:0,2"

#define RECORD_IN_DEVICE_NAME "hw:0,3"
#define RECORD_OUT_DEVICE_NAME "hw:1,0"



int alsa_play_test(void);
//int alsa_record(void);


#endif