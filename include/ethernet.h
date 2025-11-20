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
    ETHERTYPE_IPV4      = 0x0800, // IPv4
    ETHERTYPE_ARP       = 0x0806, // ARP
    ETHERTYPE_WOL       = 0x0842, // Wake-on-LAN (Magic Packet)
    ETHERTYPE_RARP      = 0x8035, // Reverse ARP
    ETHERTYPE_8021Q     = 0x8100, // 802.1Q VLAN-tagged frames
    ETHERTYPE_MPLS_UC   = 0x8847, // MPLS unicast
    ETHERTYPE_MPLS_MC   = 0x8848, // MPLS multicast
    ETHERTYPE_PPPOE_DISC= 0x8863, // PPPoE Discovery
    ETHERTYPE_PPPOE     = 0x8864, // PPPoE Session
    ETHERTYPE_EAPOL     = 0x888E, // EAP over LAN
    ETHERTYPE_LLDP      = 0x88CC, // LLDP
    ETHERTYPE_IPV6      = 0x86DD  // IPv6
} ethertype_t;

void print_mac_address(const uint8_t* mac_address); //  6 byte array pointer
void mac_address_to_string(char* buf, size_t buf_size, const uint8_t mac_address);
void process_ethernet_header(const unsigned char* packet_data, int data_length); 

#endif // ETHERNET_H