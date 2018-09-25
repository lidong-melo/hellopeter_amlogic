#include "alsalib_test.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "serial_test.h"
#include <pthread.h>

int g_play_flag = FALSE;
int g_record_flag = FALSE;

// long long get_us_time(void)
// {
    // struct timeval  tv;
    // struct timezone tz;
    // gettimeofday(&tv, &tz);
	// long long sum = 0;
	// sum = (tv.tv_sec-1419998000)*1000000 + tv.tv_usec;
	// return sum;
    // //printf("tv_sec:%ld\n",tv.tv_sec);
    // //printf("tv_usec:%ld\n",tv.tv_usec);
    // //printf("time_now:%d%d%d%d%d%d.%ld\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
// }

// void cal_delta_us_time(long long start, long long end)
// {
	
	// //printf("%lld - %lld = %lld\n",end,start,end-start);
// }


int alsa_test(int dir)
{
    int ret;
	int aaa = 0;

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
		aaa ++;
		//printf("thread_id:%d,dir = %d,aaa=%d\n",pthread_self(),dir,aaa);
		if(dir == AUDIO_PLAY)
		{
			if(g_play_flag == FALSE)
			{
				printf("~~~~~play stop\n");
				usleep(500000); // 500ms
				continue;
			}
			else
			{
				//printf("%d dir:%d,g_play:%d,g_record:%d\n",pthread_self(),dir,g_play_flag, g_record_flag);
				//continue;
				printf("~");
			}
		}
		else if(dir == AUDIO_RECORD)
		{
			if (g_record_flag == FALSE)
			{
				printf("!!!!!record stop\n");
				usleep(500000);
				continue;
			}
			else
			{
				//printf("%d dir:%d,g_play:%d,g_record:%d\n",pthread_self(),dir,g_play_flag, g_record_flag);
				//continue;
				printf("!");
			}
		}
		
		
		//printf("^^^");
		//printf("^^^ jump out!thread_id:%d,dir = %d,aaa=%d\n",pthread_self(),dir,aaa);
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
			//return PLAY_FAIL;
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
					fprintf(stderr, "dir %d, error from writei: %s\n", dir, snd_strerror(ret));
				}
			}
		}
		
    }
	destroy_recorder(&record_handle);
	
	return RECORD_SUCCESS;
}

void *thread_alsa_test( void *arg)
{
    alsa_test(*((int*)arg));
	return ((void*)0);
}

