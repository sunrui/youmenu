/*
 * event pack security communicate support
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2013-10-27
 */
#ifndef EP_CERT_H
#define EP_CERT_H

#define EP_CATEGORY_INTERNAL_KEY		(ep_categroy_t)(-1)

/*
 * client encrypt rsa with pub_key
 *
 * @return
 *     a new ep_buffer_t
 */
ep_buffer_t * ep_cert_client_encrypt(const char * buffer, int size);

#ifdef EP_HAVE_SERVER

/*
 * server decrypt rsa with pri_key
 *
 * @return
 *     a new ep_buffer_t
 */
ep_buffer_t * ep_cert_server_decrypt(const char * buffer, int size);

#endif

/*
 * client send aes crypt key/iv to server using security RSA
 *
 * @request
 *
 * - security say "hello"
 *   value is an rsa encrypted pack
 *       - key  client aes key
 *       - vec client aes vec
 *
 * @response
 *
 * - result
 *    1 sign ok
 *    0 server not response
 *   -1 server set client security failed
 */
int ep_cert_client_security(ep_client_t * client, int wait_timeout);

#ifdef EP_HAVE_SERVER

/*
 * server receive aes crypt key/iv from client using safety RSA
 *
 * @request
 *
 * - security say "hello"
 *   value is an rsa encrypted pack
 *       - key  client aes key
 *       - vec client aes vec
 *
 * @response
 *
 * - result
 *    1 sign ok
 *   -1 server set client security failed
 */
ep_pack_t * ep_cert_server_security(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

#endif

#endif