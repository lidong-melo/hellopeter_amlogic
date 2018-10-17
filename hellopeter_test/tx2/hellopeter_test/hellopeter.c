#include "hellopeter.h"



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

void *thread_alsa_record_test(void * arg)
{
    int ret;
	int dir = *((int*)arg);
	int i=0;
	char test_buf[50] = {0};
	//char str_test_flag[30] = "this is lidong test!\n";
//	char str_test_flag[30] = "this";
	char * pt_char = 0;
	int time_a113 = 0;
	int time_tx2 = 0;
	int delta = 0;

	//play_handle_t play_handle;
	record_handle_t record_handle;
	
	// if(dir == AUDIO_PLAY)
	// {
		// play_handle.channels = PLAY_CHANNELS;
		// play_handle.rate = PLAY_RATE;
		// play_handle.device_name = (char*)PLAY_OUT_DEVICE_NAME;
		// record_handle.device_name = (char*)PLAY_IN_DEVICE_NAME;
	// }
	// else if(dir == AUDIO_RECORD)
	// {	
		// play_handle.channels = RECORD_CHANNELS;
		// play_handle.rate = RECORD_RATE;
		// play_handle.device_name = (char*)RECORD_OUT_DEVICE_NAME;
		record_handle.device_name = (char*)RECORD_IN_DEVICE_NAME;
	
	// }

	// play_handle.format = SND_PCM_FORMAT_S16_LE;	
	
	record_handle.channels = RECORD_CHANNELS;
	record_handle.rate =  RECORD_RATE;
	record_handle.format =  SND_PCM_FORMAT_S16_LE;
	record_handle.buffer_time = BUFFER_TIME;
	record_handle.periods = PERIODS;
	
	
    if(init_recorder(&record_handle)==PLAY_FAIL){
        log_out("init_recorder fail.\n");
        return ((void*)PLAY_FAIL);
    }
	
	// play_handle.buffer = record_handle.buffer;
	// play_handle.frames = record_handle.chunk_size*record_handle.periods;
	record_handle.frames = record_handle.chunk_size * record_handle.periods;
	log_out("record_handle.frames = record_handle.chunk_size * record_handle.periods: %d = %d * %d \n", (int)record_handle.frames, (int)record_handle.chunk_size, (int)record_handle.periods);
	
	while (1)
	{		
		
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, record_handle.frames);
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
			for(i=0;i<1024*16-5;i++ )
			{
				if((record_handle.buffer[i] == 50) && (record_handle.buffer[i+1] == 51)&&(record_handle.buffer[i+2] == 52)&&(record_handle.buffer[i+3] == 53)&&(record_handle.buffer[i+4] == 54)&&(record_handle.buffer[i+5] == 55))
				{
					get_systime(test_buf);
					log_out("\n-----------------");
					log_out("record channel find index:%d\n",i);
					log_out("tx2 time is: %s\n", test_buf);
					pt_char = strchr(test_buf, '.');
					//printf("%s\n", pt_char);
					time_tx2 = atoi(pt_char+1);
					memcpy(test_buf, record_handle.buffer+i+6, 30); // copy time stamp
					log_out("a113 time is: %s\n", test_buf);
					pt_char = strchr(test_buf, '.');
					time_a113 = atoi(pt_char+1);
					delta = time_tx2 - time_a113;
					if( delta < 0)
						delta += 1000000;
					printf("delta: %d-%d = %d.%dms\n", time_tx2,time_a113, delta/1000,delta%1000);
					break;
				}
			}
		}
		
    }
	uninit_recorder(&record_handle);
	
	return ((void*)RECORD_SUCCESS);
}



void *thread_alsa_play_test(void * arg)
{
    int ret;
	int dir = *((int*)arg);
	char test_buf[50] = {0};
//	char str_test_flag[30] = "this is lidong test!\n";
	int cycle = 20;
	int count = 0;
	
	play_handle_t play_handle;
	//record_handle_t record_handle;
	
	//if(dir == AUDIO_PLAY)
	{
		play_handle.channels = PLAY_CHANNELS;
		play_handle.rate = PLAY_RATE;
		play_handle.device_name = (char*)PLAY_OUT_DEVICE_NAME;
		//record_handle.device_name = (char*)PLAY_IN_DEVICE_NAME;
	}
	

	play_handle.format = SND_PCM_FORMAT_S16_LE;	
	
	// record_handle.channels = play_handle.channels;
	// record_handle.rate =  play_handle.rate;
	// record_handle.format =  play_handle.format;
	// record_handle.buffer_time = BUFFER_TIME;
	// record_handle.periods = PERIODS;
	
	
								
	if (alsa_play_init(&play_handle) <0)
	{
		log_out("play init fail.\n");
		return ((void*)PLAY_FAIL);
	}
	
	play_handle.frames = 3072;
	play_handle.frame_size = 4;
	play_handle.buffer = (char*)malloc(play_handle.frames * play_handle.frame_size);
	
	while (1)
	{
		// copy test_flag & time into buffer
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
		else
		{
			play_handle.buffer[0] = 0;
			play_handle.buffer[1] = 1;
			play_handle.buffer[2] = 2;
			play_handle.buffer[3] = 3;
			play_handle.buffer[4] = 4;
			play_handle.buffer[5] = 5;	
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
	//uninit_recorder(&record_handle);
	
	return ((void*)RECORD_SUCCESS);
}



int main(int argc, char *argv[])
{
	int arg1 = AUDIO_PLAY;
    int arg2 = AUDIO_RECORD;
	
	char str_amlogic_ready[] = "Amlogic ready\n";
	
	int ret_thrd1, ret_thrd2, ret_thrd3;
    
	pthread_t thread1, thread2, thread3;

	time_t file_name_time = time(NULL);
	struct tm* tm_log = localtime(&file_name_time);
	char file_path[200];
	int volume_value = 0;
	
	//直接调用mkdir函数
	//建立一个名为log的文件夹
	//权限为0777，即拥有者权限为读、写、执行, 拥有者所在组的权限为读、写、执行, 其它用户的权限为读、写、执行
	if(access("./log", F_OK) != 0)
	{  
		printf("dir log does not exist!\n");
		if(mkdir("./log", 0755) == -1)  
		{   
			log_init("mkdir ./log error\n");  
			printf("create log dir failed!\n");
			return -1;   
		}
		else
		{
			printf("create log dir successfully!\n");
			
		}
	}
	else
	{
		printf("dir log already exist!\n");
	}
	
	//FILE * pFile;
	sprintf(file_path,"./log/log_%04d%02d%02d_%02d%02d%02d.txt",tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
	
	int ret_log = log_init(file_path);
	
	if (ret_log != 0)
	{
		printf("log file create fail!\n");
	}
	else
	{
		printf("log file create successfully! log file = %s\n", file_path);
	}
	

    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_play_test, (void*)&arg1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_record_test, (void*)&arg2);
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
	//volume_init();

	while(1)
	{
		sleep(1);
		//volume_value = volume_read();
		printf("volume=%d\n",volume_value);
		// if (flag_register == FALSE)
			// UART0_Send(fd,str_amlogic_ready,strlen(str_amlogic_ready));
	}

	log_uninit();
	return 0;
} 
