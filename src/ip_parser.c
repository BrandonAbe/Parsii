#include <stdio.h>
#include <string.h>
#include "../include/ip.h"
#include "../include/utils.h"
#include "../include/ptp.h"

/**
 * @brief Converts a raw IPv4 address (uint32_t) to a human-readable string.
 * @param ip_raw The raw IPv4 address in network byte order.
 * @param ip_str Output buffer for the IP string.
 */
void ipv4_to_string(uint32_t ip_raw, char* ip_str){
    snprintf(ip_str, 16, "%u.%u.%u.%u",
        (ip_raw >> 0) & 0xFF, 
        (ip_raw >> 8) & 0xFF, 
        (ip_raw >> 16) & 0xFF, 
        (ip_raw >> 24) & 0xFF);  
}

/**
 * @brief Converts a raw IPv6 address (uint8_t array) to a human-readable string.
 * @param ip_raw The raw IPv6 address.
 * @param ip_str Output buffer for the IP string.
 */
void ipv6_to_string(const uint8_t* ip_raw, char* ip_str){
    snprintf(ip_str, 40, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
        ip_raw[0], ip_raw[1], ip_raw[2], ip_raw[3],
        ip_raw[4], ip_raw[5], ip_raw[6], ip_raw[7],
        ip_raw[8], ip_raw[9], ip_raw[10], ip_raw[11],
        ip_raw[12], ip_raw[13], ip_raw[14], ip_raw[15]);
}

/**
 * @brief Processes a UDP header and dispatches to PTP header processing if applicable.
 * @param file_ctx File context for byte swapping.
 * @param packet_data Pointer to the UDP packet data.
 * @param data_length Length of the UDP packet data.
 * @param eth_src_mac Source MAC address.
 * @param eth_dst_mac Destination MAC address.
 */
void process_udp_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac) {
    if (data_length < sizeof(udp_header_t)) {
        fprintf(stderr, "Incomplete UDP header\n");
        return;
    }
    const udp_header_t* udp_header = (const udp_header_t*)packet_data;
    uint16_t dst_port = udp_header->dst_port;
    if (file_ctx->swap_bytes) { 
        dst_port = swap_uint16(dst_port);
    }

    if (dst_port == PTP_EVENT_PORT || dst_port == PTP_GENERAL_PORT) {
        printf("PTP packet found over UDP\n");
        const uint8_t* ptp_data = packet_data + sizeof(udp_header_t);
        uint32_t ptp_length = data_length - sizeof(udp_header_t);
        process_ptp_header(file_ctx, ptp_data, ptp_length, eth_src_mac, eth_dst_mac);
    }
}

/**
 * @brief Processes an IPv4 header.
 * @param file_ctx File context for byte swapping.
 * @param packet_data Pointer to the IPv4 packet data.
 * @param data_length Length of the IPv4 packet data.
 * @param eth_src_mac Source MAC address.
 * @param eth_dst_mac Destination MAC address.
 */
void process_ipv4_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac){
    if (data_length < 20){ 
        fprintf(stderr, "Error: Incomplete IPv4 header. Patcket is too small.\n");
        return; 
    }
    
    ipv4_header_t ip_header;
    memcpy(&ip_header, packet_data, sizeof(ipv4_header_t));
    uint16_t total_length = ip_header.total_length;
    if(file_ctx->swap_bytes){ 
        total_length = swap_uint16(total_length);
    }
    printf("    IPv4 Header len: %u\n", total_length);

    if (ip_header.protocol == 17) { 
        const uint8_t* udp_data = packet_data + sizeof(ipv4_header_t);
        uint32_t udp_length = data_length - sizeof(ipv4_header_t);
        process_udp_header(file_ctx, udp_data, udp_length, eth_src_mac, eth_dst_mac);
    }
}

/**
 * @brief Processes an IPv6 header.
 * @param file_ctx File context for byte swapping.
 * @param packet_data Pointer to the IPv6 packet data.
 * @param data_length Length of the IPv6 packet data.
 * @param eth_src_mac Source MAC address.
 * @param eth_dst_mac Destination MAC address.
 */
void process_ipv6_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac){
    if (data_length < 40){ 
        fprintf(stderr, "Error: Incomplete IPv6 header. Patcket is too small.\n");
        return; 
    }
    ipv6_header_t ip_header;
    memcpy(&ip_header, packet_data, sizeof(ipv6_header_t));

    uint32_t version_tc_flowlabel = ip_header.version_tc_flowlabel;
    if (file_ctx->swap_bytes) { 
        version_tc_flowlabel = swap_uint32(version_tc_flowlabel);
    }

    uint8_t version = (version_tc_flowlabel >> 28) & 0x0F;
    uint8_t traffic_class = (version_tc_flowlabel >> 20) & 0xFF;
    uint32_t flow_label = version_tc_flowlabel & 0x000FFFFF;

    uint16_t payload_length = ip_header.payload_length;
    if (file_ctx->swap_bytes) { 
        payload_length = swap_uint16(payload_length);
    }

    char src_ip_str[40];
    char dst_ip_str[40];
    ipv6_to_string(ip_header.src_ip, src_ip_str);
    ipv6_to_string(ip_header.dst_ip, dst_ip_str);

    printf("    IPv6 Header:\n");
    printf("        Version: %u\n", version);
    printf("        Traffic Class: %u\n", traffic_class);
    printf("        Flow Label: %u\n", flow_label);
    printf("        Payload Length: %u\n", payload_length);
    printf("        Next Header: %u\n", ip_header.next_header);
    printf("        Hop Limit: %u\n", ip_header.hop_limit);
    printf("        Source IP: %s\n", src_ip_str);
    printf("        Destination IP: %s\n", dst_ip_str);

    if (ip_header.next_header == 17) { 
        const uint8_t* udp_data = packet_data + sizeof(ipv6_header_t);
        uint32_t udp_length = data_length - sizeof(ipv6_header_t);
        process_udp_header(file_ctx, udp_data, udp_length, eth_src_mac, eth_dst_mac);
    }
}

/**
 * @brief Parses the network layer header based on EtherType.
 * @param file_ctx File context for byte swapping.
 * @param packet_data Pointer to the network layer packet data.
 * @param data_length Length of the network layer packet data.
 * @param ethertype EtherType to determine the network protocol.
 * @param eth_src_mac Source MAC address.
 * @param eth_dst_mac Destination MAC address.
 */
void parse_network_layer_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, uint16_t ethertype, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac) {
    switch (ethertype) {
        case ETHERTYPE_IPV4:
            process_ipv4_header(file_ctx, packet_data, data_length, eth_src_mac, eth_dst_mac);
            break;
        case ETHERTYPE_IPV6:
            process_ipv6_header(file_ctx, packet_data, data_length, eth_src_mac, eth_dst_mac);
            break;
        case ETHERTYPE_PTP:
            process_ptp_header(file_ctx, packet_data, data_length, eth_src_mac, eth_dst_mac);
            break;
        default:
            printf("Unsupported network layer protocol: 0x%04X\n", ethertype);
            break;
    }
}