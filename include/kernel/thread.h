#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

/*
Kernel states:
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_TERMINATED
thread_state;
*/
typedef enum thread_state {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_TERMINATED
} thread_state;

typedef struct thread_control_block {
    void *esp;
    void *esp0;
    void *cr3;
    struct thread_control_block *next;
    thread_state state;
    void *stack_base;
    uint32_t time_used;
    uint32_t wake_time;
    uint32_t id;
    uint8_t priority;
    int8_t remaining_time;
} thread_control_block_t;

extern void switch_to_task(void **old_esp_ptr, void *new_esp);

/**
 * initialize_multitasking() - Initialize multitasking.
 * Initialize multitasking by allocating memory for the initial task running
 * since boot. Assigns it to the global pointer, that points to the
 * current running thread for the OS.
 */
void initialize_multitasking(void);

/**
 * create_kernel_task() - Creates a new tesk for the kernel.
 * @entry_point: Pointer to the new task.
 */
void create_kernel_thread(void (*entry_point)());

/**
 * get_current_thread() - Get pointer to the current thread running.
 */
thread_control_block_t *get_current_thread();

/**
 * set_current_thread() - Update the current thread running.
 * @new_thread: Pointer to the new thread.
 */
void set_current_thread(thread_control_block_t *new_thread);

/**
 * get_idle_thread() - Get pointer to the isolated idle thread.
 */
thread_control_block_t *get_idle_thread(void);

#endif