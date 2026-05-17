#ifndef PANIC_H
#define PANIC_H

/**
 * panic() - Triggers a fatal kernel error and halts the system.
 * @message: The specific error message explaining the reason for the panic.
 *
 * This function permanently halts OS execution. It changes the VGA
 * framebuffer to a red "Screen of Death", prints the provided error
 * message, and traps the CPU in an infinite loop with hardware interrupts
 * disabled (cli; hlt).
 */
__attribute__((noreturn)) void panic(const char *message);

#endif