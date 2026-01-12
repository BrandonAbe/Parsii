#include <stdint.h>
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define PCAP_MAGIC_NUMBER 0xa1b2c3d4
#define PCAP_SWAPPED_MAGIC_NUMBER 0xd4c3b2a1

/* Macros to handle byte swapping based on file context */
#define BYTES_TO_UINT16(data_ptr, swap_bytes_flag) ( (swap_bytes_flag) ? swap_uint16(*(uint16_t*)(data_ptr)) : *(uint16_t*)(data_ptr) )
#define BYTES_TO_UINT32(data_ptr, swap_bytes_flag) ( (swap_bytes_flag) ? swap_uint32(*(uint32_t*)(data_ptr)) : *(uint32_t*)(data_ptr) )


/* Define common values */
#define MAC_ADDR_LEN 6
#define ETHERNET_HEADER_LEN 14

/* Functions to swap values */
uint16_t swap_uint16(uint16_t val);
uint32_t swap_uint32(uint32_t val);
uint64_t swap_uint64(uint64_t val);
uint64_t bytes_to_uint48(const uint8_t* bytes, int swap_bytes);
int is_little_endian();

#endif // UTILS_H