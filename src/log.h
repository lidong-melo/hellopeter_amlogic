#ifndef _LOG_H
#define _LOG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>


int log_out (const char *format, ...);
 
int log_test(void);

int log_init(void);

int log_uninit(void);

#endif

