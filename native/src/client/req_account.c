/*
 * media module api create processor
 *
 * @author rui.sun 2012-7-4 Wed 23:58
 */
#include "client.h"
#include "connect.h"
#include "identity.h"

#include "timer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/*
 * login and get session instance
 *
 * @expire
 *    0  valid in current session [default]
 *    1  valid in 1 day (24 hours)
 *   30  valid in 30 days
 *
 * @result[out]
 *     account_r_ok
 *     account_r_login_already_login
 *     account_r_login_ok_conlict_another_address
 *     account_r_login_user_not_exist
 *     account_r_login_invalid_user_or_pwd
 *     account_r_login_session_expired
 *     account_r_illegal_parameter
 *     account_r_service_unavailable
 *     account_r_no_response
 */
void client_account_login(connect_t * connect, const char * uid, const char * pwd, int token_expire, client_session_t * session)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
	
	if (connect == NULL || !valid_uid(uid) || !valid_pwd(pwd)|| token_expire < 0)
	{
		session->result = account_r_illegal_parameter;
		return;
	}
	
	if (connect->session != NULL)
	{
		session->result = account_r_login_already_login;
		return;
	}
	
	memset(session, 0, sizeof(*session));
	pack = ep_pack_new();
	ep_pack_append_string(pack, "uid", uid);
	ep_pack_append_string(pack, "pwd", pwd);
	ep_pack_append_int(pack, "token_expire", token_expire);

	{
		REQ_MAKE_PARAM(param, ep_id_account_login, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			session->result = account_r_no_response;
			return;
		}
	}

	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		session->result = account_r_no_response;
		return;
	}

	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        
		session->result = account_r_illegal_parameter;
		iter = ep_pack_iterator_get(pack);
		
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "result") == 0)
			{
				session->result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "type") == 0)
			{
				session->account.type = (account_type)ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
            else if (strcmp(key, "nick") == 0)
            {
				strncpy(session->account.nick, ep_pack_iterator_string(iter), sizeof(session->account.nick));
				assert(type == PACKET_STRING);
            }
            else if (strcmp(key, "comment") == 0)
            {
				strncpy(session->account.comment, ep_pack_iterator_string(iter), sizeof(session->account.comment));
				assert(type == PACKET_STRING);
            }
			else if (strcmp(key, "token") == 0)
			{
				strncpy(session->token, ep_pack_iterator_string(iter), sizeof(session->token));
				assert(type == PACKET_STRING);
			}
		}
		
		strcpy(session->account.uid, uid);
        session->account.pwd[0] = '\0';
		
		if (session->result == account_r_ok || session->result == account_r_login_ok_conlict_another_address)
		{
			// TODO: begin heartbeat
			connect->session = (client_session_t *)ep_malloc(sizeof(client_session_t));
			memcpy(connect->session, session, sizeof(*session));
		}

		ep_req_destroy(&handle);
	}
}

/*
 * login and get session instance
 *
 * @expire
 *    0  valid in current session [default]
 *    1  valid in 1 day (24 hours)
 *   30  valid in 30 days
 *
 * @result[out]
 *     account_r_ok
 *     account_r_login_already_login
 *     account_r_login_ok_conlict_another_address
 *     account_r_login_user_not_exist
 *     account_r_login_invalid_user_or_pwd
 *     account_r_login_session_expired
 *     account_r_illegal_parameter
 *     account_r_service_unavailable
 *     account_r_no_response
 */
void client_account_login2(connect_t * connect, const char token[33], client_session_t * session)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || !valid_token(token))
	{
		session->result = account_r_illegal_parameter;
		return;
	}
	
	if (connect->session != NULL)
	{
		session->result = account_r_login_already_login;
		return;
	}
    
	memset(session, 0, sizeof(*session));
	pack = ep_pack_new();
	ep_pack_append_string(pack, "token", token);
	
	{
		REQ_MAKE_PARAM(param, ep_id_account_login, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			session->result = account_r_no_response;
			return;
		}
	}

	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		session->result = account_r_no_response;
		return;
	}

	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		session->result = account_r_illegal_parameter;
		iter = ep_pack_iterator_get(pack);
		
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				session->result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "type") == 0)
			{
				session->account.type = (account_type)ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "uid") == 0)
			{
				strncpy(session->account.uid, ep_pack_iterator_string(iter), sizeof(session->account.uid));
				assert(type == PACKET_STRING);
			}
            else if (strcmp(key, "nick") == 0)
            {
				strncpy(session->account.nick, ep_pack_iterator_string(iter), sizeof(session->account.nick));
				assert(type == PACKET_STRING);
            }
            else if (strcmp(key, "comment") == 0)
            {
				strncpy(session->account.comment, ep_pack_iterator_string(iter), sizeof(session->account.comment));
				assert(type == PACKET_STRING);
            }
		}
		
		strcpy(session->token, token);
        session->account.pwd[0] = '\0';
		
		if (session->result == account_r_ok || session->result == account_r_login_ok_conlict_another_address)
		{
			// TODO: begin heartbeat
			connect->session = (client_session_t *)ep_malloc(sizeof(client_session_t));
			memcpy(connect->session, session, sizeof(*session));
		}
		
		ep_req_destroy(&handle);
	}
}

