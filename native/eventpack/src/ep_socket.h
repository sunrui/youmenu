/*
 * service socket module
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2013-11-2
 */
#ifndef EP_SERVICE_CORE_H
#define EP_SERVICE_CORE_H

typedef struct service_core_cb
{
	void (* on_accept)(void * opaque, int fd, void * extra);
	/*
	 * @return
	 *     1 if read ok
	 *     0 if you need do shutdown this fd
	 */
	int (* on_read)(void * opaque, int fd, const char * buffer, int size);
	void (* on_close)(void * opaque, int fd);
} service_core_cb;

/*
 * start engine
 *
 * @return
 *     1 start ok
 *     0 error
 */
int service_core_start(int port, int keepalive, service_core_cb core_cb, void * opaque, char * err_string);
int service_core_send(int fd, char * buffer, int size, void * extra);
void service_core_stop();

#endif