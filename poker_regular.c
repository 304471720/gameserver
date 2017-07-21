//  Hello World server

//#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <stdlib.h>
#include <time.h>
#include "poker_regular.h"
//#include "connection.h"

const int CONST_PLAYER_CARD_INDEX_COMBINATION[10][CONST_PLAYER_CARDNUM] =  //所有三张牌的排列组合从中判断牌型
                                        //前三个位置判断是否有牛，如果有牛，计算后两个位置计算牌型
                        {
                                {1,2,3,4,5},
                                {1,2,4,3,5},
                                {1,2,5,3,4},
                                {1,3,4,2,5},
                                {1,3,5,2,4},
                                {1,4,5,2,3},
                                {2,3,4,1,5},
                                {2,3,5,1,4},
                                {2,4,5,1,3},
                                {3,4,5,1,2}
                        };


const ST_POKER CONST_POKER =
        {
                {CARD_A,WEIGHT_1,REAL_VALUE_1},{CARD_A,WEIGHT_2,REAL_VALUE_1},{CARD_A,WEIGHT_3,REAL_VALUE_1},{CARD_A,WEIGHT_4,REAL_VALUE_1},
                {CARD_2,WEIGHT_1,REAL_VALUE_2},{CARD_2,WEIGHT_2,REAL_VALUE_2},{CARD_2,WEIGHT_3,REAL_VALUE_2},{CARD_2,WEIGHT_4,REAL_VALUE_2},
                {CARD_3,WEIGHT_1,REAL_VALUE_3},{CARD_3,WEIGHT_2,REAL_VALUE_3},{CARD_3,WEIGHT_3,REAL_VALUE_3},{CARD_3,WEIGHT_4,REAL_VALUE_3},
                {CARD_4,WEIGHT_1,REAL_VALUE_4},{CARD_4,WEIGHT_2,REAL_VALUE_4},{CARD_4,WEIGHT_3,REAL_VALUE_4},{CARD_4,WEIGHT_4,REAL_VALUE_4},
                {CARD_5,WEIGHT_1,REAL_VALUE_5},{CARD_5,WEIGHT_2,REAL_VALUE_5},{CARD_5,WEIGHT_3,REAL_VALUE_5},{CARD_5,WEIGHT_4,REAL_VALUE_5},
                {CARD_6,WEIGHT_1,REAL_VALUE_6},{CARD_6,WEIGHT_2,REAL_VALUE_6},{CARD_6,WEIGHT_3,REAL_VALUE_6},{CARD_6,WEIGHT_4,REAL_VALUE_6},
                {CARD_7,WEIGHT_1,REAL_VALUE_7},{CARD_7,WEIGHT_2,REAL_VALUE_7},{CARD_7,WEIGHT_3,REAL_VALUE_7},{CARD_7,WEIGHT_4,REAL_VALUE_7},
                {CARD_8,WEIGHT_1,REAL_VALUE_8},{CARD_8,WEIGHT_2,REAL_VALUE_8},{CARD_8,WEIGHT_3,REAL_VALUE_8},{CARD_8,WEIGHT_4,REAL_VALUE_8},
                {CARD_9,WEIGHT_1,REAL_VALUE_9},{CARD_9,WEIGHT_2,REAL_VALUE_9},{CARD_9,WEIGHT_3,REAL_VALUE_9},{CARD_9,WEIGHT_4,REAL_VALUE_9},
                {CARD_10,WEIGHT_1,REAL_VALUE_10},{CARD_10,WEIGHT_2,REAL_VALUE_10},{CARD_10,WEIGHT_3,REAL_VALUE_10},{CARD_10,WEIGHT_4,REAL_VALUE_10},
                {CARD_J,WEIGHT_1,REAL_VALUE_10},{CARD_J,WEIGHT_2,REAL_VALUE_10},{CARD_J,WEIGHT_3,REAL_VALUE_10},{CARD_J,WEIGHT_4,REAL_VALUE_10},
                {CARD_Q,WEIGHT_1,REAL_VALUE_10},{CARD_Q,WEIGHT_2,REAL_VALUE_10},{CARD_Q,WEIGHT_3,REAL_VALUE_10},{CARD_Q,WEIGHT_4,REAL_VALUE_10},
                {CARD_K,WEIGHT_1,REAL_VALUE_10},{CARD_K,WEIGHT_2,REAL_VALUE_10},{CARD_K,WEIGHT_3,REAL_VALUE_10},{CARD_K,WEIGHT_4,REAL_VALUE_10},
                {CARD_X,WEIGHT_1,REAL_VALUE_0},
                {CARD_D,WEIGHT_1,REAL_VALUE_0}
        };


