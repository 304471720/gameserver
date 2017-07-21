#include "package_info.h"
#include "poker_regular.h"
#include "db_connector_pool.h"


const uint16_t CMD_CONNECTED = 0xfffc;
const uint16_t CMD_TIMEOUT = 0xfffd;
const uint16_t CMD_CLOSED = 0xfffe;
const uint16_t CMD_ERROR = 0xffff;

FUNC_PTR AKG_FUNC[MAX_PKG_TYPE];
uint32_t g_authoritiesid;

inline  uint32_t CMD_TYPE( uint16_t str, uint16_t cmd )
{
        return (str == cmd );
}
inline void regist_cmd_proc_func(uint16_t id, FUNC_PTR func) {
        //CMD_PROCESSOR_FUNC[id] = func;
	AKG_FUNC[id] = func;
}
/*
typedef enum _CMD {
        CMD_FUNCTION_BASE = 0x0100,
	CMD_FUNCTION_BASE_REQ,
        CMD_FUNCTION_REGIST,
	CMD_FUNCTION_REGIST_REQ
} CMD;
*/

uint32_t 
player_register(CONN * c)//struct player_info *user,uint8_t *pkgcontent,int32_t pkglength)
{
	uint8_t * pkgcontent = c->in_buf + SIZE_OF_HEAD;
	int32_t pkglength = c->in_buf_len - SIZE_OF_HEAD;
        uint8_t  sEmail[100]={0};
        uint8_t sTradePassword[100]={0};
        uint8_t sPassword[100]={0};
        uint8_t sSureName[100]={0};
        uint8_t *pTest1 = pkgcontent;
        uint32_t iPackageLength = pkglength;
        //InitReadPackage(&pTest1);
        fprintf(stdout,"----- Read Number -----------------%d \n",iPackageLength);
        ReadString_ING(&pTest1,sEmail,&iPackageLength);
        ReadString_ING(&pTest1,sTradePassword,&iPackageLength);
        ReadString_ING(&pTest1,sPassword,&iPackageLength);
        ReadString_ING(&pTest1,sSureName,&iPackageLength);
        fprintf(stdout,"--------------- %s \n",sEmail);
        fprintf(stdout,"--------------- %s \n",sTradePassword);
        fprintf(stdout,"--------------- %s \n",sPassword);
        fprintf(stdout,"--------------- %s \n",sSureName);
        uint32_t iContentStartPos=sizeof(int32_t);
        uint8_t *pTest = InitPackage();
        uint8_t statusCode=1;
        int8_t* errMsg="error i don't know!";
        uint32_t loginSign=987654329;
        uint8_t* serverIp="192.168.1.1";
        uint16_t  serverPort= 11211;
        MYSQL * pConn_ptr = conn_from_pool();
        fprintf(stdout," conn_from_pool  mysql * %p pooltotalsize %d poolusedsize %d \n",pConn_ptr,pooltotalsize,poolusedsize);
	WriteShort_ING(pTest,&iContentStartPos,(uint16_t)CMD_FUNCTION_REGIST_REQ);
        if(!registerUserInfo(pConn_ptr,sEmail ,sSureName,sPassword))
        {
                WriteByte_ING(pTest,&iContentStartPos,0);
                WriteString_ING(pTest,&iContentStartPos,errMsg);
        }else
        {
                WriteByte_ING(pTest,&iContentStartPos,1);
                WriteInt_ING(pTest,&iContentStartPos,loginSign);
                WriteString_ING(pTest,&iContentStartPos,serverIp);
                WriteShort_ING(pTest,&iContentStartPos,serverPort);
        }
        releaseConn(pConn_ptr);
        WriteData_End(pTest,iContentStartPos);
        printf("-----Write End -----------------%d \n",iContentStartPos);
        write(c->fd,pTest,iContentStartPos);
        Close(pTest);
        pTest = NULL;
        return 0;
}

