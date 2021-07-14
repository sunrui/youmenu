/*
 * event pack security communicate support
 *
 * Copyright (C) 2012-2014 honeysense.com
 * @author rui.sun 2014-3-7
 */
#include "eventpack.h"

#include <time.h>
#include <string.h>
#include <assert.h>

void ep_cert_random(unsigned char buffer[16])
{
	char * curtime;
	time_t tm;
    
	time(&tm);
	curtime = ctime(&tm);
	ep_crypt_md5(buffer, curtime, (int)strlen(curtime));
}

#define EP_MAKEWORD(major, minor) (((major) << 8) | (minor))

/*
 * key = '\0' + '1 byte' + '13 random byte' + '1 byte'
 * value = md5(EP_MAKEWORD('1 byte', '1 byte') + '\0' + 13 random byte)
 */
void ep_cert_gen_vec(unsigned char key[16], unsigned char vec[16])
{
    unsigned char vec_buf[16];
    unsigned short magic;

    {
        unsigned char magic0, magic1;
        
        magic0 = key[1];
        magic1 = key[15];
        
        magic = EP_MAKEWORD(magic0, magic1);
    }

    memcpy(vec_buf, &magic, sizeof(magic));
    memcpy(vec_buf + sizeof(magic), &key[0], sizeof(key[0]));
    memcpy(vec_buf + sizeof(magic) + sizeof(key[0]), &key[2], 13);
    ep_crypt_md5(vec, (const char *)vec_buf, sizeof(vec_buf));
}

void ep_cert_gen_client_key(unsigned char key[16], unsigned char vec[16])
{
    ep_cert_random(key);
    key[0] = '\0';
	ep_cert_gen_vec(key, vec);
}

#ifdef EP_HAVE_SERVER

int ep_cert_verify_client_key(unsigned char key[16], unsigned char vec[16])
{
    unsigned char vec_buf[16];
    
    if (key[0] != '\0')
    {
        assert(0);
        return 0;
    }
    
    ep_cert_gen_vec(key, vec_buf);
    if (memcmp(vec_buf, vec, 16) != 0)
    {
        assert(0);
        return 0;
    }
    
    return 1;
}

#endif