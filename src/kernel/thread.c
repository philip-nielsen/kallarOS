#include <arch/i386/apic.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <libc/stdio.h>
#include <stdint.h>

static thread_control_block_t *current_thread;
static thread_control_block_t *idle_thread;
static uint32_t id;

static void idle_thread_method() {
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

    idle_thread->esp = (void *)idle_stack;
    idle_thread->stack_base = (void *)idle_stack_base;
    idle_thread->state = THREAD_READY;
    idle_thread->time_used = 0;
    idle_thread->priority = 2;
    idle_thread->id = 255;
    idle_thread->remaining_time = 0;
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