PST_PLAYER ROOM_getLocation(TYPE_NUMBER client_fd,PST_ROOM pRoom)
{
	int i,j=0;
	srand(time(NULL));
	i=rand()%(CONST_ROOM_DESKNUM);
	j=rand()%(CONST_DESK_PLAYERNUM);
	while((*pRoom).desk[i].player[j].id != 0) //随机查找房间内id为0的（空位子）
	{
		i=rand()%(CONST_ROOM_DESKNUM);
		j=rand()%(CONST_DESK_PLAYERNUM);
	}
	PST_PLAYER pRet = &(*pRoom).desk[i].player[j];  //随机找到空位置，把player标志设置为client socket
	printf("第%d桌  ... 第%d位 ...client_fd : %d \n",i,j,(int)client_fd);
	pRet->id =client_fd;
	return pRet;
}


void PRINT_CLIENT_LOCATION(PST_ROOM pRoom)
{
	int i=0,j=0;
	for(i=0;i<CONST_ROOM_DESKNUM;i++)
	{
		for(j=0;j<CONST_DESK_PLAYERNUM;j++)
		{
			if((int)(*pRoom).desk[i].player[j].id != 0)
			{
				printf(" %d ",(int)(*pRoom).desk[i].player[j].id);	
			}
		}
		//printf(" --\n ");
	}
}

ENUM_CARD_TYPE  DESK_getCardType(PST_DESK pDesk)
{
	int i,j=0;
	int sum_player_left = 0;
	int sum_player_right = 0;
	int iMax=0;
	int iValue =0;
	int index0=0,index1=0,index2=0,index3=0,index4=0;
	for(i=0;i<CONST_DESK_PLAYERNUM;i++)
	{
		iValue = -1;
		for(j=0;j<10;j++)
		{
			index0 = CONST_PLAYER_CARD_INDEX_COMBINATION[j][0] - 1;
			index1 = CONST_PLAYER_CARD_INDEX_COMBINATION[j][1] - 1;
			index2 = CONST_PLAYER_CARD_INDEX_COMBINATION[j][2] - 1;
			index3 = CONST_PLAYER_CARD_INDEX_COMBINATION[j][3] - 1;
			index4 = CONST_PLAYER_CARD_INDEX_COMBINATION[j][4] - 1;
			sum_player_left = 0;
			sum_player_right =0;

			sum_player_left = pDesk->player[i].card[index0].realValue + pDesk->player[i].card[index1].realValue + pDesk->player[i].card[index2].realValue;
			sum_player_right = pDesk->player[i].card[index3].realValue+pDesk->player[i].card[index4].realValue;
			
			if((sum_player_left+sum_player_right)%10 == 0)
			{
				pDesk->player[i].cardType = OX_OX;
				iValue = OX_OX;
				break;
			}
			if(sum_player_left%10 == 0)
			{
				iValue = sum_player_right%10;
				if(pDesk->player[i].cardType<iValue)
					pDesk->player[i].cardType = iValue;
			}
		}
		if(iValue == -1 )
		{
			pDesk->player[i].cardType == NO_OX;
		}
	}
}


void PRINT_PLAYER_CARDS(PST_CARDS pCards);
void PRINT_DESK(PST_DESK pDesk);

void PRINT_DESK(PST_DESK pDesk)
{
	assert(pDesk);
	int i=0,j=0;
	for(i=0;i<CONST_DESK_PLAYERNUM;i++)
	{
		PST_PLAYER pPlayer = &(pDesk->player[i]);
		printf("player cards denomitation : ------" );
		printf("%d %d %d %d %d ",pPlayer->card[0].denomination,pPlayer->card[1].denomination,pPlayer->card[2].denomination,pPlayer->card[3].denomination,pPlayer->card[4].denomination);
		printf("----\n" );
		printf("player cardType:*******" );
		printf(" %d ",pPlayer->cardType);
		printf("*******\n" );
	}
}	

void DESK_Licensing(PST_POKER pPoker,PST_DESK pDesk)
{
	assert(pDesk);
	int i=0;
	assert(CONST_DESK_PLAYERNUM < (52/5));
	for(i=0;i<CONST_DESK_PLAYERNUM;i++)
	{
		memcpy(&(pDesk->player[i].card),(*pPoker) + i * CONST_PLAYER_CARDNUM ,sizeof(ST_CARD)*CONST_PLAYER_CARDNUM);
	}
}

PST_DESK ROOM_getNewDesk()
{
	PST_DESK t = (PST_DESK)malloc(sizeof(ST_DESK));
	assert(t);
	memset(t,0,sizeof(ST_DESK));
	return t;
}

