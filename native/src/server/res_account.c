/*
 * client request version processor
 *
 * update 2012-6-17 0:01 rui.sun
 */
#include "../../eventpack/include/eventpack.h"

#include "db.h"
#include "session.h"

#include "../utils/guid.h"

#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * check whether session is already exist
 *
 * @return 1 if session conflict
 */
int res_account_check_session_exist(const char * uid, int fd)
{
	session_t * old;
	int conflict;
	int r;
	
	conflict = 0;
    
	/* check whehter uid is login-ed by another place */
	r = session_ref_uid(uid, session_fd, &old);
	if (r == 1)
	{
		int fd = old->fd;
		
		session_dref(old);
		r = session_del_fd(fd);
		assert(r == 1);
		
		// MARK: server push old session logout
		
		conflict = 1;
	}
    
	/* check whether same fd has logined more than 2 account */
	r = session_ref_fd(fd, &old);
	if (r == 1)
	{
		int fd = old->fd;
		
		session_dref(old);
		r = session_del_fd(fd);
		assert(r == 1);
		
		// MARK: server push old session logout
		
		conflict = 1;
	}
	
	return conflict;
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
 *     account_r_login_ok
 *     account_r_login_ok_conlict_another_address
 *     account_r_login_user_not_exist
 *     account_r_login_invalid_user_or_pwd
 *     account_r_login_session_expired
 *     account_r_illegal_parameter
 *     account_r_internal_error
 *     account_r_no_response
 */
ep_pack_t * res_account_login(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	char uid[16 + 1] = { 0 };
	char pwd[16 + 1] = { 0 };
	char token[38 + 1] = { 0 };
	int token_expire = 0;

    if (pack != NULL)
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "uid") == 0)
			{
				strncpy(uid, ep_pack_iterator_string(iter), sizeof(uid));
				
				/* word to lower */
				{
					char * p_uid = uid;
					
					do
					{
						*p_uid = tolower(*p_uid);
					} while (*p_uid++);
				}

				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "pwd") == 0)
			{
				strncpy(pwd, ep_pack_iterator_string(iter), sizeof(pwd));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "token") == 0)
			{
				strncpy(token, ep_pack_iterator_string(iter), sizeof(token));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "token_expire") == 0)
			{
				token_expire = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
		}
	}

	pack = ep_pack_new();

	/* login by user id and pwd */
	if (valid_uid(uid) && valid_pwd(pwd))
	{
		int conflict;
		int r;
		
		/* check whether user is exist */
		{
			r = db_account_get_one(uid, NULL);
			if (r != 1)
			{
				ep_pack_append_int(pack, "result", account_r_login_user_not_exist);
				return pack;
			}
		}
		
		/* check whether user pwd is correct */
		r = db_account_pwd_check(uid, pwd);
		if (r != 1)
		{
			ep_pack_append_int(pack, "result", account_r_login_invalid_user_or_pwd);
			return pack;
		}

		/* check whether session is already exist */
		conflict = res_account_check_session_exist(uid, fd);
		
		/* add to session inst */
		{
            account_t a;
			session_t s;
            account_type type;

            {
                r = db_account_get_one(uid, &a);
                assert(r == 1);
                if (r == 1)
                {
                    type = a.type;
                }
                /* current user is logined but we NOW delete it */
                else
                {
                    ep_pack_append_int(pack, "result", account_r_login_user_not_exist);
                    return pack;
                }
            }
            
			strcpy(s.uid, uid);
			s.a_type = (account_type)type;
			s.fd = fd;
			s.s_type = session_fd;
			s.token[0] = '\0';
			gettimeofday(&s.time_created, NULL);
			
			r = session_add_fd(&s);
			assert (r == 1);
			
			/* add if user want to generate a token session */
			if (token_expire > 0)
			{
				s.s_type = session_token;
				
				/* calculate expire date */
				{
					struct timeval append;
					
					append.tv_sec = token_expire * 24 * 60 * 60;
					append.tv_usec = 0;
					
					timeradd(&s.time_created, &append, &s.time_expired);
				}
				
				guid_generate(s.token);
				s.fd = -1;
                
                /* remove old token if already exists */
                {
                    session_t * old;
                    int conflict;
                    int r;
                    
                    conflict = 0;
                    
                    r = session_ref_uid(uid, session_token, &old);
                    if (r == 1)
                    {
                        char uid[16 + 1];
                        
                        strncpy(uid, old->uid, sizeof(uid));
                        session_dref(old);
                        r = session_del_token(uid);
                        assert(r == 1);
                        
                        // MARK: server push old session logout
                        
                        conflict = 1;
                    }
                }
                
				r = session_add_token(&s);
				assert(r == 1);
				
				ep_pack_append_string(pack, "token", s.token);
			}
			
			ep_pack_append_int(pack, "type", type);
            ep_pack_append_string(pack, "nick", a.nick);
            ep_pack_append_string(pack, "comment", a.comment);
            
            ep_pack_append_int(pack, "result", conflict ? account_r_login_ok_conlict_another_address : account_r_ok);
		}

		return pack;
	}
	
	/* login by user token */
	if (valid_token(token))
	{
		session_t * s;
		int r;

		r = session_ref_token(token, &s);
		if (r != 1)
		{
			ep_pack_append_int(pack, "result", account_r_session_expired);
		}
		else
		{
			session_t ns;
			int conflict;
			
			memcpy(&ns, s, sizeof(session_t));
			ns.s_type = session_fd;
			ns.fd = fd;
			ns.token[0] = '\0';
			session_dref(s);
			
			/* check whether session is already exist */
			conflict = res_account_check_session_exist(ns.uid, fd);
			
			r = session_add_fd(&ns);
			assert(r == 1);
			
			{
				account_t a;
				
				r = db_account_get_one(ns.uid, &a);
				assert(r == 1);
                /* current user is logined but we NOW delete it */
                if (r != 1)
                {
                    ep_pack_append_int(pack, "result", account_r_login_user_not_exist);
                    return pack;
                }
                
				ep_pack_append_int(pack, "type", a.type);
				ep_pack_append_string(pack, "uid", a.uid);
                ep_pack_append_string(pack, "nick", a.nick);
                ep_pack_append_string(pack, "comment", a.comment);
			}
			
			ep_pack_append_int(pack, "result", conflict ? account_r_login_ok_conlict_another_address : account_r_ok);
		}

		return pack;
	}

	ep_pack_append_int(pack, "result", account_r_illegal_parameter);

	return pack;
}

