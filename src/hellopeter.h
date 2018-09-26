#ifndef _HELLOPETER_H
#define _HELLOPETER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "serial_port.h"
#include "gpio.h"
#include "fifo.h"



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


#endif