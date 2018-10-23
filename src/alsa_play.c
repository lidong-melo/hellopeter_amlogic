#include "alsa_play.h"





int alsa_play_init(play_handle_t * play_handle)
{
    int ret;	
    //1. 打开PCM，最后一个参数为0意味着标准配置
    ret = snd_pcm_open(&play_handle->pcm, (const char*)(play_handle->device_name), SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0)
        log_out("snd_pcm_open");
    
    ret = snd_pcm_set_params( play_handle->pcm,
                                play_handle->format,
                                SND_PCM_ACCESS_RW_INTERLEAVED,
                                play_handle->channels,
                                play_handle->rate,
                                1, //soft_resample
								2000);
                                //500000);// 0.5sec 
    if (ret < 0)
        log_out("snd_pcm_hw_params");
	
	return ret;
}


int alsa_play_uninit(snd_pcm_t * handle)
{
	//10. 关闭PCM设备句柄
    snd_pcm_close(handle);
}
