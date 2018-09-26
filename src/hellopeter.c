#include "hellopeter.h"



app_fifo_t recv_fifo_p;
uint8_t recv_fifo_buf[256];


static int flag_play = FALSE;
static int flag_record = FALSE;


void *thread_alsa_test(void * arg)
{
    int ret;
	int dir = *((int*)arg);

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
		return ((void*)PLAY_FAIL);
	}
	
    if(init_recorder(&record_handle)==PLAY_FAIL){
        printf("init_recorder fail.\n");
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
				//printf("~~~~~play stop\n");
				usleep(50000); // 50ms
				continue;
			}
			else
			{
				printf("~");
			}
		}
		else if(dir == AUDIO_RECORD)
		{
			if (flag_record == FALSE)
			{
				//printf("!!!!!record stop\n");
				usleep(50000);
				continue;
			}
			else
			{
				printf("^");
			}
		}
		
		
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
	uninit_recorder(&record_handle);
	
	return ((void*)RECORD_SUCCESS);
}



void *thread_serial_test( void *arg)
{  
    int fd;                            //文件描述符  
    int err;                           //返回调用函数的状态  
    int len;
	char port_id[20]="/dev/ttyGS0";
	
	
    char rcv_buf[256];
	char line_buf[256];
	uint32_t line_len = 0;
    
	char *str_temp;
	char str_play[] = "start play";
	char str_record[] = "start record";
	char str_stop[] = "stop";
	char find_ret = 0;
	
	GPIONumber gpio_id = gpio500;
	//GPIONumber gpio_id = gpio500;
	
	//printf("%d,%d,%d",strlen(str_play),strlen(str_record),strlen(str_stop));
	
	app_fifo_init(&recv_fifo_p, recv_fifo_buf, 256);
	
    fd = UART0_Open(fd, port_id);
	gpio_init(gpio_id);
	
    do
	{  
		err = UART0_Init(fd,115200,0,8,1,'N');  
		//printf("Set Port Exactly!\n");  
	}while(FALSE == err || FALSE == fd);  
     
                                       
	while (1) //循环读取数据  
	{    
		UART0_Recv(fd, rcv_buf,1);
		app_fifo_put(&recv_fifo_p, rcv_buf[0]);
		if (rcv_buf[0] == '\n')
		{
			find_ret = 0;
			line_len = app_fifo_length(&recv_fifo_p);
			app_fifo_read(&recv_fifo_p, (u8*)line_buf, &line_len);
			line_buf[line_len] = '\0';

			if (strstr(line_buf, str_stop) != NULL)
			{
				find_ret = 1;
				str_temp = str_stop;
				flag_play = FALSE;
				flag_record = FALSE;
				gpioSetValue(gpio_id, high);
				printf("stop play & record!\n");
			}
			else if (strstr(line_buf, str_play) != NULL)
			{
				find_ret = 2;
				str_temp = str_play;
				flag_play = TRUE;
				printf("start play!\n");
			}
			else if (strstr(line_buf, str_record) != NULL)
			{
				find_ret = 3;
				str_temp = str_record;
				flag_record = TRUE;
				gpioSetValue(gpio_id, low);
				printf("start record!\n");
			}
			
				
			if (find_ret == 0)
				printf("can't recognize: %s\n",line_buf);
			else
			{
				printf("recv: %s\n",line_buf);
				len = UART0_Send(fd,str_temp,strlen(str_temp));
				if(len == 0)
					printf("uart send failed!\n");
				else
					printf("send: %s\n",str_temp);
			}
		}
	}
	gpio_uninit(gpio_id);	
	UART0_Close(fd);  
}  




int main(int argc, char *argv[])
{
	int arg1 = AUDIO_PLAY;
    int arg2 = AUDIO_RECORD;
	int ret_thrd1, ret_thrd2, ret_thrd3;
    
	pthread_t thread1, thread2, thread3;

    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_test, (void*)&arg1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_test, (void*)&arg2);
	ret_thrd3 = pthread_create(&thread2, NULL, &thread_serial_test, (void*)0);

    // 线程创建成功，返回0,失败返回失败号
    if (ret_thrd1 != 0) {
        printf("Create play thread failed!\n");
    } else {
        printf("Create play thread !\n");
    }

    if (ret_thrd2 != 0) {
        printf("Create record thread failed!\n");
    } else {
        printf("Create record thread successfully!\n");
    }
	
    if (ret_thrd3 != 0) {
        printf("Create serial thread failed!\n");
    } else {
        printf("Create serial thread successfully!\n");
    }	

	while(1)
	{
		sleep(1);
	}
	return 0;
} 
