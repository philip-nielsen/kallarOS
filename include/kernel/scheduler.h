#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <kernel/thread.h>
#include <stdint.h>

#define FIRST_LIMIT 10
#define SECOND_LIMIT 20
#define THIRD_LIMIT 30

/**
 * yield() - Yields the current thread to the next in the queue.
 */
void yield(void);

void schedule(void);

void lock_scheduler(void);

void unlock_scheduler(void);

void enqueue_thread(thread_control_block_t *thread, uint8_t thread_priority);

thread_control_block_t *dequeue_thread(void);

void scheduler_on_tick(void);

#endif