#include "log.h"

static FILE* pFile =  NULL;



// int log_out (const char *format, ...) 
// {
	// va_list arg;
	// int done;
 
	// va_start (arg, format);
	// time_t time_log = time(NULL);
	// struct tm* tm_log = localtime(&time_log);
	// if(pFile == 0)
		// return 0;

	// fprintf(pFile,"%04d-%02d-%02d %02d:%02d:%02d ",tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
 
	// done = vfprintf (pFile, format, arg);
	// va_end (arg);
	// fflush(pFile);
	// return done;
// }
 
int log_test(void) 
{
	log_init();
	if (pFile == NULL)
	{
		log_out("log file init fail!\n");
	}
	else
	{
		log_out("log file init successful!\n");
	}
	log_uninit();
	pFile = NULL;
	return 0;
}


int log_init(void)
{
	time_t file_name_time = time(NULL);
	struct tm* tm_log = localtime(&file_name_time);
	char file_path[200];
	//FILE * pFile;
	sprintf(file_path,"./log_%04d%02d%02d_%02d%02d%02d.txt",tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
	pFile = fopen(file_path, "w+");
	if(NULL == pFile)
		return -1;
	else
		return 0;
}

int log_uninit(void)
{
	printf("uninit log\n");
	fclose(pFile);
	pFile = NULL;
	return 0;
}