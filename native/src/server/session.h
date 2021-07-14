/*
 * session
 * rui.sun 2012_12_21 4:19
 */
#ifndef SESSION_H
#define SESSION_H

#include "identity.h"
#include "public.h"

#include <time.h>

typedef enum session_type
{
	session_fd,
	session_token
} session_type;

typedef struct session
{
	account_type a_type;
	char uid[16 + 1];
	int admin;
	
	session_type s_type;
	
	/* session type token {{ */
	char token[38 + 1];
	struct timeval time_created;
	struct timeval time_expired;
	/* }} */
    
	/* session type token {{ */
	int fd;
	struct timeval time_heatbeat;
	/* }} */
} session_t;

/*
 * generate a random token
 */
void random_token(char token[33]);

/*
 * session init
 */
void session_init(int expired_second);

/*
 * session clean
 */
void session_destroy();

/*
 * add a new session
 *
 * @return
 *     1 success
 *     0 failed due to uid/token is already exist
 */
int session_add_fd(session_t * session);

/*
 * add a new session
 *
 * @return
 *     1 success
 *     0 failed due to uid/token is already exist
 */
int session_add_token(session_t * session);

/*
 * delete an exist session
 */
int session_del_fd(int fd);

/*
 * delete an exist session
 */
int session_del_token(const char * uid);

/*
 * get specified session status
 * NOTE: you must manual call session_dref when no more used.
 *
 * @return
 *     1 success
 *     0 not exist
 */
int session_ref_fd(int fd, session_t ** session);
int session_ref_uid(const char * uid, session_type type, session_t ** session);
int session_ref_token(const char * token, session_t ** session);
void session_dref(session_t * session);


#endif