#include "hellopeter.h"



app_fifo_t recv_fifo_p;
uint8_t recv_fifo_buf[256];


static int flag_play = TRUE;
static int flag_record = TRUE;


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



void *thread_serial_test( void *arg)
{  
    int fd;                            //文件描述符  
    int err;                           //返回调用函数的状态  
    int len;
	char port_id[20]="/dev/ttyGS0";
	
	
    char rcv_buf[256];
	char line_buf[256];
	uint32_t line_len = 0;
    
	//char *str_temp;
	char str_play[] = "start play";
	char str_record[] = "start record";
	char str_stop[] = "stop";
	char str_led_on[] = "led_on";
	char str_led_off[] = "led_off";
	char str_fail[] = "can't recognize: ";
	char str_temp[300];
	
	//GPIONumber gpio_id = gpio500;
	GPIONumber gpio_id = gpio505;
	
	//log_out("%d,%d,%d",strlen(str_play),strlen(str_record),strlen(str_stop));
	
	app_fifo_init(&recv_fifo_p, recv_fifo_buf, 256);
	
    fd = UART0_Open(fd, port_id);
	if(fd == FALSE)
		log_out("Open port %d failed!\n", fd);
	else
		log_out("Open port %d success!\n", fd);
	gpio_init(gpio_id);
	gpioSetValue(gpio_id, low);
	
    do
	{ 
		err = UART0_Init(fd,115200,0,8,1,'N');
		if(FALSE == err || FALSE == fd)
		{
			log_out("Set port failed!\n");
			sleep(1);
		}
		else
			log_out("Set port success!\n");
		//log_out("Set Port Exactly!\n");
	}while(FALSE == err || FALSE == fd);
     
                                       
	while (1) //循环读取数据  
	{    
		UART0_Recv(fd, rcv_buf,1);
		app_fifo_put(&recv_fifo_p, rcv_buf[0]);
		if (rcv_buf[0] == '\n')
		{
			line_len = app_fifo_length(&recv_fifo_p);
			app_fifo_read(&recv_fifo_p, (u8*)line_buf, &line_len);
			line_buf[line_len] = '\0';

			log_out("uart recv: %s",line_buf);
			
			if (strstr(line_buf, str_stop) != NULL)
			{
				UART0_Send(fd,str_stop,strlen(str_stop));
				flag_play = FALSE;
				flag_record = FALSE;
				log_out("stop play & record!\n");
			}
			else if (strstr(line_buf, str_play) != NULL)
			{
				UART0_Send(fd,str_play,strlen(str_play));
				flag_play = TRUE;
				log_out("start play!\n");
			}
			else if (strstr(line_buf, str_record) != NULL)
			{
				UART0_Send(fd,str_record,strlen(str_record));
				flag_record = TRUE;
				log_out("start record!\n");
			}
			else if (strstr(line_buf, str_led_on) != NULL)
			{
				UART0_Send(fd,str_led_on,strlen(str_led_on));
				gpioSetValue(gpio_id, high);
				log_out("led on!\n");
			}
			else if (strstr(line_buf, str_led_off) != NULL)
			{
				UART0_Send(fd,str_led_off,strlen(str_led_off));
				gpioSetValue(gpio_id, low);
				log_out("led off!\n");
			}
			else
			{
				strcpy(str_temp, str_fail);
				strcat(str_temp, line_buf);
				len = UART0_Send(fd,str_temp,strlen(str_temp)-1);//最后一个回车不发送
				if(len == 0)
					log_out("uart send failed!\n");
				else
					log_out("uart send: %s",str_temp);
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
	
	int ret_log = log_init();
	
	if (ret_log != 0)
	{
		log_out("log file init fail!\n");
	}
	

    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_test, (void*)&arg1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_test, (void*)&arg2);
	ret_thrd3 = pthread_create(&thread2, NULL, &thread_serial_test, (void*)0);

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
	
    if (ret_thrd3 != 0) {
        log_out("Create serial thread failed!\n");
    } else {
        log_out("Create serial thread successfully!\n");
    }	

	while(1)
	{
		sleep(1);
	}

	log_uninit();
	return 0;
} 
