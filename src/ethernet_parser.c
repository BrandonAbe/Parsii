#include <stdio.h>
#include <../include/utils.h>
#include <../include/ethernet.h>
#include <arpa/inet.h> // For ntohs, to convert network byte order to host byte order
#include <../include/ip.h>

void mac_address_to_string(char* buf, size_t buf_size, const uint8_t* mac_address){
    snprintf(buf, buf_size, "%02X:%02X:%02X:%02X:%02X:%02X",                                                                                                                                                                                                                     
        mac_address[0], mac_address[1], mac_address[2],                                                                                                                                                                                                                       
        mac_address[3], mac_address[4], mac_address[5]);                                                                                                                                                                                                                      
}                                                                

void print_mac_address(const uint8_t* mac_address){
    char mac_str[18]; //18 chars for address +1 null terminator
    mac_address_to_string(mac_str, sizeof(mac_str), mac_address);
    printf("%s", mac_str);
}

void process_ethernet_header(const unsigned char* packet_data, int data_length){
    if (data_length < (int)sizeof(ethernet_header_t)){
        fprintf(stderr, 
            "Error: Incomplete Ethernet header. Packet is too small (got %d bytes, requires at least %zu bytes).\n",
            data_length,
            sizeof(ethernet_header_t));    
        return; //Stop processing of invalid packet
    }

    /* Treat the first bytes of packet_data as an ethernet_header_t */
    const ethernet_header_t* eth_header = (const ethernet_header_t*)packet_data;

    /* Buffers to hold formatted MAC address strings */
    char dest_mac_str[18];
    char src_mac_str[18];

    /* Format MAC addresses into buffers */
    mac_address_to_string(dest_mac_str, sizeof(dest_mac_str), eth_header->dest_mac);
    mac_address_to_string(src_mac_str, sizeof(src_mac_str), eth_header->src_mac);


    /* Extracts and converts the EtherType field from the Ethernet frame header to host byte order */
    uint16_t ethertype = ntohs(eth_header->ethertype);

    /* Print Ethernet header data to Stdout */
    printf("Ethernet Header:\n");
    printf("Destination MAC: %s\n", dest_mac_str);    
    printf("Source MAC: %s\n", src_mac_str);
    printf("EtherType: 0x%04X (",(unsigned int)ethertype); //Cast to unsigned int because ethertype is uint16_t; raw interpretation (verbose)


    switch(ethertype){ // Print human-readable ethernet type
        case ETHERTYPE_IPV4:
            printf("IPv4\n");
            process_ipv4_header(packet_data + sizeof(ethernet_header_t), data_length - sizeof(ethernet_header_t));
            break;
        case ETHERTYPE_IPV6:
            printf("IPv6\n");
            //process_ipv6_header(packet_data + sizeof(ethernet_header_t), data_length - sizeof(ethernet_header_t));
            // commented out to focus on IPv4
            break;
        case ETHERTYPE_ARP:
            printf("ARP\n");
            //process_arp_header(packet_data + sizeof(ethernet_header_t), data_length - sizeof(ethernet_header_t));
            // commented out to focus on IPv4 and IPv6
            break;
        default:
            printf("ERROR: Unsupported EtherType)\n");
            break;
    }

}