/*
 * media module api 
 *
 * @author rui.sun 2012-7-3 Tue 11:12
 */
#include "client.h"
#include "connect.h"
#include "identity.h"

#include "timer.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

ep_pack_t * req_push_login_conflict(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

/*
 * heartbeat and check the recently changed status
 *
 * @return
 *     1 ok
 *     0 session expired
 */
int client_heartbeat(connect_t * connect)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	if (connect == NULL || connect->sock == NULL)
	{
		return 0;
	}
    
	{
		pack = NULL;
		REQ_MAKE_PARAM(param, ep_id_heartbeat, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		if (handle == NULL)
		{
			return 0;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		return 0;
	}
    
	{
		ep_pack_iterator_t iter;
        int result = 0;
        
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			ep_pack_type type;
			const char * key;
            
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
            
			if (strcmp(key, "result") == 0)
			{
				result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
        
		ep_req_destroy(&handle);
        
        return result;
	}
}

/*
 * heartbeat timer
 */
void heartbeart_timer_clock(void * userdata, int * b_abort_clock)
{
    connect_t * connect;
    int r;
    
    connect = (connect_t *)userdata;
	r = client_heartbeat((connect_t *)userdata);
	if (r == 0)
    {
        connect->disc_cb(connect->disc_cb_opaque);
    }
}

/*
 * client disconnect callback
 */
void client_disconnect_cb(void * opaque, ep_client_disc type)
{
    connect_t * connect;
    connect = (connect_t *)opaque;
    
    connect->disc_cb(connect->disc_cb_opaque);
}

int client_startup(connect_t * connect)
{
    /* set keep-alive */
//    {
//        int heartbeat_fps =  30 * 1000; /* 30 seconds */
//        
//        connect->timer = timer_init(heartbeat_fps / 2);
//        connect->heartbeat = timer_create(connect->timer, heartbeart_timer_clock, connect, heartbeat_fps, 0);
//    }
   

	//ep_client_register(client, ep_makeword(ep_id_push, ep_id_push_login_conflict), req_push_login_conflict, NULL);

	return 0;
}

/*
 * connect to server
 */
connect_t * client_connect(const char * ip, int port, connect_result * result, pfn_client_disconnect_cb disc_cb, void * disc_cb_opaque)
{
	connect_t * connect;
	ep_client_t * client;
	ep_client_conf_t conf;
	int r;

    assert(result != NULL && disc_cb != NULL);
	connect = NULL;
	
	strcpy(conf.ip, ip);
	conf.port = port;
	conf.keepalive = 60;
	conf.io_def_capacity = 4 * 1024;
	conf.io_max_capacity = 4 * 1024 * 1024;
	
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
	r = ep_client_start(&conf, &client, REQ_DEF_TIMEOUT);
	switch (r)
	{
		case 1:
		{
			connect = (connect_t *)ep_calloc(1, sizeof(connect_t));
			connect->sock = client;
            connect->disc_cb = disc_cb;
            connect->disc_cb_opaque = disc_cb_opaque;
			ep_client_event(connect->sock, client_disconnect_cb, connect);
			client_startup(connect);
			
			*result = connect_r_ok;
			break;
		}
		case 0:
			*result = connect_r_socket_error;
			break;
		case -1:
			*result = connect_r_security_handshake_error;
			break;
		case -2:
			*result = connect_r_no_response;
			break;
		case -3:
		default:
			*result = connect_r_module_init_error;
			break;
	}

	return connect;
}

/*
 * disconnect from server
 */
void client_disconnect(connect_t ** connect)
{
	if (connect != NULL && *connect != NULL)
	{
		ep_client_stop((ep_client_t **)&(*connect)->sock);
		ep_freep(*connect);
	}
}