/*
 * logout from server
 *
 * @result[out]
 *     account_r_ok
 *     account_r_session_expired
 *     account_r_no_response
 */
ep_pack_t * res_account_logout(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	session_t * session;
	int r;

	pack = ep_pack_new();
	r = session_ref_fd(fd, &session);
	if (!r)
	{
		/* session expired */
		ep_pack_append_int(pack, "result", account_r_session_expired);
		return pack;
	}

	session_dref(session);
	r = session_del_fd(fd);
	assert(r == 1);

	/* logout ok */
	ep_pack_append_int(pack, "result", account_r_ok);
	return pack;
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
ep_pack_t * res_account_create(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	account_t a;
	int r;

    a.uid[0] = '\0';
    a.pwd[0] = '\0';
	a.nick[0] = '\0';
	a.comment[0] = '\0';
    
	if (pack != NULL)
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "type") == 0)
			{
				a.type = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "uid") == 0)
			{
				strncpy(a.uid, ep_pack_iterator_string(iter), sizeof(a.uid));
				
				/* word to lower */
				{
					char * p_uid = a.uid;
					
					do
					{
						*p_uid = tolower(*p_uid);
					} while (*p_uid++);
				}
				
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "pwd") == 0)
			{
				strncpy(a.pwd, ep_pack_iterator_string(iter), sizeof(a.pwd));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "nick") == 0)
			{
				strncpy(a.nick, ep_pack_iterator_string(iter), sizeof(a.nick));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "comment") == 0)
			{
				strncpy(a.comment, ep_pack_iterator_string(iter), sizeof(a.comment));
				assert(type == PACKET_STRING);
			}
		}
	}

	pack = ep_pack_new();
	
	/* illegal request, record to sys log? */
	if (!valid_uid(a.uid) || !valid_pwd(a.pwd) || !valid_type(a.type) ||
		!valid_nick(a.nick) || !valid_comment(a.comment))
	{
		ep_pack_append_int(pack, "result", account_r_illegal_parameter);
		return pack;
	}

	/* check current user have permisson */
	{
		session_t * s;
        int admin;
        
		r = session_ref_fd(fd, &s);
		if (r != 1)
		{
			ep_pack_append_int(pack, "result", account_r_no_permission);
			return pack;
		}
        
        admin = s->admin;
        session_dref(s);
        
		if (s->admin == 0)
		{
			ep_pack_append_int(pack, "result", account_r_no_permission);
			return pack;
		}
	}
	
	/* check whether user already exist */
	r = db_account_get_one(a.uid, NULL);
	if (r == 1)
	{
		ep_pack_append_int(pack, "result", account_r_create_uid_exist);
		return pack;
	}
	
	r = db_account_add(&a);
	assert(r == 1);
	if (r != 1)
	{
		ep_pack_append_int(pack, "result", account_r_service_unavailable);
		return pack;
	}
	
	ep_pack_append_int(pack, "result", account_r_ok);
	return pack;
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
ep_pack_t * res_account_modify_pwd(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    char old_pwd[16 + 1];
    char new_pwd[16 + 1];
	int r;
    
    old_pwd[0] = '\0';
    new_pwd[0] = '\0';
    
    if (pack != NULL)
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "old_pwd") == 0)
			{
				strncpy(old_pwd, ep_pack_iterator_string(iter), sizeof(old_pwd));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "new_pwd") == 0)
			{
				strncpy(new_pwd, ep_pack_iterator_string(iter), sizeof(new_pwd));
				assert(type == PACKET_STRING);
			}
		}
	}
    
    pack = ep_pack_new();
    
	/* illegal request, record to sys log */
	if (!valid_pwd(old_pwd) || !valid_pwd(new_pwd))
	{
		ep_pack_append_int(pack, "result", account_r_illegal_parameter);
		assert(0);
		return pack;
	}
    
    if (strcmp(old_pwd, new_pwd) == 0)
    {
		ep_pack_append_int(pack, "result", account_r_modify_pwd_same);
        return pack;
    }
    
	{
		session_t * session;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            ep_pack_append_int(pack, "result", account_r_session_expired);
			return pack;
		}
        
		r = db_account_pwd_check(old_pwd, old_pwd);
		if (r == 0)
		{
            session_dref(session);
            ep_pack_append_int(pack, "result", account_r_old_pwd_incorrect);
			return pack;
		}
        
        r = db_account_update_pwd(session->uid, new_pwd);
        session_dref(session);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", account_r_service_unavailable);
			return pack;
        }
        
        ep_pack_append_int(pack, "result", account_r_ok);
    }
	
	return pack;
}

