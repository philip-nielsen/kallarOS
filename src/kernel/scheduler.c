#include <stdint.h>
#define PRINT 0

#include <arch/i386/apic.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <libc/stdio.h>
#include <stddef.h>
#include <util/debug.h>

static uint8_t IRQ_disable_counter;
static volatile uint8_t counter;

static uint8_t postponed_yield_flag;
static uint8_t postpone_switches_counter;

// The MLFQ queues
static thread_control_block_t *first_ready_to_run_thread_list[3];
static thread_control_block_t *last_ready_to_run_thread_list[3];

static thread_control_block_t *sleeping_thread_queue;
static thread_control_block_t *terminated_thread_queue;

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

void reset_queues() {
    for (int i = 1; i < 3; i++) {
        while (first_ready_to_run_thread_list[i]) {
            thread_control_block_t *next_in_line =
                first_ready_to_run_thread_list[i]->next;
            first_ready_to_run_thread_list[i]->remaining_time = FIRST_LIMIT;
            pr("\nMoving back thread %d to top\n",
               first_ready_to_run_thread_list[i]->id);
            enqueue_thread(first_ready_to_run_thread_list[i], 0);

            first_ready_to_run_thread_list[i] = next_in_line;
        }
        last_ready_to_run_thread_list[i] = NULL;
    }

    counter = 0;
}

void yield() {
    if (postpone_switches_counter > 0) {
        postponed_yield_flag = 1;
        return;
    }

    if (IRQ_disable_counter == 0) {
        pr("\nYielding without lock protection!\n");
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

    if (prev == next_thread) {
        prev->state = THREAD_RUNNING;
        return;
    }

    next_thread->state = THREAD_RUNNING;
    set_current_thread(next_thread);

    switch_to_task(&prev->esp, next_thread->esp);
}

void lock_scheduler() {
#ifndef SMP
    __asm__ volatile("cli");
    IRQ_disable_counter++;
#endif
}

void unlock_scheduler() {
#ifndef SMP
    if (IRQ_disable_counter == 0) {
        printf("unlock_scheduler underflow\n");
        return;
    }
    IRQ_disable_counter--;
    if (IRQ_disable_counter == 0) {
        __asm__ volatile("sti");
    }
#endif
}

void lock_atomic() {
    lock_scheduler();
    postpone_switches_counter++;
}

void unlock_atomic() {
    postpone_switches_counter--;
    if (postpone_switches_counter == 0 && postponed_yield_flag == 1) {
        postponed_yield_flag = 0;
        yield();
    }
    unlock_scheduler();
}

void scheduler_on_tick() {
    thread_control_block_t *current_sleep_head = sleeping_thread_queue;
    thread_control_block_t *prev_sleep_head = NULL;

    while (current_sleep_head) {
        thread_control_block_t *temp_sleep_head = current_sleep_head->next;
        if (current_sleep_head->wake_time <= apic_get_ticks()) {
            current_sleep_head->state = THREAD_READY;
            if (!prev_sleep_head) {
                sleeping_thread_queue = sleeping_thread_queue->next;
            } else {
                prev_sleep_head->next = current_sleep_head->next;
            }
            enqueue_thread(current_sleep_head, 0);
        } else {
            prev_sleep_head = current_sleep_head;
        }
        current_sleep_head = temp_sleep_head;
    }

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
        pr("\nmoving thread %d, to priority to %d\n", current_thread->id,
           current_thread->priority);
        needs_yield = 1;
    }
    counter++;

    if (counter >= 100) {
        pr("\nreset\n");
        reset_queues();
        needs_yield = 1;

        if (current_thread != get_idle_thread()) {
            current_thread->priority = 0;
            current_thread->remaining_time = FIRST_LIMIT;
        }
    }

    if (needs_yield) {
        lock_scheduler();
        yield();
        unlock_scheduler();
    }
}

void thread_sleep(thread_control_block_t *thread, uint32_t requested_time) {
    lock_scheduler();
    thread->next = NULL;
    thread->wake_time = apic_get_ticks() + requested_time;
    thread->state = THREAD_SLEEPING;

    if (!sleeping_thread_queue) {
        sleeping_thread_queue = thread;
    } else {
        thread_control_block_t *current_sleep_head = sleeping_thread_queue;

        while (current_sleep_head->next) {
            current_sleep_head = current_sleep_head->next;
        }

        current_sleep_head->next = thread;
    }
    yield();
    unlock_scheduler();
}

void thread_exit() {
    lock_scheduler();
    thread_control_block_t *thread = get_current_thread();
    thread->next = NULL;
    thread->state = THREAD_TERMINATED;

    if (!terminated_thread_queue) {
        terminated_thread_queue = thread;
    } else {
        thread_control_block_t *current_terminated_head =
            terminated_thread_queue;

        while (current_terminated_head->next) {
            current_terminated_head = current_terminated_head->next;
        }

        current_terminated_head->next = thread;
    }
    yield();
    unlock_scheduler();
}

thread_control_block_t *get_terminated_thread_queue() {
    return terminated_thread_queue;
}

void set_terminated_thread_queue(thread_control_block_t *new_queue) {
    terminated_thread_queue = new_queue;
}