#ifndef STDIO_H
#define STDIO_H

int puts(const char* chars);
int putchar(int ch);
int printf(const char* format, ...);

#endif

#ifndef EOF
# define EOF (-1)
#endif