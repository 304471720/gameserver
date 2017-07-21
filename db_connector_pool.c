#include "db_connector_pool.h"

/*#include <stdio.h>
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

static MYSQL **connectpool;
static int8_t pooltotalsize;
static int8_t poolusedsize;
static pthread_mutex_t conn_lock = PTHREAD_MUTEX_INITIALIZER;

typedef enum RET_STATUS
{
	RETURN_SUCCESS=0,
	RETURN_FAILS
}ENUM_RET_STATUS;
*/


MYSQL **connectpool;
int8_t pooltotalsize;
int8_t poolusedsize;
pthread_mutex_t conn_lock = PTHREAD_MUTEX_INITIALIZER;

MYSQL * initMysql()
{
        char *host = "127.0.0.1";
        char *user = "root";
        char *password = "bityoo";
        char *db = "bityoo";
        unsigned int port = 0;
        char *unix_socket = NULL;
        unsigned long client_flag = 0;
        MYSQL *conn_ptr = mysql_init(NULL);
        if(!conn_ptr)
        {
                fprintf(stderr, "init mysql failed\n");
                return ;
        }
        conn_ptr = mysql_real_connect(conn_ptr, host, user, password, db, port, unix_socket, client_flag);
        if(conn_ptr)
        {
                printf("Connection success......\n");
        }
        else                                      
        {
                fprintf(stderr, "Connection failed......%d:%s\n", errno, strerror(errno));
        }
        return conn_ptr;
}


ENUM_RET_STATUS initPool(int8_t iPoolNum)
{ 
	assert(iPoolNum <= CONNECT_POOL_MAX_SIZE);
    	pooltotalsize = iPoolNum;
    	poolusedsize = 0;
    	if ((connectpool = calloc(iPoolNum, sizeof(MYSQL *))) == NULL) 
	{
        	fprintf(stderr, "Failed to allocate connections pool \n");
    	}
    	fprintf(stdout, "allocate connections pool successfull!!!\n");
	int8_t i=0;
    	for(i=0;i<iPoolNum;i++)
	{
		connectpool[i] = initMysql();
        	if(!(connectpool[i]))
        	{
                	fprintf(stderr, "init mysql pool failed\n");
                	return FALSE;
        	}
	}
        return TRUE;
}


/*
 * Returns a connection from the pool, if any.
 */
MYSQL *conn_from_pool() {
    MYSQL *c;
    pthread_mutex_lock(&conn_lock);
    if (poolusedsize < pooltotalsize) {
        c = connectpool[poolusedsize++];
    } else {
        c = NULL;
    }
    pthread_mutex_unlock(&conn_lock);
    return c;
}

void releaseConn(MYSQL *c)
{
    pthread_mutex_lock(&conn_lock);
    if(poolusedsize > 0 && poolusedsize< pooltotalsize)
    	connectpool[poolusedsize--] = c;
    pthread_mutex_unlock(&conn_lock);
}

void closePool()
{	
	for(;poolusedsize>0;poolusedsize--)
	{
		mysql_close(connectpool[poolusedsize]);
	}
}

/*
 * Adds a connection to the freelist. 0 = success.
 */
ENUM_RET_STATUS realloc_pool() {
    int8_t ret = RETURN_FAILS;
    pthread_mutex_lock(&conn_lock);
    {
        /* try to enlarge free connections array */
        size_t newsize = pooltotalsize * 2;
        MYSQL **new_connectpool = realloc(connectpool, sizeof(int32_t) * newsize);
        if (new_connectpool) {
            pooltotalsize = newsize;
            connectpool = new_connectpool;
            connectpool[poolusedsize++] = initMysql();
            ret = RETURN_SUCCESS;
        }
    }
    pthread_mutex_unlock(&conn_lock);
    return ret;
}

int registerUserInfo(MYSQL * pConn_ptr,char *email ,char *surname,char *password)
{
        char sRegisterStr[1000];

        assert(email != NULL && password != NULL && surname != NULL);
        sprintf(sRegisterStr,"insert into TB_ADM_USER(USER_ID,USER_NAME,PASSWORD)  VALUES('%s','%s','%s')",email,surname,password);
        int iRet =  mysql_query(pConn_ptr, sRegisterStr);
        mysql_autocommit(pConn_ptr, TRUE);
        printf("%s  commit count : %d \n",sRegisterStr,iRet);
        return iRet;
}

int checkUserInfo(MYSQL *pConn_ptr,char *email)
{
        assert(pConn_ptr != NULL &&  email != NULL);
        MYSQL_ROW row;
        char sCheckUserInfoSql[100];
        sprintf(sCheckUserInfoSql,"select * from TB_ADM_USER where user_id = '%s' \n",email);
        int iRet = mysql_query(pConn_ptr,sCheckUserInfoSql);
        MYSQL_RES *res = mysql_store_result(pConn_ptr);
        if(res != NULL)
        {
                while((row = mysql_fetch_row(res)))
                {
                        fprintf(stdout, "result set : ---------------------  %s\t%s\t%s\n", row[0], row[1],row[2]);
                }
                iRet = mysql_num_rows(res);
        }
        //      iRet = mysql_num_rows(res);
        printf("%s select count : %d",sCheckUserInfoSql,iRet);
        mysql_free_result(res);
        return iRet;
}

//void closeMysql(MYSQL *conn_ptr);
void closeMysql(MYSQL *conn_ptr)
{
        mysql_close(conn_ptr);
}

