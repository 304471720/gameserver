#ifndef __CONNECTION_H
#define __CONNECTION_H
#include <stdlib.h>
#include <string.h>
#include "global.h"
//#include <event.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
typedef struct _WORKER WORKER;

typedef struct _CONN CONN;


struct _CONN {
	char _id[30];
	int fd;
	struct bufferevent *bufev;
	uint32_t ind; //conn连接index
	char in_buf[CONN_BUF_LEN];//输入缓冲区
	uint16_t in_buf_len; //输入缓冲区长度
	char out_buf[CONN_BUF_LEN];//输出缓冲区
	uint16_t out_buf_len;//输出缓冲区长度
	CONN *next;//指向下一个连接指针
	WORKER *owner;//所属worker线程
	uint32_t cip; //客户端ip
	uint16_t cport;//客户端端口
	uint16_t err_type;
	void *user; //放置客户端结构指针
        void* pRoom;
        void* pDesk;
        void* pPlayer;
};

/*
这是个广泛使用的常见技巧，常用来构成缓冲区。比起指针，用空数组有这样的优势：  
1.不需要初始化，数组名直接就是所在的偏移  
2.不占任何空间，指针需要占用int长度空间，空数组不占任何空间。  
“这个数组不占用任何内存”，意味着这样的结构节省空间；
“该数组的内存地址就和他后面的元素的地址相同”，
意味着无需初始化，数组名就是后面元素的地址，直接就能当做指针使用。

这样的好处是：  
一次分配解决问题，省了不少麻烦。大家知道为了防止内存泄漏，如果是分两次分配（结构体和缓冲区），
那么要是第二次malloc失败了，必须回滚释放第一个分配的结构体。这样带来了编码麻烦。
其次，分配了第二个缓冲区以后，如果结构里面用的是指针，还要为这个指针赋值。
同样，在free这个buffer的时候，用指针也要两次free。如果用空数组，所有问题一次解决。  
其次，大家知道小内存的管理是非常困难的，如果用指针，这个buffer的struct部分就是小内存了，
在系统内存在多了势必严重影响内存管理的性能。
要是用空数组把struct和实际数据缓冲区一次分配大块问题，就没有这个问题。  
*/
typedef struct _CONN_LIST {
	CONN *head;
	CONN *tail;
	CONN list[0];
}CONN_LIST;

CONN_LIST* init_conn_list(uint32_t size);

#define PUT_FREE_CONN(list, item)	\
		list->tail->next = item;	\
		list->tail = item;
#define	GET_FREE_CONN(list, item)	\
		if(list->head != list->tail){	\
			item = list->head;	\
			list->head = list->head->next;	\
		} else {	\
			item = NULL;\
		}

#endif /* __CONNECTION_H */
