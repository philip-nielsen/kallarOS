#ifndef STDIO_H
#define STDIO_H

#include "drivers/vga.h"

int puts(const char* chars);
int putchar(int ch);
int printf(const char* format, ...);

#endif

#ifndef EOF
# define EOF (-1)
#endif