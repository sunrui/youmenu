/*
 * session
 * rui.sun 2012_12_21 4:19
 */
#include "session.h"
#include "identity.h"

#include "timer.h"
#include "pref.h"
#include "guid.h"

#include "list.h"

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct session_list
{
	session_t session;
	pref_t * ref;
	struct list_head list;
} session_list_t;

struct list_head session_list; /* all sessions that registered to server */
int session_stop_sign; /* whether received a signal to stop */
st_timer_t * session_heartbeat_timer; /* automate sync last user heartbeat status timer */
pthread_mutex_t session_lock;

/*
 * session check all heartbeat if is 1 or user has been disconnect
 */
void session_check_all_hearbeat(int * b_stop)
{
	struct list_head * pos, * q;
//	session_list_t * tmp;
//	int b_need_delete;

	list_for_each_safe(pos, q, &session_list)
	{
		if (*b_stop) break;

//		tmp = list_entry(pos, session_list_t, list);
//		b_need_delete = !tmp->session.net.heartbeat || tmp->b_mark_deleted;
//
//		if (!tmp->session.net.heartbeat)
//		{
////			LOGW("[SESSION] detect '%s' disconnect.", tmp->session.basic.uid);
//		}
//
//		if (tmp->b_mark_deleted)
//		{
////			LOGI("[SESSION] logout [%d]'%s'-'%s'.", tmp->session.is_admin, tmp->session.basic.uid, tmp->session.ref_admin_uid);
//		}
//
//		if (b_need_delete)
//		{
//			list_del(pos);
//			pref_release(&tmp->ref);
//			free(tmp);
//			return;
//		}
//
//		tmp->session.net.heartbeat = 0;
		//{
		//	int ret_offline;

		//	/* set user session expired */
		//	ret_offline = session_offline_session(tmp->item.fd);

		//	/* if user session is not exist or user session is expired,
		//    just ignore any message about disconnection here */
		//	if (ret_offline == 0)
		//	{
		//		/* notify event_event_on_disconn */
		//		//event_event_on_disconn(tmp->item.fd);

		//		LOGW("heartbeat detect %s off line..", tmp->item.uid);
		//	}
		//}

		//tmp->item.is_heartbeat = 0;
	}
}

void session_beartbeat_sync_clock(void * userdata, int * b_abort_clock)
{
	/* check all session heartbeat */
	session_check_all_hearbeat(&session_stop_sign);
}

/*
 * session init
 */
void session_init(int expired_second)
{
	INIT_LIST_HEAD(&session_list);
	session_stop_sign = 0;
	pthread_mutex_init(&session_lock, NULL);
//	session_heartbeat_timer = timer_create(session_beartbeat_sync_clock, NULL, heartbeat_msec, 0);
}

/*
 * session clean
 */
void session_destroy()
{
	struct list_head * pos, * q;
	session_list_t * tmp;

	session_stop_sign = 1;
	pthread_mutex_lock(&session_lock);
	list_for_each_safe(pos, q, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);
		list_del(pos);
		pref_release(&tmp->ref);
		free(tmp);
	}
//	timer_del(session_heartbeat_timer);
	pthread_mutex_unlock(&session_lock);
	pthread_mutex_destroy(&session_lock);
}

/*
 * add a new session
 *
 * @return
 *     1 success
 *     0 failed due to uid/token is already exist
 */
int session_add(session_t * session)
{
	assert(session != NULL);
	pthread_mutex_lock(&session_lock);
    
	{
		struct list_head * pos, *q;
		session_list_t * tmp;
        
		list_for_each_safe(pos, q, &session_list)
		{
			tmp = list_entry(pos, session_list_t, list);
			
            /* case want to add session_fd */
            if (session->s_type == session_fd)
            {
                /* case fd already exist */
                if (tmp->session.fd == session->fd)
                {
                    pthread_mutex_unlock(&session_lock);
                    assert(0);
                    return 0;
                }
                
                if (tmp->session.s_type == session_fd)
                {
                    /* case uid already exist */
                    if (strcmp(tmp->session.uid, session->uid) == 0)
                    {
                        pthread_mutex_unlock(&session_lock);
                        assert(0);
                        return 0;
                    }
                }
            }
            /* case want to add token */
            else
            {
                if (tmp->session.s_type == session_token)
                {
                    /* case token already exist */
                    if (strcmp(tmp->session.token, session->token) == 0)
                    {
                        pthread_mutex_unlock(&session_lock);
                        assert(0);
                        return 0;
                    }
                }
            }
		}
	}
    
	{
		session_list_t * sess;
        
		sess = (session_list_t *)calloc(1, sizeof(session_list_t));
		memcpy(&sess->session, session, sizeof(session_t));
		pref_init(&sess->ref);
		list_add_tail(&sess->list, &session_list);
		pthread_mutex_unlock(&session_lock);
	}
    
	return 1;
}