/*
 * get current session
 *
 * return 1 if success
 */
int client_account_get_session(connect_t * connect, client_session_t * session)
{
    if (connect == NULL || connect->session == NULL)
    {
        return 0;
    }

	if (session != NULL)
	{
		*session = *connect->session;
    }
	
	return 1;
}

/*
 * logout from server
 *
 * @result[out]
 *     account_r_ok
 *     account_r_session_expired
 *     account_r_no_response
 */
void client_account_logout(connect_t * connect, account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || connect->sock == NULL)
	{
		*result = account_r_ok;
		return;
	}

	{
//        timer_del(connect->timer, connect->heartbeat);
//        timer_destroy(&connect->timer);
        ep_freep(connect->session);
	}

	{
        pack = NULL;
		REQ_MAKE_PARAM(param, ep_id_account_logout, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}

	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		*result = account_r_no_response;
		return;
	}

	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}

		ep_req_destroy(&handle);
	}
}

/*
 * create user
 *
 * @result[out]
 *     account_r_ok
 *     account_r_no_permission
 *     account_r_create_uid_exist
 *     account_r_illegal_parameter
 *     account_r_service_unavailable
 *     account_r_no_response
 */
void client_account_create(connect_t * connect, account_t * user, account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || user == NULL || connect->session == NULL ||
		!valid_uid(user->uid) || !valid_pwd(user->pwd) ||
		!valid_nick(user->nick) || !valid_comment(user->comment))
	{
		*result = account_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin)
    {                                                                                               
		*result = account_r_no_permission;
		return;
    }

	pack = ep_pack_new();
	ep_pack_append_string(pack, "uid", user->uid);
	ep_pack_append_string(pack, "pwd", user->pwd);
	ep_pack_append_int(pack, "type", user->type);

	if (user->nick[0] != '\0')
	{
		ep_pack_append_string(pack, "nick", user->nick);
	}

	if (user->comment[0] != '\0')
	{
		ep_pack_append_string(pack, "comment", user->comment);
	}
	
	{
		REQ_MAKE_PARAM(param, ep_id_account_create, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}
	
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		*result = account_r_no_response;
		return;
	}

	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
}

/*
 * modify pwd
 *
 * @return
 *      account_r_ok,
 *      account_r_old_pwd_incorrect
 *      account_r_modify_pwd_same
 *      account_r_session_expired
 *      account_r_illegal_parameter
 *      account_r_service_unavailable
 *      account_r_no_response
 */
void client_account_modify_pwd(connect_t * connect, const char * old_pwd, const char * new_pwd, account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || connect->session == NULL || !valid_uid(old_pwd) || !valid_pwd(new_pwd))
	{
		*result = account_r_illegal_parameter;
		return;
	}
 
    if (strcmp(old_pwd, new_pwd) == 0)
    {
		*result = account_r_modify_pwd_same;
		return;
    }

    pack = ep_pack_new();
	ep_pack_append_string(pack, "old_pwd", old_pwd);
	ep_pack_append_string(pack, "new_pwd", new_pwd);
    
	{
		REQ_MAKE_PARAM(param, ep_id_account_modify_pwd, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}
	
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		*result = account_r_no_response;
		return;
	}
    
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
}

/*
 * modify basic
 *
 * @return
 *      account_r_ok,
 *      account_r_session_expired
 *      account_r_illegal_parameter
 *      account_r_service_unavailable
 *      account_r_no_response
 */
void client_account_modify_basic(connect_t * connect, const char nick[64 + 1], const char comment[256 + 1], account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	if (connect == NULL || connect->session == NULL || !valid_nick(nick) || !valid_comment(comment))
	{
		*result = account_r_illegal_parameter;
		return;
	}
    
    pack = ep_pack_new();
	ep_pack_append_string(pack, "nick", nick);
	ep_pack_append_string(pack, "comment", comment);
    
	{
		REQ_MAKE_PARAM(param, ep_id_account_modify_basic, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}
	
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		*result = account_r_no_response;
		return;
	}
    
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
}

