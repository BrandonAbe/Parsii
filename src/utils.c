#include "../include/utils.h"

uint16_t swap_uint16(uint16_t val) {
    return __builtin_bswap16(val);
}

uint32_t swap_uint32(uint32_t val) {
    return __builtin_bswap32(val);
}

uint64_t swap_uint64(uint64_t val) {
    return __builtin_bswap64(val);
}

// Function to convert 6 bytes to a 48-bit unsigned integer, handling endianness
uint64_t bytes_to_uint48(const uint8_t* bytes, int swap_bytes) {
    uint64_t result = 0;
    if (swap_bytes) {
        result = ((uint64_t)bytes[5] << 40) |
                 ((uint64_t)bytes[4] << 32) |
                 ((uint64_t)bytes[3] << 24) |
                 ((uint64_t)bytes[2] << 16) |
                 ((uint64_t)bytes[1] << 8)  |
                 ((uint64_t)bytes[0] << 0);
    } else {
        result = ((uint64_t)bytes[0] << 40) |
                 ((uint64_t)bytes[1] << 32) |
                 ((uint64_t)bytes[2] << 24) |
                 ((uint64_t)bytes[3] << 16) |
                 ((uint64_t)bytes[4] << 8)  |
                 ((uint64_t)bytes[5] << 0);
    }
    return result;
}

int is_little_endian() {
    volatile uint32_t i = 0x01234567;
    // return 1 if the first byte (lowest address) is the LSB
    return (*(volatile uint8_t*)(&i) == 0x67);
}