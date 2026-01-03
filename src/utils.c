#include "../include/utils.h"

uint16_t swap_uint16(uint16_t val) {
    return __builtin_bswap16(val);
}

uint32_t swap_uint32(uint32_t val) {
    return __builtin_bswap32(val);
}

int is_little_endian() {
    volatile uint32_t i = 0x01234567;
    // return 1 if the first byte (lowest address) is the LSB
    return (*(volatile uint8_t*)(&i) == 0x67);
}