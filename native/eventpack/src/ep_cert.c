/*
 * event pack security communicate support
 *
 * Copyright (C) 2012-2013 honeysense.com
 * @author rui.sun 2013-10-27
 */
#include "eventpack.h"
#include "ep_buffer.h"
#include "ep_cert.h"
#include "ep_client_structer.h"

#include <assert.h>
#include <string.h>

/*
 * implenment in ep_cert_verify.c
 *
 * key = '\0' + '1 byte' + '13 random byte' + '1 byte'
 * value = md5(EP_MAKEWORD('1 byte', '1 byte') + '\0' + 13 random byte)
 */
void ep_cert_gen_client_key(unsigned char key[16], unsigned char vec[16]);

#ifdef EP_HAVE_SERVER

int ep_cert_verify_client_key(unsigned char key[16], unsigned char vec[16]);

#endif

#ifdef EP_HAVE_SERVER

static const char RSA_PRI_KEY[] = {
	0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x42, 0x45, 0x47, 0x49, 0x4E, 0x20, 0x52, 0x53, 0x41, 0x20, 0x50, 
	0x52, 0x49, 0x56, 0x41, 0x54, 0x45, 0x20, 0x4B, 0x45, 0x59, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 
	0x4D, 0x49, 0x49, 0x43, 0x58, 0x51, 0x49, 0x42, 0x41, 0x41, 0x4B, 0x42, 0x67, 0x51, 0x44, 0x55, 
	0x54, 0x77, 0x49, 0x78, 0x69, 0x57, 0x4F, 0x38, 0x6F, 0x43, 0x53, 0x67, 0x2B, 0x64, 0x76, 0x51, 
	0x51, 0x72, 0x5A, 0x44, 0x61, 0x58, 0x39, 0x39, 0x58, 0x35, 0x72, 0x30, 0x6F, 0x43, 0x33, 0x68, 
	0x6A, 0x4E, 0x76, 0x6D, 0x76, 0x62, 0x4B, 0x4C, 0x79, 0x53, 0x52, 0x32, 0x44, 0x30, 0x58, 0x37, 
	0x0A, 0x64, 0x51, 0x54, 0x4E, 0x72, 0x53, 0x68, 0x64, 0x74, 0x62, 0x33, 0x77, 0x79, 0x6C, 0x33, 
	0x4B, 0x4A, 0x50, 0x55, 0x71, 0x62, 0x46, 0x6D, 0x71, 0x75, 0x6D, 0x2B, 0x32, 0x56, 0x54, 0x6B, 
	0x74, 0x4A, 0x6A, 0x34, 0x48, 0x44, 0x50, 0x48, 0x32, 0x74, 0x54, 0x32, 0x68, 0x63, 0x74, 0x71, 
	0x4B, 0x44, 0x6C, 0x68, 0x2B, 0x57, 0x43, 0x48, 0x6B, 0x36, 0x34, 0x76, 0x79, 0x48, 0x74, 0x70, 
	0x77, 0x0A, 0x75, 0x67, 0x41, 0x36, 0x6E, 0x46, 0x78, 0x77, 0x52, 0x59, 0x67, 0x71, 0x78, 0x65, 
	0x72, 0x71, 0x45, 0x67, 0x41, 0x68, 0x4B, 0x66, 0x42, 0x65, 0x58, 0x55, 0x38, 0x58, 0x66, 0x34, 
	0x75, 0x37, 0x32, 0x30, 0x70, 0x4B, 0x5A, 0x42, 0x44, 0x63, 0x61, 0x73, 0x37, 0x2B, 0x36, 0x49, 
	0x5A, 0x45, 0x51, 0x6B, 0x59, 0x43, 0x73, 0x7A, 0x71, 0x79, 0x34, 0x77, 0x49, 0x44, 0x41, 0x51, 
	0x41, 0x42, 0x0A, 0x41, 0x6F, 0x47, 0x42, 0x41, 0x4C, 0x33, 0x35, 0x73, 0x4E, 0x6E, 0x49, 0x75, 
	0x61, 0x46, 0x6F, 0x6E, 0x7A, 0x34, 0x59, 0x4E, 0x68, 0x52, 0x6D, 0x44, 0x5A, 0x66, 0x47, 0x79, 
	0x42, 0x6A, 0x55, 0x75, 0x75, 0x43, 0x59, 0x6B, 0x46, 0x57, 0x61, 0x61, 0x49, 0x39, 0x52, 0x51, 
	0x58, 0x6F, 0x4A, 0x30, 0x34, 0x46, 0x38, 0x36, 0x7A, 0x38, 0x32, 0x46, 0x38, 0x55, 0x2F, 0x59, 
	0x37, 0x5A, 0x75, 0x0A, 0x30, 0x50, 0x61, 0x73, 0x37, 0x6A, 0x33, 0x61, 0x30, 0x6A, 0x4A, 0x53, 
	0x33, 0x2B, 0x32, 0x41, 0x68, 0x45, 0x50, 0x67, 0x67, 0x4A, 0x67, 0x67, 0x39, 0x7A, 0x42, 0x4F, 
	0x33, 0x57, 0x34, 0x5A, 0x2B, 0x4D, 0x58, 0x4E, 0x56, 0x64, 0x33, 0x63, 0x38, 0x2B, 0x78, 0x35, 
	0x63, 0x7A, 0x70, 0x6D, 0x45, 0x51, 0x42, 0x47, 0x73, 0x75, 0x48, 0x36, 0x5A, 0x68, 0x49, 0x5A, 
	0x62, 0x44, 0x48, 0x61, 0x0A, 0x52, 0x6F, 0x71, 0x53, 0x61, 0x61, 0x6B, 0x58, 0x47, 0x38, 0x57, 
	0x71, 0x65, 0x7A, 0x6E, 0x49, 0x51, 0x67, 0x6E, 0x36, 0x65, 0x33, 0x54, 0x49, 0x4E, 0x76, 0x44, 
	0x6F, 0x65, 0x37, 0x34, 0x68, 0x50, 0x4B, 0x52, 0x74, 0x55, 0x63, 0x6A, 0x48, 0x64, 0x65, 0x30, 
	0x6E, 0x75, 0x74, 0x73, 0x70, 0x41, 0x6B, 0x45, 0x41, 0x2F, 0x2F, 0x72, 0x37, 0x4B, 0x48, 0x63, 
	0x44, 0x64, 0x75, 0x70, 0x33, 0x0A, 0x35, 0x30, 0x2B, 0x43, 0x55, 0x48, 0x66, 0x6A, 0x72, 0x70, 
	0x2F, 0x42, 0x39, 0x4D, 0x76, 0x4F, 0x6B, 0x36, 0x51, 0x6C, 0x2F, 0x48, 0x4F, 0x68, 0x4B, 0x75, 
	0x4E, 0x6A, 0x67, 0x4B, 0x36, 0x4C, 0x56, 0x33, 0x46, 0x63, 0x52, 0x30, 0x78, 0x38, 0x36, 0x2F, 
	0x56, 0x59, 0x6D, 0x70, 0x76, 0x70, 0x70, 0x37, 0x2B, 0x54, 0x4F, 0x46, 0x43, 0x48, 0x52, 0x4F, 
	0x6B, 0x48, 0x73, 0x77, 0x79, 0x70, 0x0A, 0x4C, 0x37, 0x6C, 0x65, 0x36, 0x4E, 0x50, 0x4B, 0x31, 
	0x77, 0x4A, 0x42, 0x41, 0x4E, 0x52, 0x54, 0x4B, 0x39, 0x56, 0x30, 0x74, 0x63, 0x30, 0x66, 0x37, 
	0x47, 0x78, 0x53, 0x32, 0x46, 0x76, 0x51, 0x45, 0x6C, 0x37, 0x78, 0x33, 0x4A, 0x58, 0x77, 0x51, 
	0x38, 0x58, 0x70, 0x44, 0x78, 0x51, 0x55, 0x67, 0x45, 0x57, 0x4D, 0x70, 0x2B, 0x65, 0x2F, 0x39, 
	0x44, 0x6F, 0x6D, 0x62, 0x73, 0x57, 0x36, 0x0A, 0x68, 0x64, 0x5A, 0x51, 0x63, 0x32, 0x49, 0x38, 
	0x49, 0x73, 0x56, 0x43, 0x6B, 0x57, 0x76, 0x6B, 0x77, 0x33, 0x64, 0x52, 0x66, 0x63, 0x46, 0x46, 
	0x68, 0x72, 0x42, 0x35, 0x4B, 0x4B, 0x47, 0x55, 0x77, 0x74, 0x55, 0x43, 0x51, 0x48, 0x61, 0x63, 
	0x4E, 0x67, 0x70, 0x75, 0x38, 0x78, 0x55, 0x44, 0x32, 0x65, 0x45, 0x39, 0x62, 0x7A, 0x57, 0x59, 
	0x42, 0x39, 0x44, 0x66, 0x52, 0x38, 0x45, 0x46, 0x0A, 0x4F, 0x73, 0x67, 0x67, 0x6C, 0x46, 0x56, 
	0x67, 0x77, 0x72, 0x6F, 0x62, 0x75, 0x50, 0x78, 0x6B, 0x5A, 0x44, 0x35, 0x31, 0x55, 0x58, 0x76, 
	0x63, 0x44, 0x6A, 0x70, 0x4C, 0x61, 0x65, 0x33, 0x68, 0x39, 0x71, 0x64, 0x36, 0x31, 0x6C, 0x32, 
	0x4F, 0x75, 0x73, 0x38, 0x4D, 0x5A, 0x7A, 0x76, 0x6F, 0x7A, 0x76, 0x42, 0x46, 0x4A, 0x5A, 0x77, 
	0x65, 0x6E, 0x75, 0x73, 0x43, 0x51, 0x45, 0x4F, 0x67, 0x0A, 0x67, 0x68, 0x4F, 0x6F, 0x36, 0x73, 
	0x62, 0x38, 0x35, 0x67, 0x62, 0x53, 0x6C, 0x45, 0x73, 0x61, 0x43, 0x4E, 0x2F, 0x6F, 0x31, 0x55, 
	0x4F, 0x45, 0x6C, 0x58, 0x47, 0x52, 0x2B, 0x39, 0x56, 0x2F, 0x65, 0x69, 0x4F, 0x73, 0x32, 0x30, 
	0x58, 0x37, 0x59, 0x38, 0x53, 0x70, 0x76, 0x52, 0x71, 0x4A, 0x67, 0x46, 0x74, 0x30, 0x6F, 0x47, 
	0x75, 0x6E, 0x6A, 0x38, 0x4F, 0x59, 0x6F, 0x6B, 0x7A, 0x6C, 0x0A, 0x50, 0x6B, 0x54, 0x67, 0x31, 
	0x64, 0x63, 0x62, 0x56, 0x2F, 0x30, 0x5A, 0x65, 0x68, 0x47, 0x4E, 0x47, 0x6B, 0x6B, 0x43, 0x51, 
	0x51, 0x44, 0x64, 0x33, 0x65, 0x43, 0x58, 0x6D, 0x59, 0x65, 0x58, 0x51, 0x59, 0x42, 0x35, 0x53, 
	0x52, 0x31, 0x39, 0x54, 0x50, 0x65, 0x37, 0x36, 0x6E, 0x67, 0x4F, 0x76, 0x62, 0x77, 0x42, 0x78, 
	0x76, 0x6C, 0x62, 0x49, 0x6A, 0x77, 0x49, 0x32, 0x63, 0x66, 0x64, 0x0A, 0x65, 0x47, 0x6A, 0x65, 
	0x57, 0x6A, 0x32, 0x67, 0x67, 0x55, 0x42, 0x75, 0x66, 0x4B, 0x79, 0x6C, 0x32, 0x70, 0x67, 0x4D, 
	0x39, 0x7A, 0x57, 0x43, 0x4E, 0x79, 0x46, 0x36, 0x4F, 0x50, 0x30, 0x6B, 0x38, 0x31, 0x37, 0x66, 
	0x79, 0x78, 0x77, 0x37, 0x50, 0x73, 0x2F, 0x44, 0x0A, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x45, 0x4E, 
	0x44, 0x20, 0x52, 0x53, 0x41, 0x20, 0x50, 0x52, 0x49, 0x56, 0x41, 0x54, 0x45, 0x20, 0x4B, 0x45, 
	0x59, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D
};

