#ifndef __PACKAGE_INFO_H_
#define __PACKAGE_INFO_H_
#include "connection.h"

#define MAX_PKG_TYPE (0xffff)
extern const uint16_t CMD_CONNECTED;
extern const uint16_t CMD_TIMEOUT;
extern const uint16_t CMD_CLOSED;
extern const uint16_t CMD_ERROR;

typedef uint32_t (*FUNC_PTR)(CONN* c);

extern FUNC_PTR AKG_FUNC[MAX_PKG_TYPE];

#pragma pack(1)
typedef struct _HEAD {
	//uint32_t authoritiesid ;   //验证客户端，可以是某个字符串如服务器mack地址的Md5编码，客户端用这个地址与服务器通讯，
				  //服务器验证，不符合则放弃包的解析处理。
	uint32_t package_len; //包长度
	uint16_t command;      //命令
} HEAD;


typedef enum _CMD {
        CMD_FUNCTION_BASE = 0x0100,
        CMD_FUNCTION_BASE_REQ,
        CMD_FUNCTION_REGIST,
        CMD_FUNCTION_REGIST_REQ,
	CMD_FUNCTION_LOGIN,
	CMD_FUNCTION_LOGIN_REQ,
	CMD_FUNCTION_ENTER_DESK,
	CMD_FUNCTION_ENTER_DESK_REQ
} CMD;

uint32_t player_register(CONN * c);
uint32_t player_login(CONN * c);
uint32_t player_enter_desk(CONN * c);
#pragma pack()
#define DEFAULT_STX	(0xffffffff)
extern uint32_t g_authoritiesid;  //接收客户端发来的验证码
#define SIZE_OF_HEAD	((uint16_t) 6)

#endif /* __AKG_H_ */