/*
 * delete user
 *
 * @return
 *     account_r_ok
 *     account_r_cannot_delete_last_admin,
 *     account_r_no_permission
 *     account_r_illegal_parameter
 *     account_r_service_unavailable
 *     account_r_no_response
 */
void client_account_delete(connect_t * connect, const char uid[16 + 1], const char * old_pwd, account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || connect->session == NULL || !valid_pwd(old_pwd) || !valid_uid(uid))
	{
		*result = account_r_illegal_parameter;
		return;
	}

    if (connect->session->account.type != account_admin)
    {
        *result = account_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_string(pack, "uid", uid);
    ep_pack_append_string(pack, "old_pwd", old_pwd);
    
	{
		REQ_MAKE_PARAM(param, ep_id_account_delete, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}
	
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
		*result = account_r_no_response;
		return;
	}
    
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
}

/*
 * account list
 *
 * @return
 *     account_r_ok
 *     account_r_no_permission
 *     account_r_illegal_parameter
 *     account_r_service_unavailable
 *     account_r_no_response
 */
void client_account_list(connect_t * connect, account_t ** account, int * count, account_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	*account = NULL;
	*count = 0;
	
	if (connect == NULL || connect->session == NULL)
	{
		*result = account_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin)
    {
        *result = account_r_no_permission;
        return;
    }
    
	{
        pack = NULL;
		REQ_MAKE_PARAM(param, ep_id_account_list, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = account_r_no_response;
			return;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = account_r_no_response;
        return;
	}
    
    {
        ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        account_t * _account;
        int i;
        
        iter = ep_pack_iterator_get(pack);
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = account_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "result") != 0)
        {
            ep_req_destroy(&handle);
            *result = account_r_service_unavailable;
            return;
        }
        
        *result = (account_result)ep_pack_iterator_int(iter);
        if (*result != dish_r_ok)
        {
            ep_req_destroy(&handle);
            return;
        }
        
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = account_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "count") != 0)
        {
            ep_req_destroy(&handle);
            *result = account_r_service_unavailable;
            return;
        }
        
        *count = ep_pack_iterator_int(iter);
        if (*count == 0)
        {
            ep_req_destroy(&handle);
            *result = account_r_ok;
            return;
        }
        
        _account = (account_t *)ep_calloc(1, sizeof(account_t) * (*count));
        i = 0;
        
        while (ep_pack_iterator_next(&iter))
        {
            type = ep_pack_iterator_type(iter);
            key = ep_pack_iterator_key(iter);
            
            if (type == PACKET_ARRAY)
            {
                ep_pack_iterator_t object_iter;
                
                object_iter = ep_pack_interator_array(iter);
                while (ep_pack_iterator_next(&object_iter))
                {
                    type = ep_pack_iterator_type(object_iter);
                    key = ep_pack_iterator_key(object_iter);
                    
                    if (type == PACKET_OBJECT)
                    {
                        ep_pack_iterator_t one_iter;
                        
                        one_iter = ep_pack_interator_array(object_iter);

                        while (ep_pack_iterator_next(&one_iter))
                        {
                            type = ep_pack_iterator_type(one_iter);
                            key = ep_pack_iterator_key(one_iter);
                            
                            if (strcmp(key, "type") == 0)
                            {
                                _account[i].type = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "uid") == 0)
                            {
                                strncpy(_account[i].uid, ep_pack_iterator_string(one_iter), sizeof(_account[i].uid));
                                assert(type == PACKET_STRING);
                            }
                            else if (strcmp(key, "nick") == 0)
                            {
                                strncpy(_account[i].nick, ep_pack_iterator_string(one_iter), sizeof(_account[i].nick));
                                assert(type == PACKET_STRING);
                            }
                            else if (strcmp(key, "comment") == 0)
                            {
                                strncpy(_account[i].comment, ep_pack_iterator_string(one_iter), sizeof(_account[i].comment));
                                assert(type == PACKET_STRING);
                            }
                        }
                        
                        i++;
                    }
                    else
                    {
                        ep_free(_account);
                        *account = NULL;
                        
                        *result = account_r_service_unavailable;
                        ep_req_destroy(&handle);
                        assert(0);
                        return;
                    }
                }
                
                *account = _account;
            }
            else
            {
                ep_free(_account);
                *account = NULL;
                
                *result = account_r_service_unavailable;
                ep_req_destroy(&handle);
                assert(0);
                return;
            }
        }
    }
    
    ep_req_destroy(&handle);
    *result = account_r_ok;
}

void client_account_list_free(account_t * account)
{
	if (account != NULL)
	{
		ep_free(account);
	}
}