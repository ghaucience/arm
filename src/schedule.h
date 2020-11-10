#ifndef _AMBER_SCHEDUE_H_
#define _AMBER_SCHEDUE_H_

typedef unsigned long long _U64_t;
typedef long long _S64_t;

typedef struct stSchduleTask {
	void			*func;
	void			*arg;
	_U64_t 		start;
	_U64_t 		delt;
	
	struct stSchduleTask *next;
}stSchduleTask_t;


void schedule_init(void *_th, void *_fet);

void schedue_add(stSchduleTask_t *at, _U64_t ms, void *func, void *arg);

stSchduleTask_t *schedue_first_task_to_exec();

_S64_t schedue_first_task_delay();

void schedue_del(stSchduleTask_t *at);

_U64_t schedue_current();

void schedue_exec();

#define USE_RUN_LOOP 0

#if USE_RUN_LOOP 
#include "timer.h"
#include "lockqueue.h"
#include "file_event.h"
typedef struct stSchduleEnv {
	struct file_event_table *fet;
	struct timer_head *th;

	struct timer step_timer;
}stSchduleEnv_t;
#endif

#endif
