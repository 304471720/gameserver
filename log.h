#ifndef __LOG_H_
#define __LOG_H_
#include <stdio.h>
#include <unistd.h>
#include "mario_table.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LOG_LINE_LENGTH (2048)
typedef struct _LOG {
	int fd;
	char buf[MAX_LOG_LINE_LENGTH]; //日志缓冲区
} LOG;

//日志队列
typedef struct _LOG_QUEUE {
	uint32_t size;
	uint32_t producer;
	uint32_t consumer;
	LOG items[0];
} LOG_QUEUE;

LOG_QUEUE* create_log_queue();

uint32_t record_log(LOG_QUEUE *queue, int fd, const char *fmt, ...);

uint32_t start_log_thread();

#define slog_err_t_w(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[%ld] [ERROR] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_err_t(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[%ld] [ERROR] "fmt"\n", (long)time(NULL), ##args);
#define slog_err(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[ERROR] "fmt"\n", ##args);
#define slog_info_t_w(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [INFO] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_info_t(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [INFO] "fmt"\n", (long)time(NULL), ##args);
#define slog_info(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[INFO] "fmt"\n", ##args);
#define slog_warn_t_w(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [WARN] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_warn_t(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [WARN] "fmt"\n", (long)time(NULL), ##args);
#define slog_warn(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[WARN] "fmt"\n", ##args);

#endif /* __LOG_H_ */
