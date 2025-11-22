#include <stdio.h>
#include <../include/ip.h>

void process_ipv4_header(const unsigned char* packet_data, int data_length){
    if (data_length < 20){ // Minimum IPv4 header size (in bytes)
        fprintf(stderr, "Error: Incomplete IPv4 header. Patcket is too small.\n");
        return; // If error, do nothing
    }
    printf("    IPv4 Header: (Placeholder for IPv4 parsing)\n");
}

void process_ipv6_header(const unsigned char* packet_data, int data_length){
    if (data_length < 40){ // Minimum IPv6 header size (in bytes)
        fprintf(stderr, "Error: Incomplete IPv6 header. Patcket is too small.\n");
        return; // If error, do nothing
    }
    printf("    IPv6 Header: (Placeholder for IPv4\6 parsing)\n");
}