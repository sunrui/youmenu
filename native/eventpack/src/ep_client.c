/*
 * client interface
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2012-8-13
 */
#include "eventpack.h"
#include "ep_client_structer.h"
#include "ep_cert.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

int ep_io_client_send(int fd, char * buffer, int size, void * extra)
{
	size_t send_size;
	
	send_size = send(fd, buffer, size, 0);
    ep_free(buffer);

	return (int)send_size;
}

#define MAX_LINE 65535
void * ep_client_routine(void * param)
{
	ep_client_t * client;
	char line[MAX_LINE];
	int n;

	ep_client_disc type;
	int ret;

	type = ep_disc_socket_close;
	ret = 0;
	
	client = (ep_client_t *)param;
	client->recv_loop = 1;
	while (client->recv_loop)
	{
		n = (int)recv(client->fd, line, sizeof(line), 0);
		if (n > 0)
		{
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
			ret = ep_io_enqueue(client->io_mgr, client->fd, line, n);
			if (ret < 0)
			{
                assert(0);
				switch (ret)
				{
				case -1:
					{
						type = ep_disc_bad_request;
						break;
					}
				case -2:
					{
						type = ep_disc_capacity_overflow;
						break;
					}
				}

				break;
			}
		}
		else
		{
			break;
		}
	}
	
	ep_io_del(client->io_mgr, client->fd);
	ep_sock_close(client->fd);
	client->fd = -1;
    
	if (client->disc_cb != NULL)
	{
		if (ret >= 0 && !client->recv_loop)
		{
			type = ep_disc_user_abort;
		}

		client->disc_cb(client->disc_opaque, type);
	}

	return NULL;
}

void ep_client_send_error_cb(void * opaque, int server_fd)
{
	ep_client_t * client = (ep_client_t *)opaque;
	
	ep_io_del(client->io_mgr, client->fd);
	ep_sock_close(client->fd);
	client->fd = -1;
	
	if (client->disc_cb != NULL)
	{
		client->disc_cb(client->disc_opaque, ep_disc_socket_close);
	}
}

/*
 * start client core
 *
 * @return
 *    1 start ok
 *    0 socket error
 *   -1 server security handshake failed
 *   -2 server not response
 *   -3 init module failed
 */
int ep_client_start(ep_client_conf_t * conf, ep_client_t ** client, int wait_timeout)
{
	struct sockaddr_in svr_addr;
	int fd;
	int ret;
    
    ep_crypt_safe_init();
    
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
	{
        *client = NULL;
		return 0;
	}

	*client = (ep_client_t *)ep_calloc(1, sizeof(ep_client_t));
	if (*client == NULL)
	{
		return -3;
	}

	(*client)->conf = *conf;
	(*client)->fd = fd;
	
	if ((*client)->conf.io_max_capacity < (*client)->conf.io_def_capacity)
	{
		(*client)->conf.io_max_capacity = (*client)->conf.io_def_capacity;
	}

	svr_addr.sin_family = AF_INET;
#ifdef _WIN32
	svr_addr.sin_addr.S_un.S_addr = inet_addr(conf->ip);
#else
	svr_addr.sin_addr.s_addr = inet_addr(conf->ip);
#endif
	svr_addr.sin_port = htons(conf->port);

	ret = connect((*client)->fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
	if (ret != 0)
	{
		ep_freep(*client);
		return -2;
	}

	(*client)->disp_mgr = ep_disp_init();
	if ((*client)->disp_mgr == NULL)
	{
		ep_client_stop(client);
		ep_freep(*client);
		return -3;
	}

	{
		ep_io_conf_t io_conf;
		
		io_conf.refor = ep_io_refor_client;
		io_conf.disp_mgr = (*client)->disp_mgr;
		io_conf.send = ep_io_client_send;
		io_conf.nprocessors = 1;
		io_conf.def_capacity = conf->io_def_capacity;
		io_conf.max_capacity = conf->io_max_capacity;
		io_conf.mini_compress_if = conf->mini_compress_if;
		io_conf.send_error_cb = ep_client_send_error_cb;
		io_conf.send_err_cb_opaque = *client;
		
		(*client)->io_mgr = ep_io_init(&io_conf);
	}
	
	if ((*client)->io_mgr == NULL)
	{
		ep_client_stop(client);
		ep_freep(*client);
		return -3;
	}

	ret = ep_io_add((*client)->io_mgr, (*client)->fd, *client);
	if (ret != 1)
	{
		ep_client_stop(client);
		ep_freep(*client);
		return -3;
	}

	if (conf->keepalive > 0)
	{
		ep_sock_keepalive((*client)->fd, 1, conf->keepalive);
	}
    
	pthread_create(&(*client)->recv_thrd, NULL, ep_client_routine, (*client));
    while ((*client)->recv_loop != 1) usleep(100 * 1000);

	ret = ep_cert_client_security(*client, wait_timeout);
	if (ret != 1)
	{
		ep_client_stop(client);
		ep_freep(*client);
        ret = -1;
	}

	return ret;
}

/*
 * notify when fd disconnect
 */
void ep_client_event(ep_client_t * client, pfn_ep_disc_cb disc_cb, void * opaque)
{
	client->disc_cb = disc_cb;
	client->disc_opaque = opaque;
}

/*
 * register a dispatch callback
 *
 * @return
 *     1 ok
 *     0 category id is conflict
 */
int ep_client_register(ep_client_t * client, ep_categroy_t category, pfn_ep_cb ep_cb, void * opaque)
{
	return ep_disp_add_cb(client->disp_mgr, category, ep_cb, opaque);
}

/*
 * stop client core
 */
void ep_client_stop(ep_client_t ** client)
{
	if (client != NULL && *client != NULL)
	{
		(*client)->recv_loop = 0;
		assert((*client)->fd != 0);
		ep_sock_close((*client)->fd);
        if ((*client)->io_mgr != NULL)
        {
            ep_io_stop((*client)->io_mgr, (*client)->fd);
            ep_io_del((*client)->io_mgr, (*client)->fd);
            ep_io_destroy(&(*client)->io_mgr);
        }
        
		pthread_join((*client)->recv_thrd, NULL);
		ep_disp_destroy(&(*client)->disp_mgr);
        ep_crypt_safe_uninit();
		ep_freep(*client);
	}
}
