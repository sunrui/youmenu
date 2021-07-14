/*
 * public structer declare
 * update 2012-6-15 rui.sun
 */
#ifndef PUBLIC_H
#define PUBLIC_H

typedef enum platfrom_id
{
	platfrom_android_phone,
	platfrom_android_pad,
	platfrom_ios_phone,
	platfrom_ios_pad,
	platfrom_pc_win,
	platfrom_mac_osx,
	platform_other
} platfrom_id;

typedef enum account_type
{
	account_menu = 1,
	account_cook = 2,
	account_cashier = 3,
	account_admin = 0
} account_type;

typedef struct account
{
	account_type type;

	char uid[16 + 1];
	char pwd[128 + 1];

	char nick[64 + 1];
	char comment[256 + 1];
} account_t;

/* check valid account begin {{ */

#define valid_pid(pid)				(pid >= platfrom_android_phone && pid <= platform_other)
#define valid_type(type)            (type >= 0 && type <= 3)
#define valid_admin(admin)			(admin == 0 || admin == 1)
#define valid_uid(uid)				(uid != NULL && uid[0] != '\0' && strlen(uid) <= 16)
#define valid_pwd(pwd)				(pwd != NULL && pwd[0] != '\0' && strlen(pwd) <= 16)
#define valid_pwd(pwd)				(pwd != NULL && pwd[0] != '\0' && strlen(pwd) <= 16)
#define valid_token(token)			(token != NULL && token[0] != '\0' && strlen(token) == 38)
#define valid_nick(nick)			(nick == NULL || (nick != NULL && strlen(nick) <= 64))
#define valid_comment(comment)		(comment == NULL || (comment != NULL && strlen(comment) <= 256))

/* }} check valid account end */

typedef enum connect_result
{
	connect_r_ok,
	connect_r_socket_error,
	connect_r_security_handshake_error,
	connect_r_module_init_error,
	connect_r_no_response
} connect_result;

typedef struct version_sign
{
	int ver_id;
	char string[32];
} version_sign_t;

typedef enum verison_update_type
{
	verison_update_no_newest,
	verison_update_needed,
	verison_update_needed_force,
	version_illegal_parameter,
} verison_update_type;

typedef struct version_status
{
	version_sign_t new_ver;
	verison_update_type update_type;
	
	char url[256];
	char update_logs[4096];
} version_status_t;

typedef enum account_result
{
	account_r_ok = 0,
	account_r_login_already_login,
	account_r_login_ok_conlict_another_address,
	account_r_login_user_not_exist,
	account_r_login_invalid_user_or_pwd,
	account_r_create_uid_exist,
    account_r_old_pwd_incorrect,
    account_r_modify_pwd_same,
    account_r_cannot_delete_last_admin,
	account_r_no_permission,
	account_r_session_expired,
	account_r_illegal_parameter,
	account_r_service_unavailable,
	account_r_no_response
} account_result;

typedef enum dish_result
{
    dish_r_ok,
    dish_r_modify_no_exist,
    dish_r_modify_cook_accept,
    dish_r_seat_id_invalid,
    dish_r_dish_id_invalid,
	dish_r_no_permission,
    dish_r_illegal_parameter,
    dish_r_service_unavailable,
    dish_r_no_response
} dish_result;

typedef enum dish_category
{
    dish_category_hot,
    dish_category_cold,
    dish_category_wine,
    dish_category_drink,
    dish_category_food,
	dish_category_other
} dish_category;

typedef enum dish_price_type
{
	dish_price_plate = 0,
	dish_price_jin
} dish_price_type;

typedef struct dish
{
    int id;
    dish_category category;
    char name[64 + 1];
    char summary[256 + 1];
    char thumbnail[64 + 1];
	
	dish_price_type price_type;
    int price;
} dish_t;

typedef enum seat_result
{
	seat_r_ok,
	seat_r_no_permission,
    seat_r_illegal_parameter,
    seat_r_service_unavailable,
    seat_r_no_response
} seat_result;

typedef struct seat
{
    int id;
    char summary[256 + 1];
    char qrcode[64 + 1];
} seat_t;

typedef enum book_accept_state
{
	book_accept_state_idle,
	book_accept_state_cook_accept,
	book_accept_state_cook_finished
} book_accept_state;

typedef struct book
{
	int book_id; /* auto generate by server */

    int seat_id;
    int dish_id;
	double jin_or_plate;
	char comment[256];
	
	book_accept_state accept_state;
} book_t;

typedef enum cook_result
{
	cook_r_ok,
	cook_r_seat_id_invalid,
	cook_r_dish_id_invalid,
	cook_r_modify_no_more_accept,
	cook_r_modify_no_exist,
	cook_r_no_permission,
    cook_r_illegal_parameter,
    cook_r_service_unavailable,
    cook_r_no_response
} cook_result;

#endif