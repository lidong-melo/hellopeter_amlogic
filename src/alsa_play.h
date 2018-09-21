#ifndef _ALSA_PLAY_H
#define _ALSA_PLAY_H

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"



//#define PLAY_IN_DEVICE_NAME "hw:1,0"
#define RECORD_OUT_DEVICE_NAME "hw:1,0"
#define PLAY_OUT_DEVICE_NAME "hw:0,2"


//在c文件内部定义,可以认为是录制参数的集合
typedef struct{
   snd_pcm_t *handle;//PCM设备句柄pcm.h
   snd_pcm_uframes_t frames;
   int size;
   char * buffer; //用于保存录制数据的缓存buffer
   
   snd_pcm_format_t format; //pcm 数据的格式
   unsigned int channels;  //channel
   unsigned int rate;      //采样率
   size_t bits_per_sample;   //一个sample包含的bit 数
} play_handle_t;


int alsa_play(void);
int alsa_play_init(snd_pcm_t ** play_out_handle);
int alsa_play_init_2(snd_pcm_t ** play_out_handle);
int alsa_play_uninit(snd_pcm_t * play_out_handle);


#endif