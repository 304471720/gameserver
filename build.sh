#gcc -o test  gameserver.c poker_regular.c  dbconnector.c  package_op.c -I /usr/local/mysql/include  -lmysqlclient  -levent
#nohup ./gameserver > gameserver.log 2>&1 &
#gcc -o test  gameserver.c poker_regular.c  package_op.c db_connector_pool.c server_util.c  package_info.c network.c log.c config.c connection.c  -I /usr/local/mysql/include  -lmysqlclient  -levent

gcc -o gameserver  gameserver.c poker_regular.c  package_op.c db_connector_pool.c server_util.c  package_info.c network.c log.c config.c connection.c  -I /usr/local/mysql/include  -lmysqlclient  -levent