#endif

static const char RSA_PUB_KEY[] = {
	0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x42, 0x45, 0x47, 0x49, 0x4E, 0x20, 0x52, 0x53, 0x41, 0x20, 0x50, 
	0x55, 0x42, 0x4C, 0x49, 0x43, 0x20, 0x4B, 0x45, 0x59, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0D, 0x0A, 
	0x4D, 0x49, 0x47, 0x4A, 0x41, 0x6F, 0x47, 0x42, 0x41, 0x4E, 0x52, 0x50, 0x41, 0x6A, 0x47, 0x4A, 
	0x59, 0x37, 0x79, 0x67, 0x4A, 0x4B, 0x44, 0x35, 0x32, 0x39, 0x42, 0x43, 0x74, 0x6B, 0x4E, 0x70, 
	0x66, 0x33, 0x31, 0x66, 0x6D, 0x76, 0x53, 0x67, 0x4C, 0x65, 0x47, 0x4D, 0x32, 0x2B, 0x61, 0x39, 
	0x73, 0x6F, 0x76, 0x4A, 0x4A, 0x48, 0x59, 0x50, 0x52, 0x66, 0x74, 0x31, 0x42, 0x4D, 0x32, 0x74, 
	0x0D, 0x0A, 0x4B, 0x46, 0x32, 0x31, 0x76, 0x66, 0x44, 0x4B, 0x58, 0x63, 0x6F, 0x6B, 0x39, 0x53, 
	0x70, 0x73, 0x57, 0x61, 0x71, 0x36, 0x62, 0x37, 0x5A, 0x56, 0x4F, 0x53, 0x30, 0x6D, 0x50, 0x67, 
	0x63, 0x4D, 0x38, 0x66, 0x61, 0x31, 0x50, 0x61, 0x46, 0x79, 0x32, 0x6F, 0x6F, 0x4F, 0x57, 0x48, 
	0x35, 0x59, 0x49, 0x65, 0x54, 0x72, 0x69, 0x2F, 0x49, 0x65, 0x32, 0x6E, 0x43, 0x36, 0x41, 0x44, 
	0x71, 0x63, 0x0D, 0x0A, 0x58, 0x48, 0x42, 0x46, 0x69, 0x43, 0x72, 0x46, 0x36, 0x75, 0x6F, 0x53, 
	0x41, 0x43, 0x45, 0x70, 0x38, 0x46, 0x35, 0x64, 0x54, 0x78, 0x64, 0x2F, 0x69, 0x37, 0x76, 0x62, 
	0x53, 0x6B, 0x70, 0x6B, 0x45, 0x4E, 0x78, 0x71, 0x7A, 0x76, 0x37, 0x6F, 0x68, 0x6B, 0x52, 0x43, 
	0x52, 0x67, 0x4B, 0x7A, 0x4F, 0x72, 0x4C, 0x6A, 0x41, 0x67, 0x4D, 0x42, 0x41, 0x41, 0x45, 0x3D, 
	0x0D, 0x0A, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x45, 0x4E, 0x44, 0x20, 0x52, 0x53, 0x41, 0x20, 0x50, 
	0x55, 0x42, 0x4C, 0x49, 0x43, 0x20, 0x4B, 0x45, 0x59, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D
};