/*
 * modify basic
 *
 * @return
 *      account_r_ok
 *      account_r_session_expired
 *      account_r_illegal_parameter
 *      account_r_service_unavailable
 *      account_r_no_response
 */
ep_pack_t * res_account_modify_basic(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    char nick[16 + 1];
    char comment[16 + 1];
	int r;
    
	nick[0] = '\0';
	comment[0] = '\0';
    
    if (pack != NULL)
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "nick") == 0)
			{
				strncpy(nick, ep_pack_iterator_string(iter), sizeof(nick));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "comment") == 0)
			{
				strncpy(comment, ep_pack_iterator_string(iter), sizeof(comment));
				assert(type == PACKET_STRING);
			}
		}
	}
    
    pack = ep_pack_new();
    
	{
		session_t * session;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            ep_pack_append_int(pack, "result", account_r_session_expired);
			return pack;
		}
        
        r = db_account_update_nick(session->uid, nick);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", account_r_service_unavailable);
			return pack;
        }
        
        r = db_account_update_comment(session->uid, comment);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", account_r_service_unavailable);
			return pack;
        }
        
        ep_pack_append_int(pack, "result", account_r_ok);
    }
	
	return pack;
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
ep_pack_t * res_account_delete(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    char old_pwd[16 + 1];
    char uid[16 + 1];
	int r;
    
    old_pwd[0] = '\0';
    uid[0] = '\0';
    
    if (pack != NULL)
    {
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
            if (strcmp(key, "old_pwd") == 0)
			{
				strncpy(old_pwd, ep_pack_iterator_string(iter), sizeof(old_pwd));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "uid") == 0)
			{
				strncpy(uid, ep_pack_iterator_string(iter), sizeof(uid));
				assert(type == PACKET_STRING);
			}
		}
	}

    pack = ep_pack_new();
    
	/* illegal request, record to sys log */
	if (!valid_uid(uid) || !valid_pwd(old_pwd))
	{
        ep_pack_append_int(pack, "result", account_r_illegal_parameter);
		return pack;
	}
 
	{
		session_t * session;

		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            ep_pack_append_int(pack, "result", account_r_session_expired);
            return pack;
		}
		
		r = db_account_pwd_check(session->uid, old_pwd);
		if (r == 0)
		{
			ep_pack_append_int(pack, "result", account_r_old_pwd_incorrect);
			return pack;
		}
		
        /* if user want delete its own account */
        if (strcmp(session->uid, uid) == 0)
        {
            if (session->a_type == account_admin)
            {
                account_t * account;
                int count;
                int c = 0;
                
                count = db_account_get(&account);
                if (count != 0)
                {
                    for (int i = 0; i < count; i++)
                    {
                        if (account[i].type == account_admin) c++;
                    }
                    db_account_get_free(account);
                }
                
                if (c <= 1)
                {
					session_dref(session);
                    ep_pack_append_int(pack, "result", account_r_cannot_delete_last_admin);
                    return pack;
                }
            }

            session_dref(session);
            session_del_fd(fd);
            session_del_token(uid);

            r = db_account_del(uid);
            assert(r == 1);
            
            ep_pack_append_int(pack, "result", r ? account_r_ok : account_r_service_unavailable);
            return pack;
        }
        /* admin user delete other account */
        else
        {
            if (session->admin == 0)
            {
                session_dref(session);
                ep_pack_append_int(pack, "result", account_r_no_permission);
                return pack;
            }
            
            session_dref(session);
            r = db_account_del(uid);
            assert(r == 1);
            
            ep_pack_append_int(pack, "result", r ? account_r_ok : account_r_service_unavailable);
            return pack;
        }
    }
	
	return NULL;
}

ep_pack_t * res_account_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    /* check permission */
    {
        session_t * session;
        int r;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
			return pack;
		}
        
        if (session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
            return pack;
        }
        session_dref(session);
    }

    pack = ep_pack_new();
    
    {
        account_t * _account;
        int count;
        
        count = db_account_get(&_account);
        if (count == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_ok);
            ep_pack_append_int(pack, "count", 0);
            return pack;
        }

        ep_pack_append_int(pack, "result", dish_r_ok);
        ep_pack_append_int(pack, "count", count);
        ep_pack_append_start_array(pack, "account");
        for (int i = 0; i < count; i++)
        {
            ep_pack_append_start_object(pack, "account_one");
            ep_pack_append_int(pack, "type", _account[i].type);
            ep_pack_append_string(pack, "uid", _account[i].uid);
            ep_pack_append_string(pack, "nick", _account[i].nick);
            ep_pack_append_string(pack, "comment", _account[i].comment);
            ep_pack_append_finish_object(pack);
        }
        ep_pack_append_finish_array(pack);
    }
    
    return pack;
}