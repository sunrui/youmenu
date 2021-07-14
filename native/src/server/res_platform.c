/*
 * client request version processor
 *
 * update 2012-6-17 0:01 rui.sun
 */
#include "../../eventpack/include/eventpack.h"

#include "identity.h"
#include "session.h"

#include <sys/time.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 * check newest version from server
 *
 * @request param
 *
 * - pid				platform id
 * - cur_ver_id			current version id
 *
 * @response param
 *
 * - result				common result definition
 * - new_ver_id			new version id
 * - new_ver_string		new version string
 * - url				download url
 * - update_logs		update logs
 */
ep_pack_t * res_version_check(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	int cur_ver_id = -1;
	int pid = -1;

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

			if (strcmp(key, "pid") == 0)
			{
				pid = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "cur_ver_id") == 0)
			{
				cur_ver_id = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
		}
	}

	pack = ep_pack_new();

	/* illegal request, record to sys log? */
	if (cur_ver_id == -1 || !valid_pid(pid))
	{
		ep_pack_append_int(pack, "result", version_illegal_parameter);
		assert(0);
		return pack;
	}

	ep_pack_append_int(pack, "result", verison_update_no_newest);
	ep_pack_append_int(pack, "new_ver_id", 1);
	ep_pack_append_string(pack, "new_ver_string", "no need update current.");
	ep_pack_append_string(pack, "url", "www.honeysense.com");
	ep_pack_append_string(pack, "update_logs", "www.honeysense.com");

	return pack;
}

ep_pack_t * res_heartbeat(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	session_t * session;
	int r;

    pack = ep_pack_new();
	r = session_ref_fd(fd, &session);
	if (r == 0)
	{
        ep_pack_append_int(pack, "result", 0);
        return pack;
	}

    gettimeofday(&session->time_heatbeat, NULL);
	session_dref(session);
    
    ep_pack_append_int(pack, "result", 1);
	
	return pack;
}