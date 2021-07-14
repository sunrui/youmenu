/*
 * service automate push
 *
 * @author rui.sun 2012-7-10 Tue 12:00
 */
#include "push.h"
#include "../../eventpack/include/eventpack.h"

#include <assert.h>
#include <stdio.h>
#include "identity.h"

/*
 * push a new event to queue, it will automate popped when max 
 *  [max_status_queue_time] is arrived
 *
 * @type	active event type
 *
 * @session	session_t
 *
 * @return
 *   >=1 event push ok
 *     0 fd is not valid
 */
int service_push_login_conflict(int fd)
{
	if (fd <= 0)
	{
		assert(0);
		return 0;
	}

#define REQ_MAKE_PARAM(param, id, pack) \
ep_req_param_t param; \
param.type = req_type_get; \
param.category = id; \
param.c_crypt = crypt_type_encrypt; \
param.c_compress = compress_type_auto; \
param.s_crypt = crypt_type_encrypt; \
param.s_compress = compress_type_auto; \
param.pack = pack;
    return 1;
//	return ep_service_push(fd, 1, &param);
}
