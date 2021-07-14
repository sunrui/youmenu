/*
 * service socket module
 *
 * Copyright (C) 2012-2014 honeysense.com
 * @author rui.sun 2014-4-12
 */
#include "ep_socket.h"

#include "ae.h"
#include "anet.h"

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#ifdef EP_HAVE_SERVER

static aeEventLoop * s_event_loop;
static pthread_t s_loop_thread;
static int s_keepalive;
static int s_fd;

static service_core_cb s_core_cb;
static void * s_core_opaque;

void aeClientClose(aeEventLoop * el, int fd)
{
	aeDeleteFileEvent(el, fd, AE_READABLE);
	close(fd);
	
	s_core_cb.on_close(s_core_opaque, fd);
}

#define MAX_LINE 65535
void aeReadFromClient(aeEventLoop * el, int fd, void *privdata, int mask)
{
	char buffer[MAX_LINE];
	int r;
	
	r = (int)read(fd, buffer, sizeof(buffer));
	if (r <= 0)
	{
		aeClientClose(el, fd);
		return;
	}
	
	r = s_core_cb.on_read(s_core_opaque, fd, buffer, r);
	if (r != 1)
	{
		aeClientClose(el, fd);
	}
}

void aeAcceptTcpHandler(aeEventLoop * el, int fd, void *privdata, int mask)
{
	char ip_addr[128] = { 0 };
	char err_string[256];
	int c_fd, c_port;
	int r;
	
	c_fd = anetTcpAccept(err_string, fd, ip_addr, strlen(ip_addr), &c_port);
    
    anetKeepAlive(err_string, c_fd, s_keepalive);
    
	r = aeCreateFileEvent(el, c_fd, AE_READABLE, aeReadFromClient, NULL);
	if (r == AE_ERR)
	{
		fprintf(stderr, "client connect fail: %d\n", fd);
		close(c_fd);
        return;
	}
    
	s_core_cb.on_accept(s_core_opaque, c_fd, el);
}

void * ep_loop_proc(void * param)
{
	aeMain(s_event_loop);
	aeDeleteEventLoop(s_event_loop);
	s_event_loop = NULL;
	
	return NULL;
}

/*
 * start engine
 *
 * @return
 *     1 start ok
 *     0 error
 */
int service_core_start(int port, int keepalive, service_core_cb core_cb, void * opaque, char * err_string)
{
	int r;

	s_core_cb = core_cb;
	s_core_opaque = opaque;
    
	s_fd = anetTcpServer(err_string, port, NULL);
	if (s_fd == ANET_ERR)
	{
		return 0;
	}
	
	if (keepalive > 0)
	{
		anetKeepAlive(err_string, s_fd, keepalive);
        s_keepalive = keepalive;
	}
	
	if (s_event_loop == NULL)
	{
		s_event_loop = aeCreateEventLoop(10240);
		pthread_create(&s_loop_thread, NULL, ep_loop_proc, NULL);
	}
	
	r = aeCreateFileEvent(s_event_loop, s_fd, AE_READABLE, aeAcceptTcpHandler, NULL);
	if (r == AE_ERR)
	{
		if (err_string != NULL)
		{
			strcpy(err_string, "Unrecoverable error creating server.ipfd file event.");
		}
		
		return 0;
	}
	
	return 1;
}

int service_core_send(int fd, char * buffer, int size, void * extra)
{
	int r;
    
	r = (int)write(fd, buffer, size);
	if (r == -1)
	{
		aeClientClose((aeEventLoop *)extra, fd);
		return 0;
	}
	
    return size;
}

void service_core_stop()
{
	if (s_event_loop != NULL)
	{
		aeDeleteFileEvent(s_event_loop, s_fd, AE_ALL_EVENTS);
		close(s_fd);
	}
}

#endif