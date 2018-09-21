#include "alsa_play.h"





int alsa_play_init(snd_pcm_t ** handle)
{
    int ret;	
    //1. 打开PCM，最后一个参数为0意味着标准配置
    ret = snd_pcm_open(handle, PLAY_OUT_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0)
        perror("snd_pcm_open");
    
    ret = snd_pcm_set_params( * handle,
                                SND_PCM_FORMAT_S16_LE,
                                SND_PCM_ACCESS_RW_INTERLEAVED,
                                2,
                                48000,
                                1, //soft_resample
                                500000);// 0.5sec 
    if (ret < 0)
        perror("snd_pcm_hw_params");
	
	return ret;
}

int alsa_play_init_2(snd_pcm_t ** handle)
{
    int ret;	
    //1. 打开PCM，最后一个参数为0意味着标准配置
    ret = snd_pcm_open(handle, RECORD_OUT_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0)
        perror("snd_pcm_open");
    
    ret = snd_pcm_set_params( * handle,
                                SND_PCM_FORMAT_S16_LE,
                                SND_PCM_ACCESS_RW_INTERLEAVED,
                                8,
                                16000,
                                1, //soft_resample
                                500000);// 0.5sec 
    if (ret < 0)
        perror("snd_pcm_hw_params");
	
	return ret;
}


int alsa_play_uninit(snd_pcm_t * handle)
{
	//10. 关闭PCM设备句柄
    snd_pcm_close(handle);
}

int alsa_play(void)
{
    int ret;
    char wav_file[20] = "./test_1.wav";

	play_handle_t play_handle;
	
    FILE *fp = fopen(wav_file, "rb");
    if(fp == NULL)
    {
        printf("can't open file");
        return 0;
    }
    fseek(fp, 100, SEEK_SET);
	
	ret = alsa_play_init(&(play_handle.handle));
								
	if (ret <0)
		exit(1);
	
	play_handle.frames = 48;
	play_handle.size = play_handle.frames*4; // 2 bytes/sample, 2 channels
    //play_handle.buffer = (char *) malloc(play_handle.size);
	
    while (1)
    {
        ret = fread(play_handle.buffer, 1, play_handle.size, fp);
        if(ret == 0)
        {
			fprintf(stderr, "end of file on input\n");
            break;
        }
        else if (ret != play_handle.size)
        {
        }
        else
        {
            //9. 写音频数据到PCM设备
            while((ret = snd_pcm_writei(play_handle.handle, play_handle.buffer, play_handle.frames))<0)
            {
                printf("ret2 = %d\n",ret);
                usleep(1000);
                if(ret == -EPIPE)
                {
                    /* EPIPE means underrun */
                    fprintf(stderr, "underrun occurred\n");
                    //完成硬件参数设置，使设备准备好
                    snd_pcm_prepare(play_handle.handle);
                }
                else if (ret < 0)
                {
                    fprintf(stderr, "error from writei: %s\n", snd_strerror(ret));
                }
            }
        }
    }
    printf("done\n");
	
	alsa_play_uninit(play_handle.handle);

    return 0;
}