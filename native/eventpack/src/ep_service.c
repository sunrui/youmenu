/*
 * service
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2013-1-23
 */
#include "eventpack.h"
#include "ep_service.h"
#include "ep_buffer.h"
#include "ep_io.h"
#include "ep_dispatch.h"
#include "ep_cert.h"
#include "ep_socket.h"

#include "list.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef EP_HAVE_SERVER

static ep_io_mgr_t * s_io_mgr;
static ep_disp_mgr_t * s_disp_mgr;
static int s_started;
static ep_event_cb s_event_cb;
static void * s_event_opaque;

int ep_io_server_send(int fd, char * buffer, int size, void * extra)
{
	return service_core_send(fd, buffer, size, extra);
}

void ep_service_on_accept(void * opaque, int fd, void * extra)
{
	int ret;

	if (s_io_mgr == NULL)
	{
		return;
	}
	
	ret = ep_io_add(s_io_mgr, fd, extra);
	if (ret == 0)
	{
		return;
	}

	/* notify on fd accept */
	if (s_event_cb != NULL)
	{
		s_event_cb(s_event_opaque, event_fd_accept, fd);
	}
}

/*
 * @return
 *     1 if read ok
 *     0 if you need do shutdown this fd
 */
int ep_service_on_read(void * opaque, int fd, const char * buffer, int size)
{
	int ret;
	
	if (s_io_mgr == NULL)
	{
		return 0;
	}
	
	/*
	 * serialize received buffer to ep_buffer_t 
	 *
	 * @return
	 *     1 enqueue and send processor pool ok
	 *     0 received ok but buffer is not enough
 	 *    -1 got a bad request
	 *    -2 max capacity overflow
	 *    -3 socket not valid
	 */
	ret = ep_io_enqueue(s_io_mgr, fd, buffer, size);
	if (ret >= 0)
	{
		return 1;
	}

	ep_io_del(s_io_mgr, fd);

	if (s_event_cb != NULL)
	{
		ep_event_type type;

		switch (ret)
		{
		case -1:
			{
				type = event_io_exception;
				break;
			}
		case -2:
			{
				type = event_io_overflow;
				break;
			}
		case -3:
		default:
			{
				type = event_fd_close;
				break;
			}
		}

		s_event_cb(s_event_opaque, type, fd);
	}


	return 0;
}

void ep_service_on_close(void * opaque, int fd)
{
	if (s_io_mgr != NULL)
	{
		int ret;
		
		ret = ep_io_del(s_io_mgr, fd);
		if (ret == 0)
		{
			return;
		}
	}
	
	if (s_event_cb != NULL)
	{
		s_event_cb(s_event_opaque, event_fd_close, fd);
	}
}

void ep_service_send_error_cb(void * opaque, int fd)
{
	ep_service_on_close(NULL, fd);
}

/*
 * return
 *    1 success
 *    0 failed
 */
int ep_service_init(int nprocessors, int io_def_capacity, int io_max_capacity, int mini_compress_if)
{
	s_disp_mgr = ep_disp_init();
	if (s_disp_mgr == NULL)
	{
		assert(0);
		return 0;
	}
    
	if (nprocessors <= 0) nprocessors = 1;
	
	{
		ep_io_conf_t conf;
		
		conf.refor = ep_io_refor_server;
		conf.disp_mgr = s_disp_mgr;
		conf.send = ep_io_server_send;
		conf.nprocessors = nprocessors;
		conf.def_capacity = io_def_capacity;
		conf.max_capacity = io_max_capacity;
		conf.mini_compress_if = mini_compress_if;
		conf.send_error_cb = ep_service_send_error_cb;
		conf.send_err_cb_opaque = NULL;

		s_io_mgr = ep_io_init(&conf);
	}
	
	if (s_io_mgr == NULL)
	{
		ep_disp_destroy(&s_disp_mgr);
		assert(0);
		return 0;
	}

    /* server receive aes crypt key/iv from client using safety RSA */
	ep_disp_add_cb(s_disp_mgr, EP_CATEGORY_INTERNAL_KEY, ep_cert_server_security, s_io_mgr);
    
	return 1;
}

/*
 * start engine
 *
 * @return
 *    1 start ok
 *    0 start error
 */
int ep_service_start(ep_service_conf_t * conf, char * err_string)
{
	service_core_cb core_cb;
	int ret;

	if (s_started)
	{
		return 1;
	}
	
	ep_crypt_safe_init();
	ret = ep_service_init(conf->nprocessors, conf->io_def_capacity, conf->io_max_capacity, conf->mini_compress_if);
	if (ret != 1)
	{
		if (err_string != NULL)
		{
			strcpy(err_string, "disp_mgr or io_mgr init failed.");
		}
		
		return 0;
	}

	core_cb.on_accept = ep_service_on_accept;
	core_cb.on_close = ep_service_on_close;
	core_cb.on_read = ep_service_on_read;

	ret = service_core_start(conf->listenport, conf->keepalive, core_cb, NULL, err_string);
	if (ret != 1)
	{
		ep_disp_destroy(&s_disp_mgr);
		ep_io_destroy(&s_io_mgr);
        
		return 0;
	}

	s_started = 1;

	return 1;
}

/*
 * register fd event
 */
void ep_service_event(ep_event_cb event_cb, void * opaque)
{
	s_event_cb = event_cb;
	s_event_opaque = opaque;
}

/*
 * register a dispatch callback
 *
 * @return
 *     1 ok
 *     0 category id is conflict
 */
int ep_service_register(ep_categroy_t category, pfn_ep_cb ep_cb, void * opaque)
{
	return ep_disp_add_cb(s_disp_mgr, category, ep_cb, opaque);
}

/*
 * push a message to client
 *
 * @fd	client fd
 *
 * @notify_type
 *     0 notify all (ignore fd)
 *     1 notify just fd
 *     2 notify except fd
 *
 * @return
 *   >=1 if notify ok
 *     0 failed
 */
int ep_service_push(int fd, int notify_type, ep_req_param_t * param)
{
	ep_buffer_t * buffer;
	ep_shell_t shell;
	int ret;

	memset(&shell, 0, sizeof(ep_shell_t));
	shell.magic = EP_MAGIC_NUMBER;
	shell.proto.category = param->category;
	shell.proto.c_crypt = param->c_crypt;
	shell.proto.c_compress = param->c_compress;
	shell.proto.s_compress = param->s_compress;
	shell.proto.s_crypt = param->s_crypt;
	/* param->type; */
	shell.proto.type = req_type_post;

	{
		ep_buffer_t * tmp_buffer;

		tmp_buffer = NULL;
		if (param->pack != NULL)
		{
			tmp_buffer = ep_buffer_new2(param->pack);
			if (tmp_buffer == NULL)
			{
				return 0;
			}
		}

		buffer = ep_io_combine_shell(s_io_mgr, fd, &shell, tmp_buffer);
		ep_buffer_free(&tmp_buffer);
		if (buffer == NULL)
		{
			return 0;
		}
	}

	ret = ep_io_nodify_fd(s_io_mgr, fd, notify_type, buffer);

	return ret;
}

/*
 * stop engine
 */
void ep_service_stop()
{
	if (s_started)
	{
		service_core_stop();

		ep_disp_destroy(&s_disp_mgr);
		ep_io_destroy(&s_io_mgr);

		ep_crypt_safe_uninit();
		s_started = 0;
	}
}

#endif
