#ifndef MARIO_UTIL_H_
#define MARIO_UTIL_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
void nsleep(uint32_t ns);

void susleep(uint32_t s, uint32_t us);

void run_daemon();

typedef void (*sighandler_t)(int);
void signal_handler(int sig, sighandler_t handler);

#endif /* MARIO_UTIL_H_ */
