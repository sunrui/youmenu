/*
 * public log provider
 *
 * author rui.sun 2012-7-3 Tue 5:03
 */
#include "../include/log.h"

#include <stdio.h>

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