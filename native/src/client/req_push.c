/*
 * media event callback
 *
 * @author rui.sun 2012-7-28 Sta 20:24 Olympic first days
 */
#include "client.h"
#include "connect.h"
#include "timer.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
//#include "log4c_extend.h"
//
//ep_pack_t * req_push_login_dispatcher(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
//{
//#if 0
//#if defined(_DEBUG) || defined(DEBUG)
//	{
//		int count;
//		count = ep_parser_count(req);
//
//		{
//			int index;
//			for (index = 0; index < count; index++)
//			{
//				char * key, * value;
//				int r;
//
//				r = ep_parser_index(req, index, &key, &value, NULL);
//				assert(r == 1);
//			}
//		}
//	}
//#endif
//#endif
//	return NULL;
//}
//
//ep_pack_t * req_push_logout_dispatcher(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
//{
//#if 0
//#if defined(_DEBUG) || defined(DEBUG)
//	{
//		int count;
//		count = ep_parser_count(req);
//		{
//			int index;
//			for (index = 0; index < count; index++)
//			{
//				char * key, * value;
//				int r;
//
//				r = ep_parser_index(req, index, &key, &value, NULL);
//				assert(r == 1);
//			}
//		}
//	}
//#endif
//#endif
//	return NULL;
//}
//
//ep_pack_t * req_push_discon_dispatcher(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
//{
//#if 0
//#if defined(_DEBUG) || defined(DEBUG)
//	{
//		int count;
//		count = ep_parser_count(req);
//		{
//			int index;
//			for (index = 0; index < count; index++)
//			{
//				char * key, * value;
//				int r;
//
//				r = ep_parser_index(req, index, &key, &value, NULL);
//				assert(r == 1);
//			}
//		}
//	}
//#endif
//#endif
//	return NULL;
//}
//
//ep_buffer_t * req_push_update_dispatcher(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
//{
//#if 0
//#if defined(_DEBUG) || defined(DEBUG)
//	{
//		int count;
//		count = ep_parser_count(req);
//		{
//			int index;
//			for (index = 0; index < count; index++)
//			{
//				char * key, * value;
//				int r;
//
//				r = ep_parser_index(req, index, &key, &value, NULL);
//				assert(r == 1);
//			}
//		}
//	}
//#endif
//#endif
//	return NULL;
//}

ep_pack_t * req_push_login_conflict(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
//	ep_pack_print(pack);

//	LOGI("my account is logged from anothe area.");

	return NULL;
}
//
//
///*
// * register server event callback
// */
//void result_register_cb(const session_t * session, result_event_cb cb, void * userdata)
//{
//	session->ev_cb = cb;
//	session->ev_userdata = userdata;
//}