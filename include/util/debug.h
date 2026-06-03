#include <libc/stdio.h>

/*
 * Define PRINT 1 on top of your source, to print using pr.
 * For example pr("test\n");
 * If PRINT is missing or set to 0, then nothing is printed
 */
#ifndef PRINT
#define PRINT 0
#endif

/* From Jonas Skeppstedt, best teacher :)
 * the funny do-while next clearly performs one iteration of the loop.
 * if you are really curious about why there is a loop, please check
 * the course book (Writing Efficient C Code) about the C preprocessor where it
 * is explained. it is to avoid bugs and/or syntax errors in case you use the pr
 * in an if-statement without { }.
 *
 */
#if PRINT
#define pr(...)                                                                \
    do {                                                                       \
        printf(__VA_ARGS__);                                                   \
    } while (0)
#else
#define pr(...) /* no effect at all */
#endif