/*
 * client encrypt rsa with pub_key
 *
 * @return
 *     a new ep_buffer_t
 */
ep_buffer_t * ep_cert_client_encrypt(const char * buffer, int size)
{
	char * ptr;
	int ptr_size;
	int r;

	assert(buffer != NULL);
	r = ep_crypt_rsa(RSA_PUB_KEY, sizeof(RSA_PUB_KEY), buffer, size, &ptr, &ptr_size, 1);
	if (r == 0)
	{
		ep_free(ptr);
		assert(0);
		return NULL;
	}

	{
		ep_buffer_t * buffer;

		buffer = ep_buffer_new(ptr, ptr_size);
		ep_free(ptr);

		return buffer;
	}
}

#ifdef EP_HAVE_SERVER

/*
 * server decrypt rsa with pri_key
 *
 * @return
 *     a new ep_buffer_t
 */
ep_buffer_t * ep_cert_server_decrypt(const char * buffer, int size)
{
	char * ptr;
	int ptr_size;
	int r;

	assert(buffer != NULL && size > 0);
	r = ep_crypt_rsa(RSA_PRI_KEY, sizeof(RSA_PRI_KEY), buffer, size, &ptr, &ptr_size, 0);
	if (r == 0)
	{
		assert(0);
		return NULL;
	}

	{
		ep_buffer_t * ret_buffer;
		ret_buffer = ep_buffer_new(ptr, ptr_size);
		ep_free(ptr);

		return ret_buffer;
	}
}

