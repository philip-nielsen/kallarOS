#define PRINT 1

#include <arch/i386/apic.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <libc/stdio.h>
#include <stdint.h>
#include <util/debug.h>

static thread_control_block_t *current_thread;
static thread_control_block_t *idle_thread;
static uint32_t id = 1;

static void cleaner_task() {
    unlock_scheduler();
    for (;;) {
        lock_scheduler();
        thread_control_block_t *terminated_thread_queue =
            get_terminated_thread_queue();

        if (terminated_thread_queue) {
            set_terminated_thread_queue(NULL);
        }

        unlock_scheduler();
        while (terminated_thread_queue) {
            pr("\nTERMINATE THREAD %d\n", terminated_thread_queue->id);
            thread_control_block_t *tmp = terminated_thread_queue->next;
            kfree((uint32_t)terminated_thread_queue->stack_base);
            kfree((uint32_t)terminated_thread_queue);
            terminated_thread_queue = tmp;
        }

        thread_sleep(get_current_thread(), 2000);
    }
}

static void idle_thread_method() {
    unlock_scheduler();
    for (;;) {
        __asm__ volatile("hlt");
    }
}

void initialize_multitasking() {
    thread_control_block_t *root_thread =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));
    root_thread->esp = 0;
    root_thread->esp0 = 0;
    root_thread->state = THREAD_RUNNING;
    root_thread->id = 0;
    root_thread->time_used = 0;
    root_thread->priority = 0;
    root_thread->remaining_time = FIRST_LIMIT;
    current_thread = root_thread;

    idle_thread =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));
    uint32_t idle_stack_base = (uint32_t)kmalloc(4096);
    uint32_t *idle_stack = (uint32_t *)(idle_stack_base + 4096 - 24);

    idle_stack[0] = 0;     // pop edi
    idle_stack[1] = 0;     // pop esi
    idle_stack[2] = 0;     // pop ebx
    idle_stack[3] = 0;     // pop ebp
    idle_stack[4] = 0x202; // popfd (EFLAGS with interrupts enabled)
    idle_stack[5] = (uint32_t)idle_thread_method; // ret
    idle_stack[6] = (uint32_t)thread_exit;

    idle_thread->esp = (void *)idle_stack;
    idle_thread->stack_base = (void *)idle_stack_base;
    idle_thread->state = THREAD_READY;
    idle_thread->time_used = 0;
    idle_thread->priority = 2;
    idle_thread->id = UINT32_MAX - 1;
    idle_thread->remaining_time = 0;

    create_kernel_thread(cleaner_task);
}

void create_kernel_thread(void (*entry_point)()) {
    thread_control_block_t *new_thread =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));

    uint32_t stack_base = (uint32_t)kmalloc(4096);
    uint32_t *stack = (uint32_t *)(stack_base + 4096 - 24);

    stack[0] = 0;                     // pop edi
    stack[1] = 0;                     // pop esi
    stack[2] = 0;                     // pop ebx
    stack[3] = 0;                     // pop ebp
    stack[4] = 0x202;                 // popfd (EFLAGS with interrupts enabled)
    stack[5] = (uint32_t)entry_point; // ret
    stack[6] = (uint32_t)thread_exit;

    new_thread->esp = (void *)stack;
    new_thread->stack_base = (void *)stack_base;
    new_thread->state = THREAD_READY;
    new_thread->time_used = 0;
    new_thread->priority = 0;
    new_thread->id = ++id;
    new_thread->remaining_time = FIRST_LIMIT;

    enqueue_thread(new_thread, 0);
}

thread_control_block_t *get_current_thread() { return current_thread; }

void set_current_thread(thread_control_block_t *new_thread) {
    current_thread = new_thread;
}

thread_control_block_t *get_idle_thread() { return idle_thread; }

semaphore_t *create_semaphore(uint16_t max_count) {
    semaphore_t *new_semaphore = (semaphore_t *)kmalloc(sizeof(semaphore_t));
    new_semaphore->max_count = max_count;
    new_semaphore->current_count = 0;
    new_semaphore->first_waiting_thread = NULL;
    new_semaphore->last_waiting_thread = NULL;

    return new_semaphore;
}

mutex_t *create_mutex() {
    mutex_t *new_mutex = (mutex_t *)kmalloc(sizeof(mutex_t));
    new_mutex->owner_thread = NULL;
    new_mutex->first_waiting_thread = NULL;
    new_mutex->last_waiting_thread = NULL;

    return new_mutex;
}

void acquire_semaphore(semaphore_t *semaphore) {
    lock_atomic();
    if (semaphore->current_count < semaphore->max_count) {
        semaphore->current_count++;
    } else {
        current_thread->next = NULL;
        if (!semaphore->first_waiting_thread) {
            semaphore->first_waiting_thread = current_thread;
        } else {
            semaphore->last_waiting_thread->next = current_thread;
        }
        semaphore->last_waiting_thread = current_thread;
        current_thread->state = THREAD_BLOCKED;
        yield();
    }
    unlock_atomic();
}

void acquire_mutex(mutex_t *mutex) {
    lock_atomic();
    if (!mutex->owner_thread) {
        mutex->owner_thread = current_thread;
    } else {
        current_thread->next = NULL;
        if (!mutex->first_waiting_thread) {
            mutex->first_waiting_thread = current_thread;
        } else {
            mutex->last_waiting_thread->next = current_thread;
        }
        mutex->last_waiting_thread = current_thread;
        current_thread->state = THREAD_BLOCKED;
        yield();
    }
    unlock_atomic();
}

void release_semaphore(semaphore_t *semaphore) {
    lock_atomic();

    if (semaphore->first_waiting_thread) {
        thread_control_block_t *thread = semaphore->first_waiting_thread;
        thread->state = THREAD_READY;
        semaphore->first_waiting_thread = thread->next;
        enqueue_thread(thread, thread->priority);
        yield();
    } else {
        semaphore->current_count--;
    }
    unlock_atomic();
}

void release_mutex(mutex_t *mutex) {
    lock_atomic();
    if (!mutex->first_waiting_thread) {
        mutex->owner_thread = NULL;
    } else {
        thread_control_block_t *next_thread = mutex->first_waiting_thread;
        mutex->first_waiting_thread = mutex->first_waiting_thread->next;

        next_thread->state = THREAD_READY;
        enqueue_thread(next_thread, next_thread->priority);
        mutex->owner_thread = next_thread;
        yield();
    }
    unlock_atomic();
}