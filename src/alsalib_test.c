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


int alsa_test(int dir)
{
    int ret;

	play_handle_t play_handle;
	record_handle_t record_handle;
	
	if(dir == AUDIO_PLAY)
	{
		play_handle.channels = PLAY_CHANNELS;
		play_handle.rate = PLAY_RATE;
		play_handle.device_name = (char*)PLAY_OUT_DEVICE_NAME;
		record_handle.device_name = (char*)PLAY_IN_DEVICE_NAME;
	}
	else if(dir == AUDIO_RECORD)
	{
		play_handle.channels = RECORD_CHANNELS;
		play_handle.rate = RECORD_RATE;
		play_handle.device_name = (char*)RECORD_OUT_DEVICE_NAME;
		record_handle.device_name = (char*)RECORD_IN_DEVICE_NAME;
	
	}

	play_handle.format = SND_PCM_FORMAT_S16_LE;	
	
	record_handle.channels = play_handle.channels;
	record_handle.rate =  play_handle.rate;
	record_handle.format =  play_handle.format;
	record_handle.buffer_time = BUFFER_TIME;
	record_handle.periods = PERIODS;
	
	
								
	if (alsa_play_init(&play_handle) <0)
	{
		printf("play init fail.\n");
		return PLAY_FAIL;
	}
	
    if(init_recorder(&record_handle)==PLAY_FAIL){
        printf("init_recorder fail.\n");
        return PLAY_FAIL;
    }
	
	play_handle.buffer = record_handle.buffer;
	play_handle.frames = record_handle.chunk_size*record_handle.periods;// periods =4
	
	while (1)
	{
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, play_handle.frames);
		if (ret == -EAGAIN ) {
			snd_pcm_wait(record_handle.pcm, 1000);
		} else if (ret == -EPIPE) {
			snd_pcm_prepare(record_handle.pcm);
			printf("snd_pcm_readi return EPIPE.\n");
		} else if (ret == -ESTRPIPE) {
			printf("snd_pcm_readi return ESTRPIPE.\n");
		} else if (ret < 0) {
			printf("snd_pcm_readi return fail.\n");
			return PLAY_FAIL;
		}

		if(ret>0)
		{
			while((ret = snd_pcm_writei(play_handle.pcm, play_handle.buffer, play_handle.frames))<0)
			{
				usleep(1000);
				if(ret == -EPIPE)
				{
					// EPIPE means underrun 
					fprintf(stderr, "underrun occurred\n");
					//完成硬件参数设置，使设备准备好
					snd_pcm_prepare(play_handle.pcm);
				}
				else if (ret < 0)
				{
					fprintf(stderr, "error from writei: %s\n", snd_strerror(ret));
				}
			}
		}
		
    }
	destroy_recorder(&record_handle);
	
	return RECORD_SUCCESS;
}


// int arecord_test(void)
// {
	// record_handle_t record_handle={0};
	
	// //////////////////// play
    // int ret, ret2;
	// long long start, end;
	// long long start2, end2;

	// play_param_t play_handle;
	
    
	// ret = alsa_play_init(&(play_handle.handle, ));
								
	// if (ret <0)
		// exit(1);
	
	// //play_handle.frames = 1024;
	// //play_handle.size = play_handle.frames*4; // 2 bytes/sample, 2 channels
    // //play_handle.buffer = (char *) malloc(play_handle.size);
	// //////////////// play end

	
	// //////////////// record 
    // record_params_t params;
    // params.duration = 10;
    // params.format = SND_PCM_FORMAT_S16_LE;
    // params.rate = 16000;
    // params.channel = 8;
	
    // if(init_recorder(&record_handle,&params, RECORD_IN_DEVICE_NAME)==RECORD_FAIL){
        // printf("init_recorder fail.\n");
        // return RECORD_FAIL;
    // }
	
	// //record_handle.size = 4096;
	// play_handle.buffer = record_handle.buffer;
	// play_handle.frames = record_handle.chunk_size*4;
	
	// while (1) //(total_size>0) { //写入的数据超过total size就结束
	// {
		// start2 = get_us_time();
		// start = get_us_time();
		// ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, play_handle.frames);
		// end = get_us_time();
		// //printf("read----------");
		// cal_delta_us_time(start, end);
		// if (ret == -EAGAIN ) {
			// snd_pcm_wait(record_handle.pcm, 1000);
		// } else if (ret == -EPIPE) {
			// snd_pcm_prepare(record_handle.pcm);
			// printf("snd_pcm_readi return EPIPE.\n");
		// } else if (ret == -ESTRPIPE) {
			// printf("snd_pcm_readi return ESTRPIPE.\n");
		// } else if (ret < 0) {
			// printf("snd_pcm_readi return fail.\n");
			// return RECORD_FAIL;
		// }

		// if(ret>0)
		// {
			
			// //while( ret > 0)
			// {
				// //printf("read = %d\n",ret);
				// start = get_us_time();
				// while((ret2 = snd_pcm_writei(play_handle.handle, play_handle.buffer, play_handle.frames))<0)
				// {
					// //printf("write = %d\n",ret2);
					// usleep(1000);
					// if(ret2 == -EPIPE)
					// {
						// // EPIPE means underrun 
						// fprintf(stderr, "underrun occurred\n");
						// //完成硬件参数设置，使设备准备好
						// snd_pcm_prepare(play_handle.handle);
					// }
					// else if (ret2 < 0)
					// {
						// fprintf(stderr, "error from writei: %s\n", snd_strerror(ret2));
					// }
				// }
				// end = get_us_time();
				// //printf("write----------");
				// cal_delta_us_time(start, end);
				// //printf("ret2=%d\t",ret2);
				// //ret -= ret2;
				// //play_handle.buffer += ret2;
			// }
			// //total_size -= count;
		// }
		// end2 = get_us_time();
		// //printf("total----------");
		// cal_delta_us_time(start2, end2);
		
    // }
	// destroy_recorder(&record_handle);
	// ///////////////////// record
	
	// return 1;
// }