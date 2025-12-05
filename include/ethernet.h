#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdio.h>

/* Ethernet II header structure (14 bytes) */
typedef struct {
    uint8_t  dest_mac[6];   // Destination MAC address
    uint8_t  src_mac[6];    // Source MAC address
    uint16_t ethertype;     // Ethertype field
} ethernet_header_t;

typedef enum {
    ETHERTYPE_IPV4      = 0x0800, // IP Internet Protocol version 4 (IPv4)
    ETHERTYPE_ARP       = 0x0806, // ARP
    ETHERTYPE_VLAN      = 0x8100, // VLAN-tagged frame (IEEE 802.1Q)
    ETHERTYPE_IPV6      = 0x86DD  // IP Internet Protocol version 6 (IPv6)
    ETHERTYPE_PTP       = 0x88F7  // Precision Time Protocol
} ethertype_t;

void print_mac_address(const uint8_t* mac_address); //  6 byte array pointer
void mac_address_to_string(char* buf, size_t buf_size, const uint8_t* mac_address);
void process_ethernet_header(const unsigned char* packet_data, int data_length); 

#endif // ETHERNET_H