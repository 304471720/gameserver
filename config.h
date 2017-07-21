#ifndef __CONF_H__
#define __CONF_H__
#include <stdint.h>
//#include "mario/mario.h"

#pragma pack(1)
typedef struct _CONF {
	uint16_t port;
	uint32_t timeout;
	uint32_t workernum;
	uint32_t connnum;
	uint32_t usernum;
	uint32_t filenum;
} CONF;
#pragma pack()

uint32_t init_conf();

extern CONF* conf;

#endif	//__CONF_H__
