#ifndef STDIO_H
#define STDIO_H

#ifndef EOF
#define EOF (-1)
#endif

/**
 * putchar() - Writes a character to the VGA framebuffer.
 * @ic: The character to write (passed as an int).
 *
 * Return: The character written.
 */
int putchar(int ic);

/**
 * puts() - Writes a string to the VGA framebuffer, appended with a newline.
 * @chars: The null-terminated string to print.
 *
 * Return: The total number of characters printed, including the newline.
 */
int puts(const char *chars);

/**
 * printf() - A stripped-down kernel implementation of formatted printing.
 * @format: The format string.
 * @...: Variadic arguments.
 *
 * WARNING: This is not a full POSIX printf. It currently ONLY supports:
 * * %d - Signed 32-bit integers
 * * %x - Hexadecimal integers (lowercase)
 * * %s - Null-terminated strings
 * * %c - Single characters
 * * %% - Literal percent sign
 * Does NOT support padding, width, floats, or long longs.
 *
 * Return: 0 (Note: Standard printf returns characters printed, this does not).
 */
int printf(const char *format, ...);

#endif