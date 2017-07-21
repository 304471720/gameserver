#ifndef __DB_CONNECTOR_POOL_H
#define __DB_CONNECTOR_POOL_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <mysql.h>
#include <assert.h>
#include <pthread.h>

#ifndef TRUE
        #define TRUE 1
#endif

#ifndef FALSE
        #define FALSE 0
#endif
#define CONNECT_POOL_MAX_SIZE 100

extern MYSQL **connectpool;
extern int8_t pooltotalsize;
extern int8_t poolusedsize;
extern pthread_mutex_t conn_lock;

typedef enum RET_STATUS
{
        RETURN_SUCCESS=0,
        RETURN_FAILS
}ENUM_RET_STATUS;

MYSQL * initMysql();
ENUM_RET_STATUS initPool(int8_t iPoolNum);
MYSQL *conn_from_pool();
void releaseConn(MYSQL *c);
void closePool();
ENUM_RET_STATUS realloc_pool();
int registerUserInfo(MYSQL * pConn_ptr,char *email ,char *surname,char *password);
int checkUserInfo(MYSQL *pConn_ptr,char *email);
//uint32_t player_login(CONN * c);
void closeMysql(MYSQL *conn_ptr);

#endif
