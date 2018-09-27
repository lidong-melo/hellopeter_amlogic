#ifndef _SERIAL_PORT_
#define _SERIAL_PORT_

#include <stdio.h>      //标准输入输出定义
#include <stdlib.h>     //标准函数库定义 
#include <unistd.h>     //Unix 标准函数定义
#include <fcntl.h>      //文件控制定义
#include <termios.h>    //PPSIX 终端控制定义
#include <errno.h>      //错误号定义
#include "log.h"

//宏定义  
#define FALSE  -1  
#define TRUE   0 

//int thread_serial(void);

int UART0_Open(int fd,char* port);
void UART0_Close(int fd);
int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
int UART0_Recv(int fd, char *rcv_buf,int data_len);
int UART0_Send(int fd, char *send_buf,int data_len);



#endif