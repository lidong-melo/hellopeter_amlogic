#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "alsa/asoundlib.h"  //alsa-lib相关的头文件
#include "alsa_record.h"

 
int init_recorder(record_handle_t* handle)
{
    snd_pcm_hw_params_t *hwparams;
	int ret = 0;
	
    if(handle==NULL){
        return RECORD_FAIL;
    }
    //1、打开pcm录音设备
    if(snd_pcm_open(&(handle->pcm), (const char*)(handle->device_name), SND_PCM_STREAM_CAPTURE, 0) < 0) {
        printf("snd_pcm_open fail.\n");
        return RECORD_FAIL;
    }
    //2、录音相关参数设置
    
    //构建参数结构体
    snd_pcm_hw_params_alloca(&hwparams);
 
    //3、获取参数
    if(snd_pcm_hw_params_any(handle->pcm, hwparams) < 0) {
        printf("snd_pcm_hw_params_any fail.\n");
        goto init_recorder_fail;
    }
 
    //4、设置相关参数
    if(snd_pcm_hw_params_set_access(handle->pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        printf("snd_pcm_hw_params_set_access fail.\n");
        goto init_recorder_fail;
    }
 
    //设置pcm数据格式
    if(snd_pcm_hw_params_set_format(handle->pcm, hwparams, handle->format) < 0) {
        printf("snd_pcm_hw_params_set_format fail.\n");
        goto init_recorder_fail;
    }
 
 
    //设置channel
    if(snd_pcm_hw_params_set_channels(handle->pcm, hwparams, handle->channels) < 0) {
        printf("snd_pcm_hw_params_set_channels fail.\n");
        goto init_recorder_fail;
    }
    
    //设置采样率
    if(snd_pcm_hw_params_set_rate(handle->pcm, hwparams, handle->rate, 0) < 0) {
        printf("snd_pcm_hw_params_set_channels fail.\n");
        goto init_recorder_fail;
    }
 
    if (snd_pcm_hw_params_set_buffer_time(handle->pcm, hwparams, handle->buffer_time, 0) < 0) {
        printf("snd_pcm_hw_params_set_buffer_time_near fail.\n");
        goto init_recorder_fail;
    }
 
    if (snd_pcm_hw_params_set_period_time(handle->pcm, hwparams, handle->buffer_time/handle->periods, 0) < 0) {
        printf("snd_pcm_hw_params_set_period_time_near fail.\n");
        goto init_recorder_fail;
    }
 
	
    //5、设置参数给alsa
    if (snd_pcm_hw_params(handle->pcm, hwparams) < 0) {
        printf("snd_pcm_hw_params fail.\n");
        goto init_recorder_fail;
    }
 
    //获取相关参数
    snd_pcm_hw_params_get_period_size(hwparams, &(handle->chunk_size), 0);
	
    //根据设置计算出audio 的chunk 大小
    handle->bits_per_sample = snd_pcm_format_physical_width(handle->format);
	handle->frame_size = (handle->bits_per_sample * handle->channels)/8;
	
    handle->chunk_bytes = handle->chunk_size*handle->frame_size;
	
    
	
	snd_pcm_hw_params_get_periods(hwparams, &handle->periods, 0);
	
	//ret = snd_pcm_hw_params_set_buffer_size(handle->pcm, hwparams, frames);
	snd_pcm_hw_params_get_buffer_size(hwparams, &handle->frames);
	snd_pcm_hw_params_get_buffer_time(hwparams, &handle->buffer_time,0);
	printf("chunk_size= %d,chunk_bytes= %d,buffer_time= %d,buffer_size= %d,frame_size=%d, periods=%d\n",handle->chunk_size,handle->chunk_bytes,handle->buffer_time, handle->frames, handle->frame_size, handle->periods);
	
	//申请所需的内存
    handle->buffer = (char*)malloc(handle->frames * handle->frame_size);
	
    if(!handle->buffer){
        printf("malloc buffer fail.\n");
        goto init_recorder_fail;
    }
 
    return RECORD_SUCCESS;
 
init_recorder_fail:
    snd_pcm_close(handle->pcm);  //参数设置失败，就关闭
    return RECORD_FAIL;
}

 
int uninit_recorder(record_handle_t* handle){
    if(handle==NULL){
        printf("destroy_recorder handle is null.\n");
        return RECORD_FAIL;
    }
    free(handle->buffer);  //释放内存
    snd_pcm_drain(handle->pcm);
    snd_pcm_close(handle->pcm);
    return RECORD_SUCCESS;
}

