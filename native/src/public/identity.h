/*
 * public identify declare
 * update 2012-6-15 rui.sun
 */
#ifndef IDENTIFY_H
#define IDENTIFY_H

/* declare identity begin {{ */

#define ep_id_version_check			0
#define ep_id_heartbeat				1

#define ep_id_push_login_conflict	4

#define ep_id_account				6
#define ep_id_account_login			7
#define ep_id_account_logout		8
#define ep_id_account_create		9
#define ep_id_account_modify_pwd	10
#define ep_id_account_modify_basic  11
#define ep_id_account_delete		12
#define ep_id_account_list			13

#define ep_id_dish_fetch            14
#define ep_id_dish_book             153
#define ep_id_dish_cancel           15
#define ep_id_dish_list             178

#define ep_id_seat_list				19

#define ep_id_cook_accept			21
#define ep_id_cood_ready			22

/* }} declare identity end */

#define REQ_DEF_TIMEOUT 2 /* seconds */

/* default request param */
#define REQ_MAKE_PARAM(param, id, pack) \
ep_req_param_t param; \
param.type = req_type_get; \
param.category = id; \
param.c_crypt = crypt_type_encrypt; \
param.c_compress = compress_type_auto; \
param.s_crypt = crypt_type_encrypt; \
param.s_compress = compress_type_auto; \
param.pack = pack;

#endif