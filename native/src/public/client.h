/*
 * media module api 
 *
 * @author rui.sun 2012-7-3 Tue 11:12
 */
#ifndef CLIENT_H
#define CLIENT_H

#include "public.h"

#ifdef  __cplusplus
# define BEGIN_DECLS  extern "C" {
# define END_DECLS    }
#else
# define BEGIN_DECLS
# define END_DECLS
#endif

BEGIN_DECLS

typedef struct client_session_t
{
	account_result result;

	account_t account;
	char token[38 + 1];
} client_session_t;

typedef struct connect connect_t;

/*
 * client disconnect callback
 */
typedef void (* pfn_client_disconnect_cb)(void * opaque);

/*
 * connect to server
 */
connect_t * client_connect(const char * ip, int port, connect_result * result, pfn_client_disconnect_cb disc_cb, void * disc_cb_opaque);

/*
 * disconnect from server
 */
void client_disconnect(connect_t ** connect);

/*
 * check newest version from server
 *
 * @return
 *     1 check success
 *     0 no response
 */
int client_check_version(connect_t * connect, platfrom_id pid, int cur_vid, version_status_t * version);

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
void client_account_login(connect_t * connect, const char * uid, const char * pwd, int token_expire, client_session_t * session);
void client_account_login2(connect_t * connect, const char token[33], client_session_t * session);

/*
 * get current session
 *
 * return 1 if success
 */
int client_account_get_session(connect_t * connect, client_session_t * session);

/*
 * logout from server
 *
 * @result[out]
 *     account_r_ok
 *     account_r_session_expired
 *     account_r_no_response
 */
void client_account_logout(connect_t * connect, account_result * result);

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
void client_account_create(connect_t * connect, account_t * user, account_result * result);

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
void client_account_modify_pwd(connect_t * connect, const char * old_pwd, const char * new_pwd, account_result * result);

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
void client_account_modify_basic(connect_t * connect, const char nick[64 + 1], const char comment[256 + 1], account_result * result);

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
void client_account_delete(connect_t * connect, const char uid[16 + 1], const char * old_pwd, account_result * result);

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
void client_account_list(connect_t * connect, account_t ** account, int * count, account_result * result);
void client_account_list_free(account_t * account);

/*
 * fetch dish
 *
 * @return
 *     dish_r_ok
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
void client_dish_fetch(connect_t * connect, dish_category category, dish_t ** dish, int * count, dish_result * result);
void client_dish_fetch_free(dish_t ** dish, int count);

/*
 * dish book
 *
 * @return
 *     dish_r_ok
 *     dish_r_ignore_conflict
 *     dish_r_seat_id_invalid
 *     dish_r_dish_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
void client_dish_book(connect_t * connect, int seat_id, int dish_id, double jin_or_plate, const char * comment, dish_result * result);

/*
 * dish cancel
 *
 * @return
 *     dish_r_ok
 *     dish_r_seat_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
void client_dish_cancel(connect_t * connect, int seat_id, dish_result * result);

/*
 * dish list
 *
 * @return
 *     dish_r_ok
 *     dish_r_dish_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
void client_dish_list(connect_t * connect, int seat_id, book_t ** book, int * count, dish_result * result);
void client_dish_list_free(book_t * book);

/*
 * seat list
 *
 * @return
 *     seat_r_ok
 *     seat_r_no_permission
 *     seat_r_illegal_parameter
 *     seat_r_service_unavailable
 *     seat_r_no_response
 */
void client_seat_list(connect_t * connect, seat_t ** seat, int * count, seat_result * result);
void client_seat_list_free(seat_t * seat);

/*
 * cook accept
 *
 * @return
 *     cook_r_ok
 *     cook_r_seat_id_invalid
 *     cook_r_dish_id_invalid
 *     cook_r_modify_no_more_accept
 *     cook_r_modify_no_exist
 *     cook_r_no_permission
 *     cook_r_illegal_parameter
 *     cook_r_service_unavailable
 *     cook_r_no_response
 */
void client_cook_accept(connect_t * connect, int seat_id, int dish_id, int number, cook_result * result);

END_DECLS

#endif