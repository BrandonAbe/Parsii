#ifndef IP_H
#define IP_H

#include <stdint.h>
#include <stdio.h>

void process_ipv4_header(const unsigned char* packet_data, int data_length);
void process_ipv6_header(const unsigned char* packet_data, int data_length);
void process_arp_header(const unsigned char* packet_data, int data_length);

#endif // IP_H