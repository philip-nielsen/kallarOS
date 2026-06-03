#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <kernel/thread.h>
#include <stdint.h>

#define FIRST_LIMIT 10
#define SECOND_LIMIT 20
#define THIRD_LIMIT 30

/**
 * yield() - Yields the CPU from the current thread to the next in the MLFQ.
 */
void yield(void);

/**
 * lock_scheduler() - Disables hardware interrupts to protect scheduler state.
 */
void lock_scheduler(void);

/**
 * unlock_scheduler() - Re-enables hardware interrupts if the lock depth reaches
 * zero.
 */
void unlock_scheduler(void);

/**
 * lock_atomic() - Initiates an atomic block, buffering any context switch
 * requests.
 */
void lock_atomic(void);

/**
 * unlock_atomic() - Ends an atomic block and executes any postponed context
 * switches.
 */
void unlock_atomic(void);

/**
 * enqueue_thread() - Inserts a thread into the specified MLFQ priority tier.
 * @thread: Pointer to the thread control block.
 * @thread_priority: Target priority queue (0 is highest).
 */
void enqueue_thread(thread_control_block_t *thread, uint8_t thread_priority);

/**
 * dequeue_thread() - Extracts the highest-priority available thread from the
 * MLFQ.
 * Return: Pointer to the next thread to run, or NULL if all queues are
 * empty.
 */
thread_control_block_t *dequeue_thread(void);

/**
 * scheduler_on_tick() - Core timer interrupt handler.
 * Wakes sleeping threads, updates time slices, and demotes greedy CPU hogs.
 */
void scheduler_on_tick(void);

/**
 * thread_sleep() - Puts the given thread to sleep for a specific duration.
 * @thread: Pointer to the thread control block.
 * @requested_time: Minimum APIC ticks to sleep before awaking.
 */
void thread_sleep(thread_control_block_t *thread, uint32_t requested_time);

/**
 * thread_exit() - Terminates the active thread and moves it to the graveyard.
 */
void thread_exit(void);

/**
 * get_terminated_thread_queue() - Retrieves the list of dead threads.
 * Return: Pointer to the head of the graveyard queue.
 */
thread_control_block_t *get_terminated_thread_queue(void);

/**
 * set_terminated_thread_queue() - Updates the head of the graveyard queue.
 * @new_queue: Pointer to the new head of the queue.
 */
void set_terminated_thread_queue(thread_control_block_t *new_queue);

#endif