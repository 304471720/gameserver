#ifndef __GAMESERVER_H
#define __GAMESERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Required by event.h. */
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "queue.h"
#include <assert.h>

#include "log.h"
#include "network.h"
#include "config.h"
#include "package_info.h"

#pragma pack(1)
typedef struct _PKG_HEAD {
	uint32_t pkglen;
	uint16_t command;
} PKG_HEAD;

#endif