PST_POKER  DESK_getNewPoker() //一副新扑克
{
	PST_POKER t =  (PST_POKER)malloc(sizeof(ST_POKER));
	assert(t);
	memset(t,0,sizeof(ST_POKER));
	memcpy(t,&CONST_POKER,sizeof(ST_POKER));
	return t;
}

void PRINT_POKER(PST_POKER poker)
{
	int i;
	assert(poker);
	for(i=0;i<CONST_POKER_CARDNUM;i++)
	{
		printf("%d,%d \n",(*poker)[i].denomination,(*poker)[i].weight);
	}
	printf("......poker....\n");
}

void PRINT_PLAYER_CARDS(PST_CARDS pCards)
{
        int i;
        assert(pCards);
        for(i=0;i<CONST_PLAYER_CARDNUM;i++)
        {
                printf("%d,%d ,%d ",(*pCards)[i].denomination,(*pCards)[i].weight,(*pCards)[i].realValue);
        }
	printf(".....player cards ....\n");
}
void DESK_Shuffle(PST_POKER poker) //洗牌
{
	assert(poker);
	int i,j;
 	long k;
 	ST_CARD temp;
 	srand(time(NULL));
 	for(k=1;k<CONST_TEST_COUNT;k++)
 	{
  		i=rand()%52;
  		j=rand()%52;
  		temp=(*poker)[i];
  		(*poker)[i]=(*poker)[j];
  		(*poker)[j]=temp;
	}
}
void test()
{
	//PST_POKER pPoker = DESK_getNewPoker();
	//PRINT_POKER(pPoker);
	//DESK_Shuffle(pPoker);
	//PST_DESK pDesk = ROOM_getNewDesk();
	//DESK_Licensing(pPoker,pDesk);
	//PRINT_DESK(pDesk);
	//DESK_getCardType(pDesk);
	//PRINT_DESK(pDesk);
	static int i=1;
	//for(i=1;i<2;i++)
	ROOM_getLocation(i++,&ROOM_LEVEL_1);
	PRINT_CLIENT_LOCATION(&ROOM_LEVEL_1);
	/*PRINT_PLAYER_CARDS(&(pDesk->player[0].card));
	PRINT_PLAYER_CARDS(&(pDesk->player[1].card));
	PRINT_PLAYER_CARDS(&(pDesk->player[2].card));
	PRINT_PLAYER_CARDS(&(pDesk->player[3].card));
	printf("\n");*/
	//sleep(1);
	//free(pPoker);
	//free(pDesk);
	//pPoker = NULL;
	//pDesk = NULL;
}

void test_time(int num)
{
	int i;
	double cost_time;
	clock_t start=clock();
	//for(i=0;i<num;i++)
	//{
	//	test();	
	//}
	ROOM_getLocation(999999,&ROOM_LEVEL_1);
	clock_t end=clock();
	cost_time=(double)(end-start)/CLOCKS_PER_SEC;
	printf("loop count :%d 所用时间为%f \n",num,cost_time);
}
/*
int zeromq_main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
	test_time(1);
	printf ("Received Hello\n");
        sleep (1);         
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}*/

uint8_t 
ROOM_assignLocation(CONN *conn,uint8_t aera)
{
    CONN * user = (CONN *)conn;
    assert(user != NULL);
    PST_ROOM pRoom = NULL;
    if(aera == 1)
    {
	pRoom = &ROOM_LEVEL_1;
    }else if(aera == 2)
    {
    	pRoom = &ROOM_LEVEL_2;
    }else if(aera == 3)
    {
        pRoom = &ROOM_LEVEL_3;
    }else
    {
	return 0;
    }
    
    int i=0,j=0;
    while((*pRoom).desk[i].player[j].pUser != NULL) //随机查找房间内id为0的（空位子）
    {
		if(i>=CONST_ROOM_DESKNUM && j >= CONST_DESK_PLAYERNUM)
			return 0;
                j = (++j)%(CONST_DESK_PLAYERNUM);
                if(j==0)
                   i++;
    }
	
    PST_PLAYER pPlayer = &(*pRoom).desk[i].player[j];  //随机找到空位置，把player标志设置为client socket
    //sprintf(pConn->message,"第%d桌  ... 第%d位 ...client %s\n",i,j,pConn->name);
    pPlayer->pUser =user;
    pPlayer->id = i*CONST_DESK_PLAYERNUM + j;
  
    user->pPlayer = pPlayer;
    PST_DESK pDesk = &((*pRoom).desk[i]);
    sprintf(pDesk->message,"第%d桌 第%d位 client %s come in !\n",i,j,user->_id);
    pDesk->id = i*CONST_DESK_PLAYERNUM;
    user->pDesk = pDesk;
    return 1;
}
