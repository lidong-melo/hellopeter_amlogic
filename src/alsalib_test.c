/**
*test.c
*
*注意：这个例子在Ubuntu 12.04.1环境下编译运行成功。
*
*/
#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"

#define REC_DEVICE_NAME "hw:0,2"


int alsa_play(void)
{
    int i;
    int ret;
    int buf[128];
    unsigned int val;
    int dir=0;
    char *buffer;
    char wav_file[20] = "./test_1.wav";
    int size;
    snd_pcm_uframes_t frames;
    snd_pcm_uframes_t periodsize;
    snd_pcm_t *playback_handle;//PCM设备句柄pcm.h
    snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置
    FILE *fp = fopen(wav_file, "rb");
    if(fp == NULL)
    {
        printf("can't open file");
        return 0;
    }
    fseek(fp, 100, SEEK_SET);

    //1. 打开PCM，最后一个参数为0意味着标准配置
    ret = snd_pcm_open(&playback_handle, REC_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        perror("snd_pcm_open");
        exit(1);
    }
    frames = 48;
    ret = snd_pcm_set_params(playback_handle,
                                SND_PCM_FORMAT_S16_LE,
                                SND_PCM_ACCESS_RW_INTERLEAVED,
                                2,
                                48000,
                                1,
                                500000);// 0.5sec 
    if (ret < 0){   
        perror("snd_pcm_hw_params");
        exit(1);
    }    

   

    size = frames*4; // 2 bytes/sample, 2 channels
    buffer = (char *) malloc(size);
    fprintf(stderr,"size = %d, frames = %d\n", size, frames);

    while (1)
    {
        ret = fread(buffer, 1, size, fp);
        if(ret == 0)
        {
              fprintf(stderr, "end of file on input\n");
              break;
        }
        else if (ret != size)
        {
        }
        //9. 写音频数据到PCM设备
        while((ret = snd_pcm_writei(playback_handle, buffer, frames))<0)
        {
            //printf("%d\t",ret);
            usleep(1000);
            if(ret == -EPIPE)
            {
                  /* EPIPE means underrun */
                  fprintf(stderr, "underrun occurred\n");
                  //完成硬件参数设置，使设备准备好
                  snd_pcm_prepare(playback_handle);
            }
            else if (ret < 0)
            {
                  fprintf(stderr,
                      "error from writei: %s\n",
                      snd_strerror(ret));
            }
        }
        //printf("%d\t",ret);
        //usleep(100);

    }
    printf("done\n");
    //10. 关闭PCM设备句柄
    snd_pcm_close(playback_handle);

    return 0;
}