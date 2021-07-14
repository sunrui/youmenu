/*
 * guid provider
 * @2012-12-5 rui.sun
 */
#include "guid.h"

#include <stdio.h>

#ifdef WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
typedef struct _GUID
{
	unsigned int Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} GUID, uid;
#endif

void guid_generate(char guid[38 + 1])
{
	GUID uid;
#ifdef WIN32
	CoCreateGuid(&uid);
#else
	uuid_generate((unsigned char *)(&uid));
#endif

	sprintf(
		guid,
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		uid.Data1, uid.Data2, uid.Data3,
		uid.Data4[0], uid.Data4[1],
		uid.Data4[2], uid.Data4[3],
		uid.Data4[4], uid.Data4[5],
		uid.Data4[6], uid.Data4[7]);
}