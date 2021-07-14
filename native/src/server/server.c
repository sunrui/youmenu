//
//  server.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-10.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#include "server.h"

#ifdef EP_HAVE_SERVER

#include "db.h"
#include "session.h"

#include "../../eventpack/include/eventpack.h"
#include "../../log/include/log.h"
#include "../utils/timer.h"

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

/* platform category */
ep_pack_t * res_version_check(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_heartbeat(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

/* account category */
ep_pack_t * res_account_login(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_logout(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_create(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_modify_pwd(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_modify_basic(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_delete(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_account_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

/* dish category */
ep_pack_t * res_dish_fetch(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_dish_book(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_dish_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);
ep_pack_t * res_dish_cancel(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

/* seat category */
ep_pack_t * res_seat_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

/* cook category */
ep_pack_t * res_cook_accept(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim);

int server_load_conf();
int server_init_log();
int server_init_db();
int server_init_network(int listen_port);

int server_unload_conf();
int server_deinit_log();
int server_deinit_db();
int server_deinit_network();

#define VERIFY_OK(x) { \
	int _r = (x); \
	if (!_r) { \
		LOGE(#x " error"); \
		return 0; \
	} \
}

int server_startup(int listen_port)
{
	VERIFY_OK(server_load_conf());
	VERIFY_OK(server_init_log());
	VERIFY_OK(server_init_db());
	VERIFY_OK(server_init_network(listen_port));
	
	return 1;
}

int server_stop()
{
	VERIFY_OK(server_deinit_network());
	VERIFY_OK(server_deinit_db());
	VERIFY_OK(server_unload_conf());
	VERIFY_OK(server_deinit_log());
	
	return 1;
}

int server_load_conf()
{
	ep_alloctor_t alloctor;
	alloctor.malloc = malloc;
	alloctor.free = free;
	alloctor.realloc = realloc;
	alloctor.calloc = calloc;
	
	ep_alloctor_custom(&alloctor);
	
	return 1;
}

int server_init_log()
{
//	char dir[256];
//	
//	getcwd(dir, 256);
//	strcat(dir, "/log");
//	mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//	strcat(dir, "/youmenu.log");
//	log_init(dir);
	
	// profile/log/
	
	VERIFY_OK(log_init("youmenu.log"));
	log_set_level(log_level_debug);
	
	return 1;
}

int server_init_db()
{
	dbp_conf_t conf;
    int r = 0;

	strcpy(conf.db, DB_DATABASE);
    {
        r = db_init(&conf);
        VERIFY_OK(r == 1);
    }
	
	/* makesure reserved account 'youmenu' exist */
	{
        r = db_account_init(&conf);
        VERIFY_OK(r == 1);
		
        r = db_account_get_one("youmenu", NULL);
		if (r != 1)
		{
			account_t a;
			
			a.type = account_admin;
			strcpy(a.uid, "youmenu");
			strcpy(a.pwd, "youmenu");
			strcpy(a.nick, "youmenu");
			strcpy(a.comment,
				   "This is the youmenu default account, "
				   "please change the password as soon as possible.");

            r = db_account_add(&a);
            VERIFY_OK(r == 1);
		}
	}
    
    {
        r = db_dish_init(&conf);
        VERIFY_OK(r == 1);
        
        //test
        {
            dish_t * all;
            dish_t dish;
            int count;
            
            if (db_dish_get_by_category(dish_category_hot, NULL) == 0)
            {
                dish.category = dish_category_hot;
                dish.id = 0;
                strcpy(dish.name, "dish_name");
				dish.price_type = dish_price_plate;
                dish.price = 30;
                strcpy(dish.summary, "summary");
                strcpy(dish.thumbnail, "thumbnail");
            
                r = db_dish_add(&dish);
                VERIFY_OK(r == 1);
            }
            
            count = db_dish_get_by_category(dish_category_hot, &all);
            db_dish_free(all);
        }
    }
    
    {
        r = db_seat_init(&conf);
        VERIFY_OK(r == 1);
        
        // just a test
        {
            seat_t * all;
            seat_t seat;
            int count;
            
            if (!db_seat_get_one(0, NULL))
            {
                seat.id = 0;
                strcpy(seat.summary, "summary");
                strcpy(seat.qrcode, "qrcode");
                
                r = db_seat_add(&seat);
                VERIFY_OK(r == 1);
            }
            
            count = db_seat_get(&all);
            db_seat_free(all);
        }
    }
    
    {
        r = db_book_init(&conf);
        VERIFY_OK(r == 1);
    }
    
	return 1;
}

void service_event_cb(void * opaque, ep_event_type type, int fd)
{
    switch (type)
    {
        case event_fd_close:
        case event_io_overflow:
        case event_io_exception:
            session_del_fd(fd);
            break;
        default:
            break;
    }
}

int server_init_network(int listen_port)
{
	ep_service_conf_t conf;
	char err_string[256];
	int r;
	
	conf.listenport = listen_port;
	conf.keepalive = 60;
	conf.nprocessors = 10;
	conf.io_def_capacity = 4 * 1024;
	conf.io_max_capacity = 4 * 1024 * 1024;
	conf.mini_compress_if = 1024;

	r = ep_service_start(&conf, err_string);
	if (r != 1)
	{
		LOGE("service start error, %s.", err_string);
		return 0;
	}
    
    session_init(300);
    ep_service_event(service_event_cb, NULL);

	/* register all request handler */
	{
		/* platform category */
		ep_service_register(ep_id_version_check, res_version_check, NULL);
		ep_service_register(ep_id_heartbeat, res_heartbeat, NULL);

		/* account category */
		ep_service_register(ep_id_account_login, res_account_login, NULL);
		ep_service_register(ep_id_account_logout, res_account_logout, NULL);
		ep_service_register(ep_id_account_create, res_account_create, NULL);
		ep_service_register(ep_id_account_modify_pwd, res_account_modify_pwd, NULL);
		ep_service_register(ep_id_account_modify_basic, res_account_modify_basic, NULL);
		ep_service_register(ep_id_account_delete, res_account_delete, NULL);
		ep_service_register(ep_id_account_list, res_account_list, NULL);

        /* dish category */
        ep_service_register(ep_id_dish_fetch, res_dish_fetch, NULL);
        ep_service_register(ep_id_dish_book, res_dish_book, NULL);
        ep_service_register(ep_id_dish_cancel, res_dish_cancel, NULL);
        ep_service_register(ep_id_dish_list, res_dish_list, NULL);
		
		/* seat category */
        ep_service_register(ep_id_seat_list, res_seat_list, NULL);
		
		/* cook category */
        ep_service_register(ep_id_cook_accept, res_cook_accept, NULL);
	}
	
	return 1;
}

int server_unload_conf()
{
	return 1;
}

int server_deinit_log()
{
	log_fini();
	return 1;
}

int server_deinit_db()
{
	db_account_deinit();
    db_dish_deinit();
    db_seat_deinit();
    db_book_deinit();
    db_deinit();
    
	return 1;
}

int server_deinit_network()
{
	ep_service_stop();
	session_destroy();
	
	return 1;
}

#endif