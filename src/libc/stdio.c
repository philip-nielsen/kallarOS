#include <libc/stdio.h>
#include <drivers/vga.h>
#include <stdarg.h>

static int print_string(const char* chars) {
    int i = 0;
    while (chars[i] != '\0') {
        if( putchar(chars[i]) == EOF) { 
            return EOF;
        }
        i++;
    }
    return i; // Returns number of characters printed
}

static void print_hex(unsigned int n) {
    if (n == 0) {
        print_string("0x0");
        return;
    }

    char buffer[11]; // "0x" + 8 hex digits + null terminator
    buffer[0] = '0';
    buffer[1] = 'x';
    
    int i = 9;
    buffer[10] = '\0';

    while (n > 0 && i >= 2) {
        int remainder = n % 16;
        if (remainder < 10) {
            buffer[i] = remainder + '0';
        } else {
            buffer[i] = (remainder - 10) + 'A';
        }
        n /= 16;
        i--;
    }

    print_string(&buffer[i + 1]);
}

static void print_int(int n) {
    if (n == 0) {
        print_string("0");
        return;
    }

    char buffer[12]; //Max length of a 32-bit int + sign + null terminator
    int i = 0;
    int is_negative = 0;

    unsigned int num = n;
    if (n < 0) {
        is_negative = 1;
        num = -n;
    }

    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse back number string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--; 
    }

    print_string(buffer);
}

int putchar(int ic) {
    vga_putchar((char)ic);
    return ic;
}

int puts(const char* chars) {
    int count = print_string(chars);
    putchar('\n');
    return count + 1; 
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] != '%') {
            putchar(format[i]);
            continue;
        }

        i++; 
        
        switch (format[i]) {
            case 'd': { // Integer
                int num = va_arg(args, int);
                print_int(num);
                break;
            }
            case 'x': { // Hexadecimal
                unsigned int num = va_arg(args, unsigned int);
                print_hex(num);
                break;
            }
            case 's': { // String
                char* str = va_arg(args, char*);
                print_string(str);
                break;
            }
            case 'c': { // Character
                char c = (char)va_arg(args, int);
                putchar(c);
                break;
            }
            case '%': { // Escaped '%'
                putchar('%');
                break;
            }
            case '\0': { // String ended abruptly after a '%'
                return 0; 
            }
            default: { // Unknown specifier (e.g., %z), puts it as-is
                putchar('%');
                putchar(format[i]);
                break;
            }
        }
    }

    va_end(args);
    return 0;
}