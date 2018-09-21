#include "alsalib_test.h"
#include "alsa_record.h"
#include "alsa_play.h"




void sysUsecTime()
{
    struct timeval    tv;
    struct timezone tz;
    
    struct tm         *p;
    
    gettimeofday(&tv, &tz);
    printf("tv_sec:%ld\n",tv.tv_sec);
    printf("tv_usec:%ld\n",tv.tv_usec);
    printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
    printf("tz_dsttime:%d\n",tz.tz_dsttime);
    
    p = localtime(&tv.tv_sec);
    printf("time_now:%d%d%d%d%d%d.%ld\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
}

long long get_us_time(void)
{
    struct timeval  tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
	long long sum = 0;
	sum = (tv.tv_sec-1419998000)*1000000 + tv.tv_usec;
	return sum;
    //printf("tv_sec:%ld\n",tv.tv_sec);
    //printf("tv_usec:%ld\n",tv.tv_usec);
    //printf("time_now:%d%d%d%d%d%d.%ld\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
}

void cal_delta_us_time(long long start, long long end)
{
	
	//printf("%lld - %lld = %lld\n",end,start,end-start);
}


// int alsa_play_test(void)
// {
    // int i;
    // int ret;
    // int buf[128];
    // unsigned int val;
    // int dir=0;
    // char *buffer;
    // char wav_file[20] = "./test_1.wav";
    // int size;
    // snd_pcm_uframes_t frames;
    // snd_pcm_uframes_t periodsize;
    // snd_pcm_t *play_in_handle;//PCM设备句柄pcm.h
    // snd_pcm_t *play_out_handle;//PCM设备句柄pcm.h
    // snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置
    // FILE *fp = fopen(wav_file, "rb");
    // if(fp == NULL)
    // {
        // printf("can't open file");
        // return 0;
    // }
    // fseek(fp, 100, SEEK_SET);

    // //1. 打开PCM，最后一个参数为0意味着标准配置
    // ret = snd_pcm_open(&play_out_handle, PLAY_OUT_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    // if (ret < 0) {
        // perror("snd_pcm_open");
        // exit(1);
    // }
    // frames = 48;
    // ret = snd_pcm_set_params(play_out_handle,
                                // SND_PCM_FORMAT_S16_LE,
                                // SND_PCM_ACCESS_RW_INTERLEAVED,
                                // 2,
                                // 48000,
                                // 1,
                                // 500000);// 0.5sec 
    // if (ret < 0){   
        // perror("snd_pcm_hw_params");
        // exit(1);
    // }    

    // size = frames*4; // 2 bytes/sample, 2 channels
    // buffer = (char *) malloc(size);
    // fprintf(stderr,"size = %d, frames = %d\n", size, frames);

    // while (1)
    // {
        // ret = fread(buffer, 1, size, fp);
        // if(ret == 0)
        // {
              // fprintf(stderr, "end of file on input\n");
              // break;
        // }
        // else if (ret != size)
        // {
        // }
        // else
        // {
            // //printf(",");
            // //9. 写音频数据到PCM设备
            // while((ret = snd_pcm_writei(play_out_handle, buffer, frames))<0)
            // {
                // //printf("ret2 = %d\n",ret);
                // usleep(1000);
                // if(ret == -EPIPE)
                // {
                    // // EPIPE means underrun
                    // fprintf(stderr, "underrun occurred\n");
                    // //完成硬件参数设置，使设备准备好
                    // snd_pcm_prepare(play_out_handle);
                // }
                // else if (ret < 0)
                // {
                    // fprintf(stderr,
                        // "error from writei: %s\n",
                        // snd_strerror(ret));
                // }
            // }
            // //printf("%d\t",ret);
            // //usleep(1000);
        // }
    // }
    // printf("done\n");
// }


int alsa_test_1(void)
{
	
	//////////////////// play
	record_handle_t record_handle={0};
    int ret, ret2;
	long long start, end;
	long long start2, end2;

	play_handle_t play_handle;
	
    
	ret = alsa_play_init(&(play_handle.handle));
								
	if (ret <0)
		exit(1);
	
	
	//play_handle.size = play_handle.frames*4; // 2 bytes/sample, 2 channels
    //play_handle.buffer = (char *) malloc(play_handle.size);
	//////////////// play end

	
	//////////////// record 
    record_params_t params;
    params.duration = 10;
    params.format = SND_PCM_FORMAT_S16_LE;
    params.rate = 48000;
    params.channel = 2;
	
    if(init_recorder(&record_handle,&params, PLAY_IN_DEVICE_NAME)==RECORD_FAIL){
        printf("init_recorder fail.\n");
        return RECORD_FAIL;
    }
	
	//record_handle.size = 4096;
	play_handle.buffer = record_handle.buffer;
	play_handle.frames = record_handle.chunk_size*4;
	while (1) //(total_size>0) { //写入的数据超过total size就结束
	{
		start2 = get_us_time();
		start = get_us_time();
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, play_handle.frames);
		end = get_us_time();
		//printf("read----------");
		cal_delta_us_time(start, end);
		if (ret == -EAGAIN ) {
			snd_pcm_wait(record_handle.pcm, 1000);
		} else if (ret == -EPIPE) {
			snd_pcm_prepare(record_handle.pcm);
			printf("snd_pcm_readi return EPIPE.\n");
		} else if (ret == -ESTRPIPE) {
			printf("snd_pcm_readi return ESTRPIPE.\n");
		} else if (ret < 0) {
			printf("snd_pcm_readi return fail.\n");
			return RECORD_FAIL;
		}

		if(ret>0)
		{
			
			//while( ret > 0)
			{
				//printf("read = %d\n",ret);
				start = get_us_time();
				while((ret2 = snd_pcm_writei(play_handle.handle, play_handle.buffer, play_handle.frames))<0)
				{
					//printf("write = %d\n",ret2);
					usleep(1000);
					if(ret2 == -EPIPE)
					{
						// EPIPE means underrun 
						fprintf(stderr, "underrun occurred\n");
						//完成硬件参数设置，使设备准备好
						snd_pcm_prepare(play_handle.handle);
					}
					else if (ret2 < 0)
					{
						fprintf(stderr, "error from writei: %s\n", snd_strerror(ret2));
					}
				}
				end = get_us_time();
				//printf("write----------");
				cal_delta_us_time(start, end);
				//printf("ret2=%d\t",ret2);
				//ret -= ret2;
				//play_handle.buffer += ret2;
			}
			//total_size -= count;
		}
		end2 = get_us_time();
		//printf("total----------");
		cal_delta_us_time(start2, end2);
		
    }
	destroy_recorder(&record_handle);
	///////////////////// record
	
	return 1;
}


