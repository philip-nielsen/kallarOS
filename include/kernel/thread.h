#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

/**
 * enum thread_state - Lifecycle states of a kernel thread.
 */
typedef enum thread_state {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_BLOCKED,
    THREAD_TERMINATED
} thread_state;

/**
 * struct thread_control_block - Stores the execution context of a thread.
 * @esp: Stack pointer for context switching.
 * @esp0: Kernel stack pointer for ring 3 transitions.
 * @cr3: Page directory base register (for address space).
 * @next: Pointer to the next thread in a queue.
 * @state: Current lifecycle state.
 * @stack_base: Base address of the thread's stack (for memory cleanup).
 * @time_used: Total CPU ticks consumed.
 * @wake_time: Target tick for awaking from a sleep state.
 * @id: Unique thread identifier.
 * @saved_interrupt_state: The EFLAGS register saved during scheduler locks.
 * @priority: MLFQ priority level (0 is highest).
 * @remaining_time: Ticks left in the current time slice.
 */
typedef struct thread_control_block {
    void *esp;
    void *esp0;
    void *cr3;
    struct thread_control_block *next;
    thread_state state;
    void *stack_base;
    uint32_t time_used;
    uint64_t wake_time;
    uint32_t id;
    uint32_t saved_interrupt_state;
    uint8_t priority;
    int8_t remaining_time;
} thread_control_block_t;

/**
 * struct semaphore - A counting semaphore for thread synchronization.
 * @max_count: Maximum number of available tokens.
 * @current_count: Currently checked out tokens.
 * @first_waiting_thread: Head of the blocked threads queue.
 * @last_waiting_thread: Tail of the blocked threads queue.
 */
typedef struct semaphore {
    uint16_t max_count;
    uint16_t current_count;
    thread_control_block_t *first_waiting_thread;
    thread_control_block_t *last_waiting_thread;
} semaphore_t;

/**
 * struct mutex - A mutual exclusion lock for thread synchronization.
 * @owner_thread: Thread currently holding the lock.
 * @first_waiting_thread: Head of the blocked threads queue.
 * @last_waiting_thread: Tail of the blocked threads queue.
 */
typedef struct mutex {
    thread_control_block_t *owner_thread;
    thread_control_block_t *first_waiting_thread;
    thread_control_block_t *last_waiting_thread;
} mutex_t;

/**
 * switch_to_task() - Architecture-specific assembly routine for context
 * switching.
 * @old_esp_ptr: Pointer to the outgoing thread's stack pointer.
 * @new_esp: The incoming thread's stack pointer.
 */
extern void switch_to_task(void **old_esp_ptr, void *new_esp);

/**
 * initialize_multitasking() - Initializes the MLFQ scheduler.
 * Allocates the root thread, sets up the idle thread, and starts the cleaner
 * task.
 */
void initialize_multitasking(void);

/**
 * create_kernel_thread() - Creates and enqueues a new kernel task.
 * @entry_point: Function pointer to the thread's execution start.
 */
void create_kernel_thread(void (*entry_point)());

/**
 * get_current_thread() - Retrieves the currently executing thread.
 */
thread_control_block_t *get_current_thread();

/**
 * set_current_thread() - Updates the active thread pointer.
 * @new_thread: Pointer to the thread taking over the CPU.
 */
void set_current_thread(thread_control_block_t *new_thread);

/**
 * get_idle_thread() - Retrieves the isolated fallback thread.
 */
thread_control_block_t *get_idle_thread(void);

/**
 * create_semaphore() - Allocates and initializes a new semaphore.
 * @max_count: Maximum capacity of the semaphore.
 * Return: Pointer to the newly created semaphore.
 */
semaphore_t *create_semaphore(uint16_t max_count);

/**
 * create_mutex() - Allocates and initializes a new mutex.
 * Return: Pointer to the newly created mutex.
 */
mutex_t *create_mutex(void);

/**
 * acquire_semaphore() - Attempts to acquire a token from the semaphore.
 * @semaphore: The target semaphore.
 * Blocks and enqueues the caller if the semaphore is at capacity.
 */
void acquire_semaphore(semaphore_t *semaphore);

/**
 * acquire_mutex() - Attempts to acquire exclusive ownership of the mutex.
 * @mutex: The target mutex.
 * Blocks and enqueues the caller if the mutex is already owned.
 */
void acquire_mutex(mutex_t *mutex);

/**
 * release_semaphore() - Returns a token to the semaphore.
 * @semaphore: The target semaphore.
 * Wakes the first blocked thread in the queue, or decrements the count.
 */
void release_semaphore(semaphore_t *semaphore);

/**
 * release_mutex() - Releases exclusive ownership of the mutex.
 * @mutex: The target mutex.
 * Transfers ownership directly to the first blocked thread if the queue is not
 * empty.
 */
void release_mutex(mutex_t *mutex);

#endif