#ifndef KEYBOARD_H
#define KEYBOARD_H

/**
 * keyboard_handler() - Processes a keyboard interrupt and prints the character.
 *
 * Reads the hardware scancode directly from the PS/2 controller on I/O port
 * 0x60, maps it to the US keyboard layout, and prints the resulting ASCII
 * character to the VGA buffer. Expected to be called by the IRQ 1 handler.
 */
void keyboard_handler(void);

#endif