int alsa_test_2(void)
{
	record_handle_t record_handle={0};
	
	//////////////////// play
    int ret, ret2;
	long long start, end;
	long long start2, end2;

	play_handle_t play_handle;
	
    
	ret = alsa_play_init_2(&(play_handle.handle));
								
	if (ret <0)
		exit(1);
	
	play_handle.frames = 1024;
	//play_handle.size = play_handle.frames*4; // 2 bytes/sample, 2 channels
    //play_handle.buffer = (char *) malloc(play_handle.size);
	//////////////// play end

	
	//////////////// record 
    record_params_t params;
    params.duration = 10;
    params.format = SND_PCM_FORMAT_S16_LE;
    params.rate = 16000;
    params.channel = 8;
	
    if(init_recorder(&record_handle,&params, RECORD_IN_DEVICE_NAME)==RECORD_FAIL){
        printf("init_recorder fail.\n");
        return RECORD_FAIL;
    }
	
	//record_handle.size = 4096;
	play_handle.buffer = record_handle.buffer;
	
	while (1) //(total_size>0) { //写入的数据超过total size就结束
	{
		start2 = get_us_time();
		start = get_us_time();
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, record_handle.chunk_bytes*4);
		end = get_us_time();
		//printf("read----------");
		cal_delta_us_time(start, end);
		if (ret == -EAGAIN ) {
			snd_pcm_wait(record_handle.pcm, 1000);
		} else if (ret == -EPIPE) {
			snd_pcm_prepare(record_handle.pcm);
			printf("snd_pcm_readi return EPIPE.\n");
		} else if (ret == -ESTRPIPE) {
			printf("snd_pcm_readi return ESTRPIPE.\n");
		} else if (ret < 0) {
			printf("snd_pcm_readi return fail.\n");
			return RECORD_FAIL;
		}

		if(ret>0)
		{
			
			//while( ret > 0)
			{
				//printf("read = %d\n",ret);
				start = get_us_time();
				while((ret2 = snd_pcm_writei(play_handle.handle, play_handle.buffer, play_handle.frames))<0)
				{
					//printf("write = %d\n",ret2);
					usleep(1000);
					if(ret2 == -EPIPE)
					{
						// EPIPE means underrun 
						fprintf(stderr, "underrun occurred\n");
						//完成硬件参数设置，使设备准备好
						snd_pcm_prepare(play_handle.handle);
					}
					else if (ret2 < 0)
					{
						fprintf(stderr, "error from writei: %s\n", snd_strerror(ret2));
					}
				}
				end = get_us_time();
				//printf("write----------");
				cal_delta_us_time(start, end);
				//printf("ret2=%d\t",ret2);
				//ret -= ret2;
				//play_handle.buffer += ret2;
			}
			//total_size -= count;
		}
		end2 = get_us_time();
		//printf("total----------");
		cal_delta_us_time(start2, end2);
		
    }
	destroy_recorder(&record_handle);
	///////////////////// record
	
	return 1;
}