//#pragma pack(1)
#ifndef  _POKER_DEFINE_H
#define  _POKER_DEFINE_H

#define  CONST_PLAYER_CARDNUM  5 //每个玩家牌的数量
#define  CONST_DESK_PLAYERNUM  6 //每个桌面玩家的数量
#define  CONST_DESK_MULTIPLE  3  //倍数
#define  CONST_ROOM_DESKNUM  20000 //每个房间的桌数
#define  CONST_DISTRICT_ROOMNUM  200 //每个区域房间数量
#define  CONST_POKER_CARDNUM  54 //每副扑克的数量
#define  CONST_TEST_COUNT 100
#define CONST_DESK_MESSAGE_LENGTH 100

#include <stdint.h>
#include "connection.h"

typedef long TYPE_NUMBER; //定义编号的数据类型
typedef long TYPE_CURRENCY;//定义币的数据类型
//#include "gameserver.h"
extern const int CONST_PLAYER_CARD_INDEX_COMBINATION[10][CONST_PLAYER_CARDNUM] ;  //所有三张牌的排列组合从中判断牌型

typedef enum CARD_DENO
{
    	CARD_A = 1,
	CARD_2 = 2,
	CARD_3 = 3,
	CARD_4 = 4,
  	CARD_5 = 5,
	CARD_6 = 6,
	CARD_7 = 7,
	CARD_8 = 8,
	CARD_9 = 9,
	CARD_10 = 10,
	CARD_J = 11,
	CARD_Q = 12,
	CARD_K = 13,
	CARD_X = 14,
	CARD_D = 15
}ENUM_CARD_DENO;
typedef enum CARD_WEIGHT
{
	WEIGHT_1 = 1,
	WEIGHT_2 = 2,
	WEIGHT_3 = 3,
	WEIGHT_4 = 4
}ENUM_CARD_WEIGHT; //牌的权值

typedef enum CARD_REAL_VALUE
{
	REAL_VALUE_0 = 0,
	REAL_VALUE_1 = 1,
	REAL_VALUE_2,
	REAL_VALUE_3,
	REAL_VALUE_4,
	REAL_VALUE_5,
	REAL_VALUE_6,
	REAL_VALUE_7,
	REAL_VALUE_8,
	REAL_VALUE_9,
	REAL_VALUE_10
}ENUM_CARD_REAL_VALUE;

typedef struct CARD
{
	ENUM_CARD_DENO denomination;//面额
	ENUM_CARD_WEIGHT weight; //权值
	ENUM_CARD_REAL_VALUE realValue;
	
}ST_CARD,*PST_CARD,ST_POKER[CONST_POKER_CARDNUM],(*PST_POKER)[CONST_POKER_CARDNUM],ST_CARDS[CONST_PLAYER_CARDNUM],(*PST_CARDS)[CONST_PLAYER_CARDNUM];

/*
typedef struct POKER
{
	ENUM_CARD_DENO denomination;//面额
	ENUM_CARD_WEIGHT weight; //权值 
}ST_POKER[CONST_POKER_CARDNUM];
*/

extern const ST_POKER CONST_POKER; 


typedef enum DESK_ROLE
{
	GENERAL = 0, //普通
	BANKER = 1//庄家
}ENUM_DESK_ROLE;

typedef enum CARD_TYPE //牌型
{
	NO_OX = 0,   //没牛
	OX_1 = 1,   //牛1
	OX_2 = 2,   //牛2
	OX_3 = 3,   //牛3 ...
	OX_4 = 4,   
	OX_5 = 5,
	OX_6 = 6,
	OX_7 = 7,
	OX_8 = 8,
	OX_9 = 9,
	OX_OX = 10  //牛牛
}ENUM_CARD_TYPE;
typedef struct ST_PLAYER
{
	TYPE_NUMBER  id;
	void* pUser;
	ST_CARDS card; //玩家的牌
	ENUM_CARD_TYPE cardType;  //牌型
	ENUM_DESK_ROLE  isMakers; //是否是庄家 0,
	TYPE_CURRENCY ox_currency;//牛币
	TYPE_CURRENCY bet; //押注
}ST_PLAYER,*PST_PLAYER,ST_PLAYERS[CONST_DESK_PLAYERNUM],(*PST_PLAYERS)[CONST_DESK_PLAYERNUM];

typedef enum ROOM_TYPE
{
        LEVEL_1=1,
        LEVEL_2,
        LEVEL_3,
        LEVEL_4
}ENUM_ROOM_TYPE;

typedef struct DESK //桌面
{
	TYPE_NUMBER id;
	uint8_t isPlaying;
	char message[CONST_DESK_MESSAGE_LENGTH];
	ST_PLAYERS player;	
}ST_DESK,*PST_DESK;

typedef struct ROOM //房间
{
	TYPE_NUMBER id;
	ENUM_ROOM_TYPE roomtype;
	ST_DESK desk[CONST_ROOM_DESKNUM];
}ST_ROOM,*PST_ROOM;

/*typedef struct DISTRICT //区域
{
	TYPE_NUMBER id;
	ST_ROOM room[CONST_DISTRICT_ROOMNUM];
}ST_DISTRICT,*PST_DISTRICT;
*/

static ST_ROOM ROOM_LEVEL_1={0,LEVEL_1,0};
static ST_ROOM ROOM_LEVEL_2={0,LEVEL_2,0};
static ST_ROOM ROOM_LEVEL_3={0,LEVEL_3,0};



PST_POKER  DESK_getNewPoker();
void PRINT_POKER(PST_POKER poker);
void DESK_Shuffle(PST_POKER poker);
void DESK_Licensing(PST_POKER pPoker,PST_DESK pDesk); //发牌
PST_DESK ROOM_getNewDesk(); //新开一个桌面
void PRINT_C_5_3();
ENUM_CARD_TYPE  DESK_getCardType(PST_DESK pDesk);
PST_PLAYER ROOM_getLocation(TYPE_NUMBER client_fd,PST_ROOM pRoom);
void PRINT_CLIENT_LOCATION(PST_ROOM pRoom);
void PRINT_PLAYER_CARDS(PST_CARDS pCards);
void PRINT_DESK(PST_DESK pDesk);
uint8_t ROOM_assignLocation(CONN *conn,uint8_t aera);
#endif