#endif

/*
 * client send aes crypt key/iv to server using safety RSA
 *
 * @request
 *
 * - security say "hello"
 *   value is an rsa encrypted pack
 *       - key  client aes key
 *       - vec client aes vec
 *
 * @response
 *
 * - result
 *    1 sign ok
 *    0 server not response
 *   -1 server set client security failed
 */
int ep_cert_client_security(ep_client_t * client, int wait_timeout)
{
	ep_req_t * handle;
	ep_pack_t * pack;
    ep_aes_key_t aes;
    int ret;
	
    ep_cert_gen_client_key(aes.key, aes.vec);
    ret = ep_io_update_aes(client->io_mgr, client->fd, &aes);
	if (ret != 1)
	{
        assert(0);
		return -1;
	}

	{
		ep_buffer_t * tmp_buffer;
		ep_pack_t * tmp_pack;

		tmp_pack = ep_pack_new();
		if (tmp_pack == NULL)
		{
            assert(0);
			return -1;
		}

		ep_pack_append_binary(tmp_pack, "key", (char *)aes.key, sizeof(aes.key));
		ep_pack_append_binary(tmp_pack, "vec", (char *)aes.vec, sizeof(aes.vec));
		ep_pack_finish(tmp_pack);

		tmp_buffer = ep_cert_client_encrypt(ep_pack_data(tmp_pack), ep_pack_size(tmp_pack));
		ep_pack_destroy(&tmp_pack);
		if (tmp_buffer == NULL)
		{
            assert(0);
			return -1;
		}

		pack = ep_pack_new();
		if (pack == NULL)
		{
            assert(0);
			ep_buffer_free(&tmp_buffer);
			return -1;
		}

		ep_pack_append_binary(pack, "security", ep_buffer_ptr(tmp_buffer), ep_buffer_size(tmp_buffer));
		ep_buffer_free(&tmp_buffer);
	}

	{
		ep_req_param_t param;

		param.category = EP_CATEGORY_INTERNAL_KEY;
		param.type = req_type_get;
		param.c_crypt = crypt_type_none;
		param.c_compress = compress_type_auto;
		param.s_crypt = crypt_type_encrypt;
		param.s_compress = compress_type_auto;
		param.pack = pack;

		handle = ep_req_new(client, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
            assert(0);
			/* 0 server not response */
			return 0;
		}
	}

	if (ep_req_wait(handle, &pack, wait_timeout) == 0)
	{
		ep_req_destroy(&handle);
		/* 0 server not response */
		return 0;
	}

	{
		ep_pack_iterator_t iter;
        int result = -1;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			ep_pack_type type;
			const char * key;

			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "result") == 0)
			{
				result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
    
        ep_req_destroy(&handle);

        return result;
	}
}

