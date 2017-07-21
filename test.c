//#include "package_op.h"
#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include   <string.h>
#include <stdio.h>

#define PACKAGE_MAX_LENGTH 4096 
#define ITEM_MAX_LENGTH 256

int8_t * InitPackage()
{
	assert(PACKAGE_MAX_LENGTH > sizeof(int32_t));
	int8_t *pPackage =  (int8_t *)calloc(PACKAGE_MAX_LENGTH,sizeof(int8_t));
	if(pPackage != NULL)
	{
		bzero(pPackage,PACKAGE_MAX_LENGTH);
		return pPackage;
	}
	return NULL;
}

void WriteString_ING(int8_t *sDestBufferStartPos,int32_t *iCurrentLength ,int8_t *sSrcBufferStartPos)
{
	assert(sDestBufferStartPos!= NULL && sSrcBufferStartPos != NULL);
	int16_t iSrcLength = strlen(sSrcBufferStartPos);
	memcpy(sDestBufferStartPos+(*iCurrentLength),&iSrcLength,sizeof(int16_t));
	memcpy(sDestBufferStartPos+(*iCurrentLength)+ sizeof(int16_t),sSrcBufferStartPos,iSrcLength);
	*iCurrentLength += iSrcLength + sizeof(int16_t);
}

void WriteData_End(int8_t *sDestBufferStartPos,int32_t iPackageSumLength)
{
	assert(sDestBufferStartPos != NULL && iPackageSumLength < PACKAGE_MAX_LENGTH);
	int32_t iSum=iPackageSumLength - sizeof(int32_t);
	memcpy(sDestBufferStartPos,&iSum,sizeof(int32_t));
	return ;
}

void WriteInt_ING(int8_t *sDestBufferStartPos,int32_t *iCurrentLength ,int32_t iIntValue)
{
        assert(sDestBufferStartPos!= NULL);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&iIntValue,sizeof(int32_t));
        *iCurrentLength  +=  sizeof(int32_t);
}

void WriteShort_ING(int8_t *sDestBufferStartPos,int32_t *iCurrentLength ,int16_t iShortValue)
{
        assert(sDestBufferStartPos!= NULL);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&iShortValue,sizeof(int16_t));
        *iCurrentLength += sizeof(int16_t);
}

void WriteByte_ING(int8_t *sDestBufferStartPos,int32_t *iCurrentLength ,int8_t iByteValue)
{
        assert(sDestBufferStartPos!= NULL);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&iByteValue,sizeof(int8_t));
        *iCurrentLength  +=  sizeof(int8_t);
}


void Close(int8_t *pPackageStart)
{
	if(pPackageStart!= NULL)
	{
		bzero(pPackageStart,PACKAGE_MAX_LENGTH);
		free(pPackageStart);
	}
}


int32_t InitReadPackage(int8_t **pRecvPackage)
{
	assert(*pRecvPackage != NULL);
	int32_t iRet=0;
	memcpy(&iRet,*pRecvPackage,sizeof(int32_t));
	*pRecvPackage += sizeof(int32_t);
	assert(iRet >  0 && iRet <= 0xffff);
	return iRet;
}

int8_t ReadByte_ING(int8_t **pRecvPackage,int32_t *iPackageLength)
{
	printf("-----------------1 %p \n",*pRecvPackage);
	assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0);
	printf("-----------------2  \n");
	int8_t iRet =0;
	memcpy(&iRet,*pRecvPackage,sizeof(int8_t));
	printf("-----------------3  %d \n",iRet);
	*pRecvPackage = (*pRecvPackage) + sizeof(int8_t);
	*iPackageLength -= sizeof(int8_t);
	assert(*iPackageLength >= 0);
	return iRet;
}


int16_t ReadShort_ING(int8_t **pRecvPackage,int32_t *iPackageLength)
{
        assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0);
        int16_t iRet =0;
        memcpy(&iRet,*pRecvPackage,sizeof(int16_t));
        *pRecvPackage += sizeof(int16_t);
        *iPackageLength -= sizeof(int16_t);
        assert(*iPackageLength >= 0);
        return iRet;
}


int32_t ReadInt_ING(int8_t **pRecvPackage,int32_t *iPackageLength)
{
	printf("Int-----------------1  %p \n",*pRecvPackage);
        assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0) ;
	printf("Int-----------------2  \n");
        int32_t iRet =0;
        memcpy(&iRet,*pRecvPackage,sizeof(int32_t));
	printf("Int-----------------3 %d  \n",iRet);
        *pRecvPackage += sizeof(int32_t);
        *iPackageLength -= sizeof(int32_t);
        assert(*iPackageLength >= 0);
        return iRet;
}

int16_t ReadString_ING(int8_t **pRecvPackage,int8_t *sDestBuff,int32_t *iPackageLength)
{
        assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength >= 0 && sDestBuff != NULL) ;
        int16_t iStrLen =0;
        memcpy(&iStrLen,*pRecvPackage,sizeof(int16_t));
        *pRecvPackage += sizeof(int16_t);
	memcpy(sDestBuff,*pRecvPackage,iStrLen);
	*pRecvPackage += iStrLen;
        *iPackageLength -= iStrLen;
        return iStrLen;
}

int 
main( int argc, char **argv )
{	
int32_t iContentStartPos=sizeof(int32_t);
int8_t *pTest = InitPackage();
//int8_t *pTest = pPackageStart + sizeof(int32_t);
int8_t statusCode=10;
int8_t* errMsg="error i don't know!!!";
int32_t loginSign=678901234;
int8_t* serverIp="192.168.1.1!!!";
int16_t  serverPort= 11211 ;

WriteByte_ING(pTest,&iContentStartPos,statusCode);
WriteInt_ING(pTest,&iContentStartPos,loginSign);
WriteString_ING(pTest,&iContentStartPos,serverIp);
WriteShort_ING(pTest,&iContentStartPos,serverPort);
WriteData_End(pTest,iContentStartPos);

printf("-----Write End -----------------%d \n",iContentStartPos);
printf("-----Read begin ----------------- \n");

int8_t *pTest1 = pTest;
int32_t iPackageLength = InitReadPackage(&pTest1);

int8_t i_statusCode=-1;
int32_t i_loginSign=0;
int8_t s_serverIp[200]={0};
int16_t i_port=0;

printf("----- Read Number -----------------%d \n",iPackageLength);
i_statusCode = ReadByte_ING(&pTest1,&iPackageLength);
i_loginSign = ReadInt_ING(&pTest1,&iPackageLength);
ReadString_ING(&pTest1,s_serverIp,&iPackageLength);
i_port = ReadShort_ING(&pTest1,&iPackageLength);

printf("----====================== -- %d ------- \n",i_statusCode);
printf("---------------------- -- %d ------- \n",i_loginSign);
printf("------------------- -- %s ------- \n",s_serverIp);
printf("------------- -- %d ------- \n",i_port);

Close(pTest);
pTest = NULL;
}
