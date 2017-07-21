#include "package_op.h"
/*#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include   <string.h>
#include <stdio.h>

#define PACKAGE_MAX_LENGTH 4096 
#define ITEM_MAX_LENGTH 256
*/

uint8_t * InitPackage()
{
	assert(PACKAGE_MAX_LENGTH > sizeof(uint32_t));
	uint8_t *pPackage =  (uint8_t *)calloc(PACKAGE_MAX_LENGTH,sizeof(uint8_t));
	if(pPackage != NULL)
	{
		bzero(pPackage,PACKAGE_MAX_LENGTH);
		return pPackage;
	}
	return NULL;
}

void WriteString_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint8_t *sSrcBufferStartPos)
{
	assert(sDestBufferStartPos!= NULL && sSrcBufferStartPos != NULL);
	//uint16_t iSrcLength = htons(strlen(sSrcBufferStartPos));
	uint16_t iSrcLength = strlen(sSrcBufferStartPos);
	uint16_t tmp_iSrcLength = htons(iSrcLength);
	memcpy(sDestBufferStartPos+(*iCurrentLength),&tmp_iSrcLength,sizeof(uint16_t));
	memcpy(sDestBufferStartPos+(*iCurrentLength)+ sizeof(uint16_t),sSrcBufferStartPos,iSrcLength);
	*iCurrentLength += iSrcLength + sizeof(uint16_t);
}

void WriteData_End(uint8_t *sDestBufferStartPos,uint32_t iPackageSumLength)
{
	assert(sDestBufferStartPos != NULL && iPackageSumLength < PACKAGE_MAX_LENGTH);
	uint32_t iSum=iPackageSumLength - sizeof(uint32_t);
	iSum = htonl(iSum);
	memcpy(sDestBufferStartPos,&iSum,sizeof(uint32_t));
	return ;
}

void WriteInt_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint32_t iIntValue)
{
        assert(sDestBufferStartPos!= NULL);
	uint32_t tmp_iIntValue = htonl(iIntValue);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&tmp_iIntValue,sizeof(uint32_t));
        *iCurrentLength  +=  sizeof(uint32_t);
}

void WriteShort_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint16_t iShortValue)
{
        assert(sDestBufferStartPos!= NULL);
	uint16_t tmp_iShortValue = htons(iShortValue);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&tmp_iShortValue,sizeof(uint16_t));
        *iCurrentLength += sizeof(uint16_t);
}

void WriteByte_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint8_t iByteValue)
{
        assert(sDestBufferStartPos!= NULL);
        memcpy(sDestBufferStartPos+(*iCurrentLength),&iByteValue,sizeof(uint8_t));
        *iCurrentLength  +=  sizeof(uint8_t);
}


void Close(uint8_t *pPackageStart)
{
	if(pPackageStart!= NULL)
	{
		bzero(pPackageStart,PACKAGE_MAX_LENGTH);
		free(pPackageStart);
	}
}


uint32_t InitReadPackage(uint8_t **pRecvPackage)
{
	assert(*pRecvPackage != NULL);
	uint32_t iRet=0;
	memcpy(&iRet,*pRecvPackage,sizeof(uint32_t));
	*pRecvPackage += sizeof(uint32_t);
	assert(iRet >  0 && iRet <= 0xffff);
	return iRet;
}

uint8_t ReadByte_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength)
{
	assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0);
	uint8_t iRet =0;
	memcpy(&iRet,*pRecvPackage,sizeof(uint8_t));
	*pRecvPackage = (*pRecvPackage) + sizeof(uint8_t);
	*iPackageLength -= sizeof(uint8_t);
	assert(*iPackageLength >= 0);
	return iRet;
}


uint16_t ReadShort_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength)
{
        assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0);
        uint16_t iRet =0;
        memcpy(&iRet,*pRecvPackage,sizeof(uint16_t));
        *pRecvPackage += sizeof(uint16_t);
        *iPackageLength -= sizeof(uint16_t);
        assert(*iPackageLength >= 0);
        return iRet;
}


uint32_t ReadInt_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength)
{
	printf("Int-----------------1  %p \n",*pRecvPackage);
        assert(*pRecvPackage != NULL && iPackageLength != NULL && *iPackageLength > 0) ;
	printf("Int-----------------2  \n");
        uint32_t iRet =0;
        memcpy(&iRet,*pRecvPackage,sizeof(uint32_t));
	printf("Int-----------------3 %d  \n",iRet);
        *pRecvPackage += sizeof(uint32_t);
        *iPackageLength -= sizeof(uint32_t);
        assert(*iPackageLength >= 0);
        return iRet;
}

uint16_t ReadString_ING(uint8_t **pRecvPackage,uint8_t *sDestBuff,uint32_t *iPackageLength)
{
        assert(*pRecvPackage != NULL);
	assert(iPackageLength != NULL);
	assert(*iPackageLength >= 0);
	assert(sDestBuff != NULL);
        uint16_t iStrLen =0;
        memcpy(&iStrLen,*pRecvPackage,sizeof(uint16_t));
	iStrLen = ntohs(iStrLen);
        *pRecvPackage += sizeof(uint16_t);
	memcpy(sDestBuff,*pRecvPackage,iStrLen);
	*pRecvPackage += iStrLen;
        *iPackageLength -= iStrLen;
        return iStrLen;
}
