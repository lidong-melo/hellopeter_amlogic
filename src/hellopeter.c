#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "alsalib_test.h"
//#include "multi_thread.h"
#include "serial_test.h"
#include "gpio.h"
#include "fifo.h"



app_fifo_t recv_fifo_p;
uint8_t recv_fifo_buf[256];

void *thread_serial( void *arg)
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
		len = UART0_Recv(fd, rcv_buf,1);
		app_fifo_put(&recv_fifo_p, rcv_buf[0]);
		if (rcv_buf[0] == '\n')
		{
			find_ret = 0;
			line_len = fifo_length(&recv_fifo_p);
			app_fifo_read(&recv_fifo_p, (u8*)line_buf, &line_len);
			line_buf[line_len] = '\0';

			if (strstr(line_buf, str_stop) != NULL)
			{
				find_ret = 1;
				str_temp = str_stop;
				g_play_flag = FALSE;
				g_record_flag = FALSE;
				gpioSetValue(gpio_id, high);
			}
			else if (strstr(line_buf, str_play) != NULL)
			{
				find_ret = 2;
				str_temp = str_play;
				g_play_flag = TRUE;
			}
			else if (strstr(line_buf, str_record) != NULL)
			{
				find_ret = 3;
				str_temp = str_record;
				g_record_flag = TRUE;
				gpioSetValue(gpio_id, low);				
			}
			
				
			if (find_ret == 0)
				printf("can't recognize: %s\n",line_buf);
			else
			{
				printf("recv: %s\n",line_buf);
				UART0_Send(fd,str_temp,strlen(str_temp));
				if(len == 0)
					printf("uart send failed!\n");
				else
					printf("send %s\n",str_temp);
			}
		}
	}
	gpio_uninit(gpio_id);	
	UART0_Close(fd);  
}  




int main_test()
{

    int tmp1, tmp2;
    void *retval;
    pthread_t thread1, thread2, thread3;
    int id1 = 1;
    int id2 = 2;

    int ret_thrd1, ret_thrd2, ret_thrd3;

	
    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_test, (void*)&id1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_test, (void*)&id2);
	ret_thrd3 = pthread_create(&thread2, NULL, &thread_serial, (void*)0);

    // 线程创建成功，返回0,失败返回失败号
    if (ret_thrd1 != 0) {
        printf("线程1创建失败\n");
    } else {
        printf("线程1创建成功,%d\n",ret_thrd1);
    }

    if (ret_thrd2 != 0) {
        printf("线程2创建失败\n");
    } else {
        printf("线程2创建成功,%d\n",ret_thrd2);
    }
	
    if (ret_thrd3 != 0) {
        printf("线程3创建失败\n");
    } else {
        printf("线程3创建成功\n");
    }	

    // //同样，pthread_join的返回值成功为0
    // tmp1 = pthread_join(thread1, &retval);
    // printf("thread1 return value(retval) is %d\n", (int)retval);
    // printf("thread1 return value(tmp) is %d\n", tmp1);
    // if (tmp1 != 0) {
        // printf("cannot join with thread1\n");
    // }
    // printf("thread1 end\n");

    // tmp2 = pthread_join(thread1, &retval);
    // printf("thread2 return value(retval) is %d\n", (int)retval);
    // printf("thread2 return value(tmp) is %d\n", tmp1);
    // if (tmp2 != 0) {
        // printf("cannot join with thread2\n");
    // }
    // printf("thread2 end\n");
	while(1)
	{
		sleep(1);
	}

}

 


int main(int argc, char *argv[])
{
	GPIONumber gpioao_8 = gpio505;
	GPIONumber gpioao_3 = gpio500;
	//alsa_test(AUDIO_PLAY);
//	alsa_test(AUDIO_RECORD);
//	gpio_test(gpioao_3);
	main_test();
//	thread_serial();
//	serial_test(0);
    return 0;
}   
