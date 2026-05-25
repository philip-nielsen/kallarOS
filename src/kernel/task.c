#include <arch/i386/apic.h>
#include <kernel/kmalloc.h>
#include <kernel/task.h>
#include <kernel/timer.h>
#include <libc/stdio.h>
#include <stdint.h>

static thread_control_block_t *current_thread;
static uint32_t last_count;

void initialize_multitasking() {
    thread_control_block_t *root_thread =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));
    root_thread->esp = 0;
    root_thread->esp0 = 0;
    root_thread->state = THREAD_RUNNING;
    root_thread->id = 0;
    root_thread->time_used = 0;
    current_thread = root_thread;
}

thread_control_block_t *create_kernel_task(void (*entry_point)()) {
    thread_control_block_t *new_task =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));

    uint32_t stack_base = (uint32_t)kmalloc(4096);
    uint32_t *stack = (uint32_t *)(stack_base + 4096 - 24);

    stack[0] = 0;                     // pop edi
    stack[1] = 0;                     // pop esi
    stack[2] = 0;                     // pop ebx
    stack[3] = 0;                     // pop ebp
    stack[4] = 0x202;                 // popfd (EFLAGS with interrupts enabled)
    stack[5] = (uint32_t)entry_point; // ret

    new_task->esp = (void *)stack;
    new_task->stack_base = (void *)stack_base;
    new_task->state = THREAD_READY;
    new_task->time_used = 0;

    return new_task;
    // Todo, add to MLFQ queue
}

void yield() {
    thread_control_block_t *prev = get_current_thread();
    update_time_used();
    set_current_thread(prev->next);
    thread_control_block_t *current_thread = get_current_thread();
    switch_to_task(&prev->esp, current_thread->esp);
}

thread_control_block_t *get_current_thread() { return current_thread; }

void set_current_thread(thread_control_block_t *new_thread) {
    current_thread = new_thread;
}

void update_time_used() {
    uint32_t current_count = apic_get_ticks();
    uint32_t elapsed = current_count - last_count;
    last_count = current_count;
    current_thread->time_used += elapsed;
}