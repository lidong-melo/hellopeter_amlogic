#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "alsa/asoundlib.h"  //alsa-lib相关的头文件
#include "alsa_record.h"
 
//wav 的文件头定义，一些固定的参数先赋值
static wav_format_t wav_format={
    {'R','I','F','F'}, //RIFF
    {0,0,0,0}, //之所以定义成char，是因为保存int 的方式不一样，比如1，可存为00 00 00 01 ,或者01 00 00 00
    {'W','A','V','E'},  //WAVE
    {'f','m','t',' '},  //fmt 后面有一个空格
    {16,0,0,0}, //一般为00000010H，若为00000012H则说明数据头携带附加信息,这里设成10h，如果设成12h，需要加上fmt_chunk_end
    {1,0}, //PCM = 1
    {2,0}, //Mono = 1, Stereo = 2
    {0,0,0,0},  //samplerate
    {0,0,0,0},  //byterate
    {4,0},  //NumChannels * BitsPerSample/8
    {16,0},  //8bits, 16bits, etc.
    {'d','a','t','a'}, //data
    {0,0,0,0}  //data size
};
 

 
//pcm format 定义在alsa-lib/include/pcm.h
static snd_pcm_format_t get_format_from_params(record_params_t* params){
    if(params!=NULL){  //传入的params不为空，
        switch (params->format) {  //这里只是简单的转换，后面可以根据需求制定转换规则
        case 0:
            return SND_PCM_FORMAT_S8;
        case 1:
            return SND_PCM_FORMAT_U8;
        case 2:
            return SND_PCM_FORMAT_S16_LE;
        case 3:
            return SND_PCM_FORMAT_S16_BE;
        default:  return SND_PCM_FORMAT_S16_LE;//默认返回
        }
    }
    return SND_PCM_FORMAT_S16_LE; //默认返回
}
 
//简单的参数转换规则，可以根据需要自行修改
static unsigned int get_channel_from_params(record_params_t* params){
    return params->channel; //(params->channel==2)?2:1;
}
 
//这里只是简单的转换，后面可以根据需求制定转换规则
static unsigned int get_rate_from_params(record_params_t* params){
	//printf("sample_rate:%d,%d\n",params->rate, RECORD_SAMPLE_RATE);
    return params->rate;//(unsigned int)(params->rate<RECORD_SAMPLE_RATE)?RECORD_SAMPLE_RATE:params->rate; //不能小于8000
}
 
