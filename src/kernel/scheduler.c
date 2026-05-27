#include <arch/i386/apic.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <libc/stdio.h>
#include <stddef.h>

static uint8_t IRQ_disable_counter;
static volatile uint8_t counter;

static thread_control_block_t *first_ready_to_run_thread_list[3];
static thread_control_block_t *last_ready_to_run_thread_list[3];

void enqueue_thread(thread_control_block_t *thread, uint8_t thread_priority) {
    thread->next = NULL;
    thread->priority = thread_priority;
    if (!first_ready_to_run_thread_list[thread_priority]) {
        first_ready_to_run_thread_list[thread_priority] = thread;
        last_ready_to_run_thread_list[thread_priority] = thread;
    } else {
        last_ready_to_run_thread_list[thread_priority]->next = thread;
        last_ready_to_run_thread_list[thread_priority] = thread;
    }
}

thread_control_block_t *dequeue_thread() {
    int i = 0;
    while (!first_ready_to_run_thread_list[i]) {
        if (++i == 3) {
            return NULL;
        }
    }

    thread_control_block_t *r = first_ready_to_run_thread_list[i];
    first_ready_to_run_thread_list[i] = first_ready_to_run_thread_list[i]->next;
    return r;
}

void move_thread(thread_control_block_t *thread, uint8_t thread_priority) {
    thread_control_block_t *current =
        first_ready_to_run_thread_list[thread_priority];
    thread_control_block_t *prev = current;

    while (current) {
        if (current->id == thread->id) {
            if (prev) {
                prev->next = current->next;
            } else {
                first_ready_to_run_thread_list[thread_priority] = current->next;
            }
        }
        prev = current;
        current = current->next;
    }

    enqueue_thread(thread, thread_priority);
}

void reset_queues() {
    for (int i = 1; i < 3; i++) {
        while (first_ready_to_run_thread_list[i]) {
            thread_control_block_t *next_in_line =
                first_ready_to_run_thread_list[i]->next;
            enqueue_thread(first_ready_to_run_thread_list[i], 0);

            first_ready_to_run_thread_list[i] = next_in_line;
        }
        last_ready_to_run_thread_list[i] = NULL;
    }

    counter = 0;
}

void yield() {
    if (IRQ_disable_counter == 0) {
        printf("\nCRITICAL: Yielding without lock protection!\n");
    }

    thread_control_block_t *prev = get_current_thread();

    if (prev->state == THREAD_RUNNING) {
        prev->state = THREAD_READY;

        if (prev != get_idle_thread()) {
            enqueue_thread(prev, prev->priority);
        }
    }
    thread_control_block_t *next_thread = dequeue_thread();

    if (!next_thread) {
        next_thread = get_idle_thread();
    }

    next_thread->state = THREAD_RUNNING;
    set_current_thread(next_thread);

    switch_to_task(&prev->esp, next_thread->esp);
}

void block_task(thread_state new_state) {
    lock_scheduler();
    thread_control_block_t *current_thread = get_current_thread();
    current_thread->state = new_state;
    yield();
    unlock_scheduler();
}

void unblock_task(thread_control_block_t *thread) {
    lock_scheduler();
    thread->id++;
    // if (!first_ready_to_run_thread) {
    //     switch_to_task(&current_thread->esp, task->esp);
    // } else {
    //     last_ready_to_run_thread->next = task;
    //     last_ready_to_run_thread = task;
    // }
    unlock_scheduler();
}

void lock_scheduler() {
#ifndef SMP
    __asm__ volatile("cli");
    IRQ_disable_counter++;
#endif
}

void unlock_scheduler() {
#ifndef SMP
    IRQ_disable_counter--;
    if (IRQ_disable_counter == 0) {
        __asm__ volatile("sti");
    }
#endif
}

void scheduler_on_tick() {
    thread_control_block_t *current_thread = get_current_thread();

    uint8_t needs_yield = 0;

    if (current_thread == get_idle_thread()) {
        counter++;
        if (counter >= 100) {
            reset_queues();
            needs_yield = 1;
        }

        if (needs_yield) {
            lock_scheduler();
            yield();
            unlock_scheduler();
        }
        return;
    }

    current_thread->time_used += 1;
    current_thread->remaining_time -= 1;

    if (current_thread->remaining_time <= 0) {
        if (current_thread->priority >= 1) {
            current_thread->remaining_time = THIRD_LIMIT;
            current_thread->priority = 2;
        } else {
            current_thread->remaining_time = SECOND_LIMIT;
            current_thread->priority++;
        }
        needs_yield = 1;
    }
    counter++;

    if (counter >= 100) {
        reset_queues();
        needs_yield = 1;
    }

    if (needs_yield) {
        lock_scheduler();
        yield();
        unlock_scheduler();
    }
}