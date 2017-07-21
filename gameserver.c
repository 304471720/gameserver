#include "gameserver.h"
#include "poker_regular.h"

/*
PST_DESK ROOM_assignLocation(struct player_info *user,PST_ROOM pRoom)
{
    assert(user != NULL && pRoom != NULL);
    int i=0,j=0;
    while((*pRoom).desk[i].player[j].pUser != NULL) //随机查找房间内id为0的（空位子）
    {
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
    sprintf(pDesk->message,"第%d桌  ... 第%d位 ...client %s come in !\n",i,j,user->_id);
    pDesk->id = i*CONST_DESK_PLAYERNUM;
    return pDesk;
}*/

int 
main( int argc, char **argv )
{
        //run_daemon();
        if (init_conf() < 0)
                return -1;
        SERVER *server = init_server(conf->port, conf->workernum, conf->connnum, conf->timeout,
                        conf->timeout);
        if (server == NULL)
                return -1;
	
	regist_cmd_proc_func((uint16_t)CMD_FUNCTION_REGIST,player_register);
	regist_cmd_proc_func((uint16_t)CMD_FUNCTION_LOGIN,player_login);
	regist_cmd_proc_func((uint16_t)CMD_FUNCTION_ENTER_DESK,player_enter_desk);
 	//regist_akg_func(CMD_FUNCTION_LOGIN, player_register);       
	if (start_log_thread() != 0) {
                fprintf(stderr, "[error] start_log_thread, errno: %d %m\n", errno);
                return -1;
        }
	
	if(!initPool(10))
        {
                printf( "启动数据库连接池失败 ...\n" );
                return -1;
        }
        printf( "启动数据库连接池成功... \n" );        
	
	start_server((void*) server);
        return EXIT_SUCCESS;
}