int init_recorder(record_handle_t* handle,record_params_t* params, const char * device_name){
    snd_pcm_hw_params_t *hwparams;
    unsigned int sample_rate = handle->rate;
    unsigned int buffer_time;
    unsigned int period_time ;
    snd_pcm_uframes_t buffer_size;
	int ret = 0;
	unsigned int periods = 0;
	int frame_size = 0;
	
	printf("handle_rate=%d\n",handle->rate);


    if(handle==NULL){
        return RECORD_FAIL;
    }
    //1、打开pcm录音设备
    if(snd_pcm_open(&(handle->pcm), device_name, SND_PCM_STREAM_CAPTURE, 0) < 0) {
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
 
    handle->format = get_format_from_params(params);
 
    //设置pcm数据格式
    if(snd_pcm_hw_params_set_format(handle->pcm, hwparams, handle->format) < 0) {
        printf("snd_pcm_hw_params_set_format fail.\n");
        goto init_recorder_fail;
    }
 
    handle->channels=get_channel_from_params(params);
 
    //设置channel
    if(snd_pcm_hw_params_set_channels(handle->pcm, hwparams, handle->channels) < 0) {
        printf("snd_pcm_hw_params_set_channels fail.\n");
        goto init_recorder_fail;
    }
 
    handle->rate = get_rate_from_params(params);
	sample_rate = handle->rate;
	printf("handle->rate=%d\n",handle->rate);
    
    //设置采样率
    if(snd_pcm_hw_params_set_rate_near(handle->pcm, hwparams,&sample_rate, 0) < 0) {
        printf("snd_pcm_hw_params_set_channels fail.\n");
        goto init_recorder_fail;
    }	
	
    if(handle->rate != sample_rate){
        printf("The rate %d Hz is not supported, Using %d Hz instead.\n",handle->rate,sample_rate);
    }
 
    //

    if (snd_pcm_hw_params_get_buffer_time_max(hwparams, &buffer_time, 0) < 0) {
        printf("snd_pcm_hw_params_get_buffer_time_max fail.\n");
        goto init_recorder_fail;
    }
	printf("max buffer_time= %d\n",buffer_time);
 
    if (buffer_time > RECORD_BUFFER_TIME_MAX){
        buffer_time = RECORD_BUFFER_TIME_MAX;
    }
	buffer_time = 64000;//record = 64000; // play = 80000;
    period_time = buffer_time / 4;
 
 
    if (snd_pcm_hw_params_set_buffer_time_near(handle->pcm, hwparams, &buffer_time, 0) < 0) {
        printf("snd_pcm_hw_params_set_buffer_time_near fail.\n");
        goto init_recorder_fail;
    }
	printf("buffer_time= %d\t",buffer_time);
 
    if (snd_pcm_hw_params_set_period_time_near(handle->pcm, hwparams, &period_time, 0) < 0) {
        printf("snd_pcm_hw_params_set_period_time_near fail.\n");
        goto init_recorder_fail;
    }
 
	printf("period_time= %d\n",period_time);
	
    //5、设置参数给alsa
    if (snd_pcm_hw_params(handle->pcm, hwparams) < 0) {
        printf("snd_pcm_hw_params fail.\n");
        goto init_recorder_fail;
    }
 
    //获取相关参数
    
    snd_pcm_hw_params_get_period_size(hwparams, &(handle->chunk_size), 0);
    snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
    if (handle->chunk_size == buffer_size) {
        printf("Can't use period equal to buffer size (%lu == %lu).\n",handle->chunk_size,buffer_size);
        goto init_recorder_fail;
    }
 
	//计算frame_size大小
	
 
 
    //根据设置计算出audio 的chunk 大小
    handle->bits_per_sample = snd_pcm_format_physical_width(handle->format);
	frame_size = (handle->bits_per_sample * handle->channels)/8;
	
    handle->chunk_bytes = handle->chunk_size*frame_size;
	
	
 
    //申请所需的内存
    handle->buffer = (char*)malloc(buffer_size * frame_size);
	
	printf("chunk_size= %d,chunk_bytes= %d,buffer_size= %d,buffer_bytes= %d\n",handle->chunk_size,handle->chunk_bytes,buffer_size,buffer_size * frame_size);
	//buffer_size = 3840*4;
	
	
	
	//snd_pcm_hw_params_get_periods_max(hwparams, &periods, 0);
	
	//ret = snd_pcm_hw_params_set_periods(handle->pcm, hwparams, 4, 0);
	snd_pcm_hw_params_get_periods(hwparams, &periods, 0);
	
	//ret = snd_pcm_hw_params_set_buffer_size(handle->pcm, hwparams, buffer_size);
	snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
	snd_pcm_hw_params_get_buffer_time(hwparams, &buffer_time,0);
	printf("chunk_size= %d,chunk_bytes= %d,buffer_time= %d,buffer_size= %d,frame_size=%d, periods=%d\n",handle->chunk_size,handle->chunk_bytes,buffer_time, buffer_size, frame_size, periods);
	
    if(!handle->buffer){
        printf("malloc buffer fail.\n");
        goto init_recorder_fail;
    }
 
    return RECORD_SUCCESS;
 
init_recorder_fail:
    snd_pcm_close(handle->pcm);  //参数设置失败，就关闭
    return RECORD_FAIL;
}

 
int destroy_recorder(record_handle_t* handle){
    if(handle==NULL){
        printf("destroy_recorder handle is null.\n");
        return RECORD_FAIL;
    }
    free(handle->buffer);  //释放内存
    snd_pcm_drain(handle->pcm);
    snd_pcm_close(handle->pcm);
    return RECORD_SUCCESS;
}
 
static int create_path(const char *path)
{
    char *start;
    char *buffer ;
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
 
    if (path[0] == '/')
        start = (char*)strchr(path + 1, '/');
    else
        start = (char*)strchr(path, '/');
 
    while (start) {
         buffer = (char*)strdup(path);
         buffer[start-path] = 0x00;
 
         if (mkdir(buffer, mode) == -1 && errno != EEXIST) {
             printf("creating directory %s fail.\n", buffer);
             free(buffer);
             return -1;
          }
          free(buffer);
          start = strchr(start + 1, '/');
    }
    return 0;
}
 
static int safe_open(const char *name)
{
    int fd;
 
    fd = open(name, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        if (errno != ENOENT)
            return -1;
        if (create_path(name) == 0)
            fd = open(name, O_WRONLY | O_CREAT, 0644);
    }
    return fd;
}
 
static int number_to_string(char* dest,size_t src,int len){
    int index=0;
    if(dest==NULL)
        return RECORD_FAIL;
    
    while(index<len){
       dest[index]=(char)(src&0xff); //取低8 bit
       src=src>>8; //右移
       index++;  //下标加1
    }
    return RECORD_SUCCESS;
}
 
 
static int do_record(record_handle_t* handle,int fd,int duration){
 
    //计算音频数据长度（秒数 * 采样率 * 帧长度）
    long total_size =duration*handle->rate*handle->bits_per_sample*handle->channels/8;
   
 
    snd_pcm_t* pcm=handle->pcm;
    char* buffer=handle->buffer;
    size_t size = handle->chunk_size;
    size_t frame_byte = handle->bits_per_sample*handle->channels/8;
    size_t count = 0;
    int ret = 0;
    size_t tmp;

     printf("do_record total_size is %d.\n",total_size);
 
    //封装好wav的文件头
    count = sizeof(wav_format_t);
    tmp=count+total_size -8; //计算chunk size大小
    number_to_string(wav_format.chunk_size,tmp,4);
    number_to_string(wav_format.audio_channels,handle->channels,2);
    number_to_string(wav_format.samplerate,handle->rate,4);
    tmp=handle->rate*frame_byte;
    number_to_string(wav_format.byterate,tmp,4);
    number_to_string(wav_format.block_align,frame_byte,2);
    number_to_string(wav_format.bits_persample,handle->bits_per_sample,2);
    number_to_string(wav_format.sub_chunk2_size,total_size,4);
    //写入wav 文件头
    if(write(fd,&wav_format,count)!=count){
        printf("write count(%d) fail.\n",count);
        return RECORD_FAIL;
    }
 
    while (total_size>0) { //写入的数据超过total size就结束
       ret = snd_pcm_readi(pcm, buffer, size);
       if (ret == -EAGAIN ) {
            snd_pcm_wait(pcm, 1000);
       } else if (ret == -EPIPE) {
            snd_pcm_prepare(pcm);
            printf("snd_pcm_readi return EPIPE.\n");
       } else if (ret == -ESTRPIPE) {
            printf("snd_pcm_readi return ESTRPIPE.\n");
       } else if (ret < 0) {
            printf("snd_pcm_readi return fail.\n");
            return RECORD_FAIL;
       }
 
       if(ret>0){
		   //printf("buffer_size=%d\t", ret);
           count = ret*frame_byte;
           if(write(fd,buffer,count)!=count){
               printf("write count(%d) fail.\n",count);
               return RECORD_FAIL;
           }
           total_size -= count;
       }
    }
 
    return RECORD_SUCCESS;
}
 
 
int do_alsa_record(char* path, record_params_t* params)
{
    int fd;
    record_handle_t record_handle={0};
    if(init_recorder(&record_handle,params, "hw:1,0")==RECORD_FAIL){
        printf("init_recorder fail.\n");
        return RECORD_FAIL;
    }
 
    fd = safe_open(path);  
    if(fd>0){
       do_record(&record_handle,fd,params->duration);
       close(fd);  //at last,we must close fd
    }else{
       printf("file(%s) open fail.\n",path);
    }
 
    destroy_recorder(&record_handle);
    return RECORD_SUCCESS;
}
