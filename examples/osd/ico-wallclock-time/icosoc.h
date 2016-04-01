// #### This file is auto-generated from icosoc.py. Do not edit! ####


#ifndef ICOSOC_H
#define ICOSOC_H

#define ICOSOC_CLOCK_FREQ_HZ 6000000

void icosoc_ser0_read(void *data, int len);
void icosoc_ser0_write(const void *data, int len);
int icosoc_ser0_read_nb(void *data, int maxlen);
int icosoc_ser0_write_nb(const void *data, int maxlen);

#include <stdint.h>

static inline void icosoc_leds_set(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000000 + 2 * 0x10000) = bitmask;
}

static inline uint32_t icosoc_leds_get() {
    return *(volatile uint32_t*)(0x20000000 + 2 * 0x10000);
}

static inline void icosoc_leds_dir(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000004 + 2 * 0x10000) = bitmask;
}


#endif /* ICOSOC_H */

