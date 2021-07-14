/*
 * request used for client2server only
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2012-8-14
 */
#ifndef EP_REQUEST_H
#define EP_REQUEST_H

typedef struct ep_req ep_req_t;

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
ep_req_t * ep_req_new(ep_client_t * client, ep_req_param_t * param, int * result);

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
int ep_req_wait(ep_req_t * handle, ep_pack_t ** pack, int timeout);

/*
 * destroy current request and cancel
 */
void ep_req_destroy(ep_req_t ** handle);

#endif
