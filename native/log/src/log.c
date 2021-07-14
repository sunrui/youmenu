/*
 * public log provider
 *
 * author rui.sun 2012-7-3 Tue 5:03
 *
 * update
 *    * for colored fputs rui.sun 2012-8-16 Thu 4:56
 */
#include "log.h"

#ifndef LOG_DISABLE

#include "colored_fputs.h"

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

log_level a_level = log_level_debug;
pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;
int a_console_enable = 1;
FILE * a_fp;

#define CHECK_LEVEL(level) (level <= a_level)

int log_init(const char * localfile)
{
	log_fini();
	
	a_fp = fopen(localfile, "a+");
	if (a_fp != NULL)
	{
		return 1;
	}
	
	return 0;
}

/*
 * set log c only show level lower in
 */
void log_set_level(log_level level)
{
	a_level = level;
}

void log_set_console(int enable)
{
	a_console_enable = enable;
}

void log_output(log_level level, const char * c_file, const char * function, int line, const char * format, ...)
{
	char string[1024];
	char log[1000];
	struct timeval tv;
	struct tm * tm;
	const char * c_level;
	
	if (a_fp == NULL && !a_console_enable)
	{
		return;
	}
	
	{
		va_list args;
		
		va_start(args, format);
		vsnprintf(log, sizeof(log) - 1, format, args);
		va_end(args);
	}

	{
		const char * suffix_file;
		
		suffix_file = strrchr(c_file, '/');
		if (suffix_file == NULL)
			suffix_file = strrchr(c_file, '\\');
		if (suffix_file != NULL)
			suffix_file += 1;
		if (suffix_file == NULL)
			suffix_file = c_file;
		c_file = suffix_file;
	}
	
	switch (level)
	{
		case log_level_debug:
			c_level = "DEBUG";
			break;
		case log_level_info:
			c_level = "INFO";
			break;
		case log_level_warn:
			c_level = "WARN";
			break;
		case log_level_error:
		default:
			c_level = "ERROR";
			break;
	}
	
	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	
	snprintf(string, sizeof(string) - 1, "%04d-%02d-%02d %02d:%02d:%02d.%03d %-5s - %s [%s/%s(%d)]\r\n",
			 tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			 tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000,
			 c_level, log, c_file, function, line);

	pthread_mutex_lock(&a_lock);
	
	if (a_console_enable)
	{
		colored_fputs(level, string);
	}

	if (a_fp != NULL)
	{
		fprintf(a_fp, string, NULL);
	}
	
	pthread_mutex_unlock(&a_lock);
}

void log_output2(const char * format, ...)
{
	char string[1024];
	char log[1000];
	struct timeval tv;
	struct tm * tm;
	
	if (a_fp == NULL && !a_console_enable)
	{
		return;
	}
	
	{
		va_list args;
		
		va_start(args, format);
		vsnprintf(log, sizeof(log) - 1, format, args);
		va_end(args);
	}

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	
	snprintf(string, sizeof(string) - 1, "%04d-%02d-%02d %02d:%02d:%02d.%03d         %s\r\n",
			 tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			 tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000,
			 log);
	
	pthread_mutex_lock(&a_lock);
	
	if (a_console_enable)
	{
		colored_fputs(log_level_info, string);
	}
	
	if (a_fp != NULL)
	{
		fprintf(a_fp, string, NULL);
	}
	
	pthread_mutex_unlock(&a_lock);
}

void log_fini()
{
	if (a_fp != NULL)
	{
		fflush(a_fp);
		fclose(a_fp);
		a_fp = NULL;
	}
}

#ifdef TEST_MAIN
int main(int argc, char * argv[])
{
	log_init("log.log");
	
	log_set_level(log_level_debug);
	
	LOGI("i am a info message");
	LOGD("i am a debug message");
	LOGW("i am warn!!!");
	LOGE("i am error!!! crazy!!");
	
	LOGI("hello world %d", 99);
	LOGE("connect to server %s failed.", "127.0.0.1");
	
	log_fini();
	
	return 0;
}
#endif

#endif