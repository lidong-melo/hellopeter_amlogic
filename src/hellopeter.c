#include "hellopeter.h"



app_fifo_t recv_fifo_p;
uint8_t recv_fifo_buf[256];


static int flag_play = TRUE;
static int flag_record = TRUE;
static int flag_register = FALSE;	
static int fd;                            //文件描述符 



void *thread_alsa_test(void * arg)
{
    int ret;
	int dir = *((int*)arg);
	int count1 = 0;
	int count2 = 0;

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
		
		
		ret = snd_pcm_readi(record_handle.pcm, record_handle.buffer, play_handle.frames/4);
		if (ret == -EAGAIN ) {
			snd_pcm_wait(record_handle.pcm, 1000);
		} else if (ret == -EPIPE) {
			snd_pcm_prepare(record_handle.pcm);
			log_out("dir %d, snd_pcm_readi return EPIPE.\n", dir);
		} else if (ret == -ESTRPIPE) {
			log_out("dir %d, snd_pcm_readi return ESTRPIPE.\n", dir);
		} else if (ret < 0) {
			if(count1++ == 30)
			{
				count1 = 0;
				log_out("10 times: dir %d, snd_pcm_readi return fail.\n", dir);
			}
			
		}

		if(ret>0)
		{
			while((ret = snd_pcm_writei(play_handle.pcm, play_handle.buffer, play_handle.frames/4))<0)
			{
				usleep(1000);
				if(ret == -EPIPE)
				{
					// EPIPE means underrun 
					log_out("dir %d, underrun occurred\n", dir);
					//完成硬件参数设置，使设备准备好
					snd_pcm_prepare(play_handle.pcm);
				}
				else if (ret < 0)
				{
					if(count2++ == 30)
					{
						count2 = 0;
						log_out("10 times: dir %d, error from writei\n", dir);
					}
				}
			}
		}
		
    }
	uninit_recorder(&record_handle);
	
	return ((void*)RECORD_SUCCESS);
}



void *thread_serial_test( void *arg)
{  

    int err;                           //返回调用函数的状态  
    int len;
	char port_id[20]="/dev/ttyGS0";
	
	
    char rcv_buf[256]={0};
	char line_buf[256]={0};
	uint32_t line_len = 0;
    
	//char *str_temp;
	char str_play[] = "start play\n";
	char str_record[] = "start record\n";
	char str_stop[] = "stop\n";
	char str_led_on[] = "led on\n";
	char str_led_off[] = "led off\n";
	char str_fail[] = "can't recognize: ";
	
	char str_tx2_ready[] = "tx2 ready!\n";
	char str_temp[300]= {0};
	
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
			else if(strstr(line_buf, str_tx2_ready) != NULL)
			{
				flag_register = TRUE;
				UART0_Send(fd,str_tx2_ready,strlen(str_tx2_ready));
				log_out("tx2 is ready!\n");
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
	
	char str_amlogic_ready[] = "Amlogic ready\n";
	char str_mkdir[] = "mkdir ./log error\n";
	char file_path[200];
	
	int ret_thrd1, ret_thrd2, ret_thrd3;
	pthread_t thread1, thread2, thread3;
	
	time_t file_name_time = time(NULL);
    struct tm* tm_log = localtime(&file_name_time);
	

	
	//直接调用mkdir函数
	//建立一个名为log的文件夹
	//权限为0777，即拥有者权限为读、写、执行, 拥有者所在组的权限为读、写、执行, 其它用户的权限为读、写、执行
	if(access("./log", F_OK) != 0)
	{  
		printf("dir log does not exist!\n");
		if(mkdir("./log", 0755) == -1)  
		{   
			log_init(str_mkdir);  
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
	
	
	if (argc > 1)
	{
		sprintf(file_path,"./log/log_%s.txt", argv[1]);
	}
	else
	{	
		//FILE * pFile;
		sprintf(file_path,"./log/log_%04d%02d%02d_%02d%02d%02d.txt",tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
	}
	int ret_log = log_init(file_path);

	

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
