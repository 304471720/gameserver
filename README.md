# gameserver

入口函数 gameserver.c
借鉴memcache网络编程源码开发的。基于libevent ， Master-Worker 模型，主线程负责接收连接，然后将连接分给各个worker线程，在各个worker线程中完成命令的接收，处理和返回结果。
自己实现的网络数据解析与数据发送。package_info.c
自己实现的简单数据库连接池db_connector_pool.c
实现简单的棋牌室，棋牌数据结构定义，和简单的发牌洗牌，判断棋牌大小的程序，poker_regular.h，package_op.h
简单编译脚本build.sh

