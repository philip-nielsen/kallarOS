#ifndef INCLUDE_PIT_H
#define INCLUDE_PIT_H

#include <stdint.h>

void pit_prepare_sleep(uint32_t microseconds);

void pit_perform_sleep(void);

#endif