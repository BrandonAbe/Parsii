#ifndef IP_H
#define IP_H

#include <stdint.h>
#include <stdio.h>


/* Define IPv4 Header struct based on documentation of */
typedef struct {
    uint8_t version_ihl;        // Version (4 bits) + Internet Header Length (4 bits)
    uint8_t type_of_service;    // Type of Service
    uint16_t total_length;      // Total Length
    uint16_t identification;    // Identification
    uint16_t flags_fragment;    // Flags (3 bits) + Fragment Offset (13 bits)
    uint8_t ttl;                // Time to Live
    uint8_t protocol;           // Protocol (e.g., 17 for UDP)
    uint16_t checksum;          // Header Checksum
    uint32_t src_ip;            // Source IP Address
    uint32_t dst_ip;            // Destination IP Address
} ipv4_header_t;


/* Define IPv6 Header struct based on documentation of */
typedef struct {
    uint32_t version_tc_flowlabel; // Version (4 bits) + Traffic Class (8 bits) + Flow Label (20 bits)
    uint16_t payload_length;       // Payload Length
    uint8_t next_header;           // Next Header
    uint8_t hop_limit;             // Hop Limit
    uint8_t src_ip[16];            // Source IP Address
    uint8_t dst_ip[16];            // Destination IP Address
} ipv6_header_t;

/* Define ARP Header struct based on documentation of */
typedef struct {
    uint16_t htype;           // Hardware Type
    uint16_t ptype;           // Protocol Type
    uint8_t hlen;             // Hardware Address Length
    uint8_t plen;             // Protocol Address Length
    uint16_t oper;            // Operation
    uint8_t sha[6];          // Sender Hardware Address
    uint8_t spa[4];          // Sender Protocol Address
    uint8_t tha[6];          // Target Hardware Address
    uint8_t tpa[4];          // Target Protocol Address
} arp_header_t;

typedef struct {
    uint16_t src_port;    // Source Port
    uint16_t dst_port;    // Destination Port
    uint16_t length;      // Length
    uint16_t checksum;    // Checksum
} udp_header_t;


/* Function prototypes for processing headers */
void process_ipv4_header(const unsigned char* packet_data, int data_length);
void process_ipv6_header(const unsigned char* packet_data, int data_length);
void process_arp_header(const unsigned char* packet_data, int data_length);

#endif // IP_H