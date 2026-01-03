#include <stdio.h>
#include <string.h>
#include "../include/ip.h"
#include "../include/utils.h"

void ipv4_to_string(uint32_t ip_raw, char* ip_str){
    // Assume ip_raw is in the network byte order (big-endian)
    snprintf(ip_str, 16, "%u.%u.%u.%u",
        (ip_raw >> 0) & 0xFF, // right-shift by 0 bits to get the last octet, mask with 0xFF
        (ip_raw >> 8) & 0xFF, 
        (ip_raw >> 16) & 0xFF, 
        (ip_raw >> 24) & 0xFF);  
}

void ipv6_to_string(const uint8_t* ip_raw, char* ip_str){
    snprintf(ip_str, 40, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
        ip_raw[0], ip_raw[1], ip_raw[2], ip_raw[3],
        ip_raw[4], ip_raw[5], ip_raw[6], ip_raw[7],
        ip_raw[8], ip_raw[9], ip_raw[10], ip_raw[11],
        ip_raw[12], ip_raw[13], ip_raw[14], ip_raw[15]);
}

void process_ipv4_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length){
    (void)file_ctx; //Cast to void to avoid unused parameter warning
    if (data_length < 20){ // Minimum IPv4 header size (in bytes)
        fprintf(stderr, "Error: Incomplete IPv4 header. Patcket is too small.\n");
        return; // If error, do nothing
    }
    
    ipv4_header_t ip_header;
    memcpy(&ip_header, packet_data, sizeof(ipv4_header_t));
    uint16_t total_length = ip_header.total_length;
    if(is_little_endian()){
        total_length = swap_uint16(total_length);
    }
    printf("    IPv4 Header len: %u\n", total_length);

}

void process_ipv6_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length){
    (void)file_ctx; //Cast to void to avoid unused parameter warning
    (void)packet_data; //Cast to void to avoid unused parameter warning
    if (data_length < 40){ // Minimum IPv6 header size (in bytes)
        fprintf(stderr, "Error: Incomplete IPv6 header. Patcket is too small.\n");
        return; // If error, do nothing
    }
    printf("    IPv6 Header: (Placeholder for IPv6 parsing)\n");
}

void parse_network_layer_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, uint16_t ethertype) {
    switch (ethertype) {
        case ETHERTYPE_IPV4:
            process_ipv4_header(file_ctx, packet_data, data_length);
            break;
        case ETHERTYPE_IPV6:
            process_ipv6_header(file_ctx, packet_data, data_length);
            break;
        case ETHERTYPE_ARP:
            // process_arp_header(file_ctx, packet_data, data_length);
            printf("ARP packet, not parsing yet.\n");
            break;
        case ETHERTYPE_PTP:
            // process_ptp_header(file_ctx, packet_data, data_length);
            printf("PTP packet, not parsing yet.\n");
            break;
        default:
            printf("Unsupported network layer protocol: 0x%04X\n", ethertype);
            break;
    }
}