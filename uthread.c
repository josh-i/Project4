#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define _UTHREAD_PRIVATE
#include "context.h"
#include "queue.h"
#include "uthread.h"

struct uthread_tcb *t_cur;
struct uthread_tcb *prev;
queue_t q_block;
queue_t q_ready;

struct uthread_tcb {
	int state;
	uthread_ctx_t *context;
	void* s_address;
};

void uthread_yield(void)
{
	if(t_cur->state == block){ 
		queue_enqueue(q_block, (void*)t_cur);

	}
	else if(t_cur->state == running){
		t_cur->state = ready;
		queue_enqueue(q_ready, (void*)t_cur);

	}

	else if (t_cur->state == done){
		free(t_cur);
	}

	prev = malloc(sizeof(struct uthread_tcb));
	prev->context = malloc(sizeof(uthread_ctx_t));
	prev = t_cur;

	queue_dequeue(q_ready, (void**)&t_cur);
	t_cur->state = running;
	uthread_ctx_switch(prev->context, t_cur->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	if (func == NULL) return -1;

	struct uthread_tcb *thread = malloc (sizeof(struct uthread_tcb));
	thread->context = malloc(sizeof(uthread_ctx_t));
        
	thread->state = block;
	thread->s_address = uthread_ctx_alloc_stack();
	uthread_ctx_init(thread->context, thread->s_address, func, arg);
	thread->state = ready;
        
	queue_enqueue(q_ready, (void*)thread);

	return 0;
}

void uthread_exit(void)
{
	t_cur->state = done;
	uthread_yield();

	// if there is next, dequeue the current one, and set the current one to the next one in ready queue	
	if(q_ready->size > 0){

	queue_dequeue(q_ready, (void**)&t_cur);

	}
}

void uthread_block(void)
{
	t_cur->state = block;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = ready;
	queue_enqueue(q_ready, (void*)uthread);
}

struct uthread_tcb *uthread_current(void)
{
	return t_cur;
}

void uthread_start(uthread_func_t start, void *arg)
{	
	q_block = queue_create();
	q_ready = queue_create();

	// create an initial thread
	uthread_create(start, arg);


	// create an idle thread
	t_cur = malloc(sizeof(struct uthread_tcb));
	t_cur->context = malloc(sizeof(uthread_ctx_t));
	t_cur->s_address = uthread_ctx_alloc_stack();
	t_cur->state = running;

	// loop
	while(q_ready->size != 0)
	{
		uthread_yield();

	}
}
