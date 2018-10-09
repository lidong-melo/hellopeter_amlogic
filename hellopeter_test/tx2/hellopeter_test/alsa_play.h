#ifndef _ALSA_PLAY_H
#define _ALSA_PLAY_H

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"
#include "log.h"

#define PLAY_FAIL   -2
#define PLAY_SUCCESS 0


typedef struct{
   snd_pcm_t *pcm;//PCM设备句柄pcm.h
   snd_pcm_uframes_t frames;
   int frame_size;
   char * buffer; //用于保存录制数据的缓存buffer
   
   char * device_name;
   snd_pcm_format_t format; //pcm 数据的格式
   int channels;  //channel
   int rate;      //采样率
   size_t bits_per_sample;   //一个sample包含的bit 数
} play_handle_t;



int alsa_play_init(play_handle_t * play_handle);
int alsa_play_uninit(snd_pcm_t * play_out_handle);


#endif