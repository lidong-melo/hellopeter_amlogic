#include "hellopeter_test.h"



app_fifo_t recv_fifo_p;
uint8_t recv_fifo_buf[256];


static int flag_play = TRUE;
static int flag_record = TRUE;
static int flag_register = FALSE;	
static int fd;                            //文件描述符 

void get_systime(char * str_time)
{
    struct timeval    tv;
    struct timezone tz; 
    struct tm         *p;
    
    gettimeofday(&tv, &tz);
    //log_out("tv_sec:%ld\n",tv.tv_sec);
    //log_out("tv_usec:%ld\n",tv.tv_usec);
    
    p = localtime(&tv.tv_sec);
	//printf("%d%d%d-%d:%d:%d.%ld\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
    sprintf(str_time, "%d%d%d-%d:%d:%d.%ld", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
	return;
}

void *thread_alsa_test(void * arg)
{
    int ret;
	int dir = *((int*)arg);
	char test_buf[60] = {0};
	char str_test_flag[21] = "this is lidong test!";
	//char str_record_test_flag[30] = "this is lidong test!\n";
	int cycle = 20;
	int count = 0;
	int i=0;

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
		log_out("play init fail.\n");
		return ((void*)PLAY_FAIL);
	}
	
    if(init_recorder(&record_handle)==PLAY_FAIL){
        log_out("init_recorder fail.\n");
        return ((void*)PLAY_FAIL);
    }
	
	play_handle.buffer = record_handle.buffer;
	play_handle.frames = record_handle.chunk_size*record_handle.periods;
	// periods =4
	
	while (1)
	{
		if(dir == AUDIO_PLAY)
		{
			if(flag_play == FALSE)
			{
				//log_out("~~~~~play stop\n");
				usleep(50000); // 50ms
				continue;
			}
			else
			{
				//log_out("~");
			}
		}
		else if(dir == AUDIO_RECORD)
		{
			if (flag_record == FALSE)
			{
				//log_out("!!!!!record stop\n");
				usleep(50000);
				continue;
			}
			else
			{
				//log_out("^");
			}
		}
		
		
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, play_handle.frames);
		if (ret == -EAGAIN ) {
			snd_pcm_wait(record_handle.pcm, 1000);
		} else if (ret == -EPIPE) {
			snd_pcm_prepare(record_handle.pcm);
			log_out("snd_pcm_readi return EPIPE.\n");
		} else if (ret == -ESTRPIPE) {
			log_out("snd_pcm_readi return ESTRPIPE.\n");
		} else if (ret < 0) {
			log_out("snd_pcm_readi return fail.\n");
		}

		if(ret>0)
		{
			if(dir == AUDIO_RECORD)
			{
				count++;

				if(count == cycle)
				{
					count = 0;									
					play_handle.buffer[0] = 50;
					play_handle.buffer[1] = 51;
					play_handle.buffer[2] = 52;
					play_handle.buffer[3] = 53;
					play_handle.buffer[4] = 54;
					play_handle.buffer[5] = 55;
					get_systime(test_buf);
					memcpy(&(play_handle.buffer[6]), test_buf, 30); // copy time stamp
				}
			}
			else if(dir == AUDIO_PLAY)
			{
				for(i=0;i<3072*4-5;i++ )
				{
					if((record_handle.buffer[i] == 50) && (record_handle.buffer[i+1] == 51)&&(record_handle.buffer[i+2] == 52)&&(record_handle.buffer[i+3] == 53)&&(record_handle.buffer[i+4] == 54)&&(record_handle.buffer[i+5] == 55))
					{
						get_systime(test_buf);
						log_out("\n-----------------");
						log_out("play channel find index:%d\n",i);
						log_out("a113 time is: %s\n", test_buf);
						memcpy(test_buf, record_handle.buffer+i+6, 30); // copy time stamp
						log_out("tx2 time is: %s\n", test_buf);
						break;
					}
				}
			}
			while((ret = snd_pcm_writei(play_handle.pcm, play_handle.buffer, play_handle.frames))<0)
			{
				usleep(1000);
				if(ret == -EPIPE)
				{
					// EPIPE means underrun 
					log_out("underrun occurred\n");
					//完成硬件参数设置，使设备准备好
					snd_pcm_prepare(play_handle.pcm);
				}
				else if (ret < 0)
				{
					log_out("dir %d, error from writei: %s\n", dir, snd_strerror(ret));
				}
			}
		}
		
    }
	uninit_recorder(&record_handle);
	
	return ((void*)RECORD_SUCCESS);
}




int main(int argc, char *argv[])
{
	int arg1 = AUDIO_PLAY;
    int arg2 = AUDIO_RECORD;
	
	char str_amlogic_ready[] = "Amlogic ready\n";
	
	int ret_thrd1, ret_thrd2, ret_thrd3;
    
	pthread_t thread1, thread2, thread3;
	
	int ret_log = log_init();
	
	if (ret_log != 0)
	{
		log_out("log file init fail!\n");
	}
	

    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_test, (void*)&arg1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_test, (void*)&arg2);
	//ret_thrd3 = pthread_create(&thread2, NULL, &thread_serial_test, (void*)0);

    // 线程创建成功，返回0,失败返回失败号
    if (ret_thrd1 != 0) {
        log_out("Create play thread failed!\n");
    } else {
        log_out("Create play thread successfully!\n");
    }

    if (ret_thrd2 != 0) {
        log_out("Create record thread failed!\n");
    } else {
        log_out("Create record thread successfully!\n");
    }
	
    // if (ret_thrd3 != 0) {
        // log_out("Create serial thread failed!\n");
    // } else {
        // log_out("Create serial thread successfully!\n");
    // }	

	while(1)
	{
		sleep(1);
		// if (flag_register == FALSE)
			// UART0_Send(fd,str_amlogic_ready,strlen(str_amlogic_ready));
	}

	log_uninit();
	return 0;
} 
