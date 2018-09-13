#include "alsalib_test.h"

int alsa_play_test(void)
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
    snd_pcm_t *play_in_handle;//PCM设备句柄pcm.h
    snd_pcm_t *play_out_handle;//PCM设备句柄pcm.h
    snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置
    FILE *fp = fopen(wav_file, "rb");
    if(fp == NULL)
    {
        printf("can't open file");
        return 0;
    }
    fseek(fp, 100, SEEK_SET);

    //1. 打开PCM，最后一个参数为0意味着标准配置
    ret = snd_pcm_open(&play_out_handle, PLAY_OUT_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        perror("snd_pcm_open");
        exit(1);
    }
    frames = 48;
    ret = snd_pcm_set_params(play_out_handle,
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
        else
        {
            printf(",");
            //9. 写音频数据到PCM设备
            while((ret = snd_pcm_writei(play_out_handle, buffer, frames))<0)
            {
                printf("ret2 = %d\n",ret);
                usleep(1000);
                if(ret == -EPIPE)
                {
                    /* EPIPE means underrun */
                    fprintf(stderr, "underrun occurred\n");
                    //完成硬件参数设置，使设备准备好
                    snd_pcm_prepare(play_out_handle);
                }
                else if (ret < 0)
                {
                    fprintf(stderr,
                        "error from writei: %s\n",
                        snd_strerror(ret));
                }
            }
            //printf("%d\t",ret);
            //usleep(1000);
        }
    }
    printf("done\n");
    //10. 关闭PCM设备句柄
    snd_pcm_close(play_out_handle);

    return 0;
}

int alsa_test_1(void)
{
	return 1;
}