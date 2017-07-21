#ifndef __PACKAGE_OP_H
#define __PACKAGE_OP_H

#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include   <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#define PACKAGE_MAX_LENGTH 4096 
#define ITEM_MAX_LENGTH 256

uint8_t * InitPackage();
void WriteString_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint8_t *sSrcBufferStartPos);
void WriteData_End(uint8_t *sDestBufferStartPos,uint32_t iPackageSumLength);
void WriteInt_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint32_t iIntValue);
void WriteShort_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint16_t iShortValue);
void WriteByte_ING(uint8_t *sDestBufferStartPos,uint32_t *iCurrentLength ,uint8_t iByteValue);
void Close(uint8_t *pPackageStart);
uint32_t InitReadPackage(uint8_t **pRecvPackage);
uint8_t ReadByte_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength);
uint16_t ReadShort_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength);
uint32_t ReadInt_ING(uint8_t **pRecvPackage,uint32_t *iPackageLength);
uint16_t ReadString_ING(uint8_t **pRecvPackage,uint8_t *sDestBuff,uint32_t *iPackageLength);

#endif
