#ifndef _ALSA_RECORD_H
#define _ALSA_RECORD_H
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "alsa/asoundlib.h"  //alsa-lib相关的头文件

#include "log.h"
 
//宏定义
#define RECORD_FAIL   -1
#define RECORD_SUCCESS 0


//在c文件内部定义,可以认为是录制参数的集合
typedef struct{
   snd_pcm_t *pcm;  //alsa pcm handle
   snd_pcm_format_t format; //pcm 数据的格式
   int channels;  //channel
   int rate;      //采样率
   size_t bits_per_sample;   //一个sample包含的bit 数
   
   size_t chunk_bytes;    //简单理解为1秒产生的pcm数据大小
   snd_pcm_uframes_t chunk_size; //简单理解为一个channel 1秒产生的frame个数
   char* buffer; //用于保存录制数据的缓存buffer
   char* device_name;
   unsigned int buffer_time;
   snd_pcm_uframes_t frames;//buffer_size;
   unsigned int periods;
   int frame_size;
} record_handle_t;

int init_recorder(record_handle_t* handle);
int uninit_recorder(record_handle_t* handle);
 
#endif
