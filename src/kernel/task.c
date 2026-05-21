#include <kernel/kmalloc.h>
#include <kernel/task.h>
#include <stdint.h>

static thread_control_block_t *current_thread;

void initialize_multitasking() {
    thread_control_block_t *root_thread =
        (thread_control_block_t *)kmalloc(sizeof(thread_control_block_t));
    root_thread->esp = 0;
    root_thread->esp0 = 0;
    root_thread->state = THREAD_RUNNING;
    root_thread->id = 0;
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
    stack[4] = 0x202;                 // popfd
    stack[5] = (uint32_t)entry_point; // ret

    new_task->esp = (void *)stack;
    new_task->stack_base = (void *)stack_base;
    new_task->state = THREAD_READY;

    return new_task;
    // Todo, add to MLFQ queue
}

thread_control_block_t *get_current_thread() { return current_thread; }

void set_current_thread(thread_control_block_t *new_thread) {
    current_thread = new_thread;
}