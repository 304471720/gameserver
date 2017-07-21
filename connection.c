#include "connection.h"
//初始化线程数组
CONN_LIST* init_conn_list(uint32_t size) {
	CONN_LIST *lst = NULL;
	uint32_t len = sizeof(CONN_LIST) + sizeof(CONN) * (size + 1);
	lst = (CONN_LIST*) malloc(len);
	if (lst == NULL)
		return NULL;
	memset(lst, 0, len);
	lst->head = &lst->list[0];
	lst->tail = &lst->list[size];
	int i = 0;
	//初始化list,数组内并设置成链表结构
	for (; i < size; i++) {
		lst->list[i].ind = i;
		lst->list[i].next = &lst->list[i + 1];
	}
	lst->list[size].ind = size;
	lst->list[size].next = NULL;
	return lst;
}
