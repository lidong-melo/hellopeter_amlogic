#ifndef _LOG_H
#define _LOG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

//#define log_out printf
int log_out (const char *format, ...);
 
int log_test(void);

int log_init(char * file_path);

int log_uninit(void);

#endif

