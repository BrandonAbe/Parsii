#include "../include/utils.h"

uint16_t swap_uint16(uint16_t val) {
    return __builtin_bswap16(val);
}

uint32_t swap_uint32(uint32_t val) {
    return __builtin_bswap32(val);
}