/*
 * request used for client2server only
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2012-8-14
 */
#include "eventpack.h"
#include "ep_client_structer.h"
#include "ep_buffer.h"

#include "list.h"
#include "queue.h"

#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct ep_req
{
	ep_client_t * client;
	ep_categroy_t category;
	ep_req_type req;
    ep_queue_t * pack_queue;
};

ep_pack_t * ep_req_default_dispatcher(void * opaque, int fd, ep_pack_t * pack, int * b_pack_reclaim)
{
	ep_req_t * handle;

	handle = (ep_req_t *)opaque;
	assert(fd != 0);

	*b_pack_reclaim = 0;

	/* we make sure category is remove before we wait timeout 
	    but service processed this after time */
	ep_queue_push(handle->pack_queue, pack);
	
	return NULL;
}

/*
 * create a request
 *
 * @result
 *     0 no error
 *    -1 socket error
 *    -2 dispather id not free-ed in last request
 *
 * @return
 *     new request handle
 *     return NULL if failed
 */
ep_req_t * ep_req_new(ep_client_t * client, ep_req_param_t * param, int * result)
{
	ep_req_t * handle;
    int pack_size;
    
	if (client == NULL || client->fd == -1)
	{
		if (result != NULL)
		{
			*result = -1;
		}

		return NULL;
	}
    
	if (param->pack != NULL)
	{
    	ep_pack_finish(param->pack);
    	pack_size = ep_pack_size(param->pack);
	}
	else
	{
		pack_size = 0;
	}
	
	if (pack_size > client->conf.io_max_capacity)
	{
		if (result != NULL)
		{
			*result = -1;
		}

		if (client->disc_cb != NULL)
		{
			client->disc_cb(client->disc_opaque, ep_disc_capacity_overflow);
		}
        
        assert(0);
		return NULL;
	}

	{
		ep_buffer_t * shellbuf;
		ep_shell_t shell;
		int ret;
		
		handle = (ep_req_t *)ep_calloc(1, sizeof(ep_req_t));
		if (handle == NULL)
		{
			if (result != NULL)
			{
				*result = -1;
			}
            
            assert(0);
			return NULL;
		}

		memset(&shell, 0, sizeof(shell));
		shell.magic = EP_MAGIC_NUMBER;
		shell.proto.category = param->category;
		shell.proto.c_crypt = param->c_crypt;
		shell.proto.c_compress = param->c_compress;
		shell.proto.s_crypt = param->s_crypt;
		shell.proto.s_compress = param->s_compress;
		shell.proto.type = param->type;

        handle->pack_queue = ep_queue_new();
		handle->category = param->category;
		handle->client = client;

		ret = ep_disp_add_cb(client->disp_mgr, handle->category, ep_req_default_dispatcher, handle);
        if (ret == 0)
        {
            if (result != NULL)
			{
				*result = -2;
			}
            
            ep_queue_destroy(&handle->pack_queue);
            ep_free(handle);
            return NULL;
        }
        
		/* notify to io fd */
		{
			ep_buffer_t * buffer;

			if (param->pack != NULL)
			{
				buffer = ep_buffer_new2(param->pack);	
			}
			else
			{
				buffer = NULL;
			}
			
			if (param->pack != NULL && buffer == NULL)
			{
                ep_queue_destroy(&handle->pack_queue);
				ep_free(handle);

				if (result != NULL)
				{
					*result = -1;
				}
                
                assert(0);
				return NULL;
			}

			shellbuf = ep_io_combine_shell(client->io_mgr, client->fd, &shell, buffer);
			ep_buffer_free(&buffer);
			if (shellbuf == NULL)
			{
                ep_queue_destroy(&handle->pack_queue);
				ep_free(handle);
				
				if (result != NULL)
				{
					*result = -1;
				}
                
                assert(0);
				return NULL;
			}
			
			/* post event message which contoured by itself */
			ret = ep_io_nodify_fd(client->io_mgr, client->fd, 1, shellbuf);
			if (ret == 0)
			{
				ep_queue_destroy(&handle->pack_queue);
				ep_free(handle);
				
				if (result != NULL)
				{
					*result = -1;
				}
				
				return NULL;
			}
		}
	}
	
	if (result != NULL)
	{
		*result = 0;
	}

	return handle;
}

/*
 * wait for response returned, operation will be returned 
 *  when time reached or response returned
 *
 * @timeout wait timeout in seconds or <0 infinite
 *
 * @return
 *     2 request is a post message
 *     1 response ok
 *     0 time reached
 */
int ep_req_wait(ep_req_t * handle, ep_pack_t ** pack, int timeout)
{
    int ret;
    
	assert(handle != NULL);
	if (handle->req == req_type_post)
	{
		return 2;
	}
   
	*pack = NULL;
    ret = ep_queue_get(handle->pack_queue, (void **)pack, timeout);
	if (ret == 1 && pack != NULL)
	{
		return 1;
	}

	return 0;
}

/*
 * destroy current request and cancel
 */
void ep_req_destroy(ep_req_t ** handle)
{
	if (handle == NULL || *handle == NULL)
	{
		assert(0);
		return;
	}

	if ((*handle)->client->fd != -1 && (*handle)->client->disp_mgr != NULL)
	{
		ep_disp_del((*handle)->client->disp_mgr, (*handle)->category);
	}
	
    {
        ep_pack_t * pack;
        int ret;
        
        ret = ep_queue_get((*handle)->pack_queue, (void **)&pack, 0);
        if (ret == 1)
        {
            assert(pack != NULL);
            ep_pack_destroy(&pack);
        }
        
        ep_queue_destroy(&(*handle)->pack_queue);
    }
  
	ep_freep(*handle);
}