#ifdef EP_HAVE_SERVER

int ep_cert_server_rsa_get_decrypt_buffer(ep_pack_t * pack, ep_buffer_t ** buffer)
{
	{
		ep_pack_iterator_t iter;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			ep_pack_type type;
			const char * key;

			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "security") == 0)
			{
				const char * bin;
				int bin_size;

				bin = ep_pack_iterator_bin_data(iter);
				bin_size = ep_pack_interator_bin_len(iter);
				assert(type == PACKET_BINDATA);

				/* rsa 128 bit */
				if (bin_size != 128)
				{
					/* refuse client attack */
                    assert(0);
					return 0;
				}

				*buffer = ep_cert_server_decrypt(bin, bin_size);
				if (*buffer != NULL)
				{
					return 1;
				}

				break;
			}
		}
	}
    
    assert(0);
	return 0;
}

/*
 * server receive aes crypt key/iv from client using safety RSA
 *
 * @request
 *
 * - security say "hello"
 *   value is an rsa encrypted pack
 *       - key  client aes key
 *       - vec client aes vec
 *
 * @response
 *
 * - result
 *    1 sign ok
 *   -1 server set client security failed
 */
ep_pack_t * ep_cert_server_security(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
	ep_aes_key_t aes = { { 0 }, { 0 } };
	ep_buffer_t * buffer;
	int ret;

	ret = ep_cert_server_rsa_get_decrypt_buffer(pack, &buffer);
	if (!ret)
	{
		pack = ep_pack_new();
		if (pack != NULL)
		{
			ep_pack_append_int(pack, "result", -1);
		}
		
		return pack;
	}

	pack = ep_pack_create(ep_buffer_ptr(buffer), ep_buffer_size(buffer));
	ep_buffer_free(&buffer);
	if (pack == NULL)
	{
		pack = ep_pack_new();
		if (pack != NULL)
		{
			ep_pack_append_int(pack, "result", -1);
		}

		return pack;
	}

	{
		ep_pack_iterator_t iter;

		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			ep_pack_type type;
			const char * key;

			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);

			if (strcmp(key, "key") == 0)
			{
				memcpy(aes.key, ep_pack_iterator_bin_data(iter), sizeof(aes.key));
				assert(type == PACKET_BINDATA);
			}
			else if (strcmp(key, "vec") == 0)
			{
				memcpy(aes.vec, ep_pack_iterator_bin_data(iter), sizeof(aes.vec));
				assert(type == PACKET_BINDATA);
			}
		}
	}

	ep_pack_destroy(&pack);
	pack = ep_pack_new();
	if (pack == NULL)
	{
        assert(0);
		return NULL;
	}

    if (ep_cert_verify_client_key(aes.key, aes.vec) != 1)
    {
		ep_pack_append_int(pack, "result", -1);
        assert(0);
		return pack;
	}

	{
		ep_io_mgr_t * s_io_mgr;
		int ret;

		s_io_mgr = (ep_io_mgr_t *)opaque;
		ret = ep_io_update_aes(s_io_mgr, fd, &aes);
		if (ret == 0)
		{
			ep_pack_append_int(pack, "result", -1);
            assert(0);
			return pack;
		}
	}

	/* server init client security ok */
	ep_pack_append_int(pack, "result", 1);

	return pack;
}

#endif