uint32_t
player_login(CONN * c)   
{
        uint8_t * pkgcontent = c->in_buf + SIZE_OF_HEAD;
        int32_t pkglength = c->in_buf_len - SIZE_OF_HEAD;
        uint8_t  sEmail[100]={0};
        uint8_t sPassword[100]={0};
        uint8_t *pTest1 = pkgcontent;
        uint32_t iPackageLength = pkglength;
        fprintf(stdout,"----- Read Number -----------------%d \n",iPackageLength);
        ReadString_ING(&pTest1,sEmail,&iPackageLength);
        ReadString_ING(&pTest1,sPassword,&iPackageLength);
        fprintf(stdout,"--------------- %s \n",sEmail);;
        fprintf(stdout,"--------------- %s \n",sPassword);

        uint32_t iContentStartPos=sizeof(int32_t);
        uint8_t *pTest = InitPackage();
        uint8_t statusCode=0;
        int8_t errMsg[100]={0};
        uint32_t loginSign=0;
        uint8_t nickName[50]={0};;
	uint32_t picture=0;
	uint32_t level=0;
	uint8_t timeCount[20] ={0};
	uint8_t vip=0;
	uint8_t coin[30]={0};
	
        MYSQL * pConn_ptr = conn_from_pool();
        fprintf(stdout," conn_from_pool  mysql * %p pooltotalsize %d poolusedsize %d \n",pConn_ptr,pooltotalsize,poolusedsize);
        WriteShort_ING(pTest,&iContentStartPos,(uint16_t)CMD_FUNCTION_LOGIN_REQ);

        MYSQL_ROW row;
        char getUserInfoSql[100];
        sprintf(getUserInfoSql,"select * from TB_ADM_USER where user_id = '%s' and password = %s\n",sEmail,sPassword);
        int iRet = mysql_query(pConn_ptr,getUserInfoSql);
        MYSQL_RES *res = mysql_store_result(pConn_ptr);
        if(res != NULL)
        {
                while((row = mysql_fetch_row(res)))
                {
                        fprintf(stdout, "result set : ---------------------  %s\t%s\t%s\t%s\n", row[1], row[14],row[10],row[8]);	
			statusCode = 1;
			loginSign=(uint32_t)99999;
			sprintf(nickName,"%s", row[1]);
			if(row[14] != NULL)
				picture = (uint32_t)atoi(row[14]);
			else 
				picture = (uint32_t)10000001;
			level = (uint32_t)atoi(row[10]);
			//timeCount = (uint32_t)0;
			sprintf(timeCount,"1111111111000000000");
			if(row[8] != NULL )
			
				vip=(uint8_t)atoi(row[8]);
			else
				vip=(uint8_t)0;	
			sprintf(coin,"333333333300000000001");
        		
                }
                //iRet = mysql_num_rows(res);
        }
	else
	{
		statusCode = 0;
		sprintf(errMsg,"login fails!!!");
	}
        mysql_free_result(res);

        if(statusCode)
        {
        	printf(" 11===================================  \n");
                WriteByte_ING(pTest,&iContentStartPos,statusCode);
                WriteInt_ING(pTest,&iContentStartPos,loginSign);
                WriteString_ING(pTest,&iContentStartPos,nickName);
                WriteInt_ING(pTest,&iContentStartPos,picture);
		WriteInt_ING(pTest,&iContentStartPos,level);
		WriteString_ING(pTest,&iContentStartPos,timeCount);
		WriteByte_ING(pTest,&iContentStartPos,vip);
		WriteString_ING(pTest,&iContentStartPos,coin);
        }else
        {
        	printf(" 22===================================  \n");
                WriteByte_ING(pTest,&iContentStartPos,statusCode);
                WriteString_ING(pTest,&iContentStartPos,errMsg);               
        }
	WriteData_End(pTest,iContentStartPos);
        
        printf("-----Write End -----------------%d \n",iContentStartPos);
        write(c->fd,pTest,iContentStartPos);
        //write(c->fd,"test",4);

	releaseConn(pConn_ptr);
        Close(pTest);
        pTest = NULL;
        return 0;
}

uint32_t
player_enter_desk(CONN * c)//struct player_info *user,uint8_t *pkgcontent,int32_t pkglength)
{
        uint8_t * pkgcontent = c->in_buf + SIZE_OF_HEAD;
        int32_t pkglength = c->in_buf_len - SIZE_OF_HEAD;
        uint8_t area=-1;

        uint8_t *pTest1 = pkgcontent;
        uint32_t iPackageLength = pkglength;
        fprintf(stdout,"----- Read Number %d \n",iPackageLength);
	area = ReadByte_ING(&pTest1,&iPackageLength);
	
        fprintf(stdout,"------------area :--- %d \n",area);


        uint32_t iContentStartPos=sizeof(int32_t);
        uint8_t *pTest = InitPackage();
        uint8_t statusCode=0;
	int8_t errMsg[100]={0};
	uint32_t num=0;
	uint8_t isPlaying=0;

        sprintf(errMsg,"-----------errmsg--------\n");
        WriteShort_ING(pTest,&iContentStartPos,(uint16_t)CMD_FUNCTION_ENTER_DESK_REQ);
        PST_DESK pDesk = NULL;
	if(!ROOM_assignLocation(c,area))
        {
                WriteByte_ING(pTest,&iContentStartPos,statusCode);
                WriteString_ING(pTest,&iContentStartPos,errMsg);
        }else
        {
                WriteByte_ING(pTest,&iContentStartPos,statusCode);
		pDesk = c->pDesk;
		assert(pDesk != NULL);
                WriteInt_ING(pTest,&iContentStartPos,pDesk->id);
        }
        WriteData_End(pTest,iContentStartPos);
        printf("-----Write End -----------------%d \n",iContentStartPos);
        write(c->fd,pTest,iContentStartPos);
	//write(c->fd,pDesk->message,strlen(pDesk->message));
        Close(pTest);
        pTest = NULL;
        return 0;
}