/*
 * add a new session
 *
 * @return
 *     1 success
 *     0 failed due to uid/token is already exist
 */
int session_add_fd(session_t * session)
{
    session->s_type = session_fd;
    session->token[0] = '\0';
    
    return session_add(session);
}

/*
 * add a new session
 *
 * @return
 *     1 success
 *     0 failed due to uid/token is already exist
 */
int session_add_token(session_t * session)
{
    session->s_type = session_token;
    session->fd = -1;
    assert(session->token[0] != '\0');
    
    return session_add(session);
}

/*
 * delete an exist session
 */
int session_del_fd(int fd)
{
	struct list_head * pos, *q;
	session_list_t * tmp;
	int r;

    assert(fd != -1);
	r = 0;
    
	pthread_mutex_lock(&session_lock);
	list_for_each_safe(pos, q, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);

		if (tmp->session.fd == fd)
		{
            assert(tmp->session.s_type == session_fd);
            
			list_del(pos);
			pref_release(&tmp->ref);
			free(tmp);
			r++;
		}
	}
	pthread_mutex_unlock(&session_lock);
	
	return !!r;
}

/*
 * delete an exist session
 */
int session_del_token(const char * uid)
{
	struct list_head * pos, *q;
	session_list_t * tmp;
	int r;
    
	r = 0;
    
	pthread_mutex_lock(&session_lock);
	list_for_each_safe(pos, q, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);
		if (tmp->session.s_type == session_token &&
            strcmp(tmp->session.uid, uid) == 0)
		{
			list_del(pos);
			pref_release(&tmp->ref);
			free(tmp);
			r++;
		}
	}
	pthread_mutex_unlock(&session_lock);
	
	return !!r;
}

/*
 * get specified session status
 * NOTE: you must manual call session_dref when no more used.
 *
 * @return
 *     1 success
 *     0 not exist
 */
int session_ref_fd(int fd, session_t **  session)
{
	struct list_head * pos;
	session_list_t * tmp;
	int r;
    
    assert(fd != -1);
	*session = NULL;
	r = 0;

	list_for_each(pos, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);
		if (tmp->session.fd == fd)
		{
			pref_get(tmp->ref);
			*session = &tmp->session;
			r++;
		}
	}

	if (r >= 1) pthread_mutex_lock(&session_lock);
    assert(r == 0 || r == 1);

	return !!r;
}

int session_ref_uid(const char * uid, session_type type, session_t ** session)
{
	struct list_head * pos;
	session_list_t * tmp;
	int r;
	
	*session = NULL;
	r = 0;
	
	list_for_each(pos, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);
        if (tmp->session.s_type == type &&
            strcmp(tmp->session.uid, uid) == 0)
		{
			pref_get(tmp->ref);
			*session = &tmp->session;
			r = 1;
			break;
		}
	}
	
	if (r == 1) pthread_mutex_lock(&session_lock);
	
	return r;
}

/*
 * get specified session status
 * NOTE: you must manual call session_dref when no more used.
 *
 * @return
 *     1 success
 *     0 not exist
 */
int session_ref_token(const char * token, session_t ** session)
{
	struct list_head * pos;
	session_list_t * tmp;
	int r;

	*session = NULL;
	r = 0;

	list_for_each(pos, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);
		if (tmp->session.s_type == session_token &&
            strcmp(tmp->session.token, token) == 0)
		{
			if (session != NULL)
			{
				pthread_mutex_lock(&session_lock);
				pref_get(tmp->ref);
				*session = &tmp->session;
			}

			r = 1;
			break;
		}
	}

	return r;
}

void session_dref(session_t * session)
{
	struct list_head * pos;
	session_list_t * tmp;
	int r;

	assert(session != NULL);

	r = 0;
	list_for_each(pos, &session_list)
	{
		tmp = list_entry(pos, session_list_t, list);

		if (&tmp->session == session)
		{
			pref_put(tmp->ref);
			r = 1;
			break;
		}
	}

	pthread_mutex_unlock(&session_lock);
	assert(r);
}
