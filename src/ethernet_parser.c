#include <stdio.h>
#include "../include/utils.h"
#include "../include/ethernet.h"
#include "../include/ip.h"
#include "../include/pcap.h"


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

void process_ethernet_header(file_context_t* file_ctx, const unsigned char* packet_data, int data_length){
    if (data_length < (int)sizeof(ethernet_header_t)){
        fprintf(stderr, 
            "Error: Incomplete Ethernet header. Packet is too small (got %d bytes, requires at least %zu bytes).\n",
            data_length,
            sizeof(ethernet_header_t));    
        return; //Stop processing of invalid packet
    }

    /* Treat the first bytes of packet_data as an ethernet_header_t */
    const ethernet_header_t* eth_header = (const ethernet_header_t*)packet_data;

    /* Use context-aware swap function if needed */
    uint16_t ethertype = eth_header->ethertype;
    if (is_little_endian()) {
        ethertype = swap_uint16(ethertype);
    }


    /* Print Ethernet header data to Stdout */
    printf("Ethernet Header:\n");
    printf(" Destination MAC: ");
    print_mac_address(eth_header->dest_mac);    
    printf("\n Source MAC: ");
    print_mac_address(eth_header->src_mac);
    printf("EtherType: 0x%04X\n", ethertype);

    /* Define pointers for next layer */
    const uint8_t* network_layer_data = packet_data + sizeof(ethernet_header_t);
    uint32_t network_layer_length = data_length - sizeof(ethernet_header_t);

    if (ethertype == ETHERTYPE_VLAN) {
        if (network_layer_length < sizeof(vlan_header_t)) {
            fprintf(stderr, "Incomplete VLAN header\n");
            return;
        }
        const vlan_header_t* vlan_header = (const vlan_header_t*)network_layer_data;
        uint16_t tci = vlan_header->tci;
        if (is_little_endian()) {
            tci = swap_uint16(tci);
        }
        uint16_t vlan_id = tci & 0x0FFF;
        uint16_t vlan_pcp = tci >> 13; // Extract the 3-bit Priority Code Point

        ethertype = vlan_header->ethertype;
        if (is_little_endian()) {
            ethertype = swap_uint16(ethertype);
        }
        printf("VLAN ID: %u\n", vlan_id);
        printf("VLAN Priority: %u\n", vlan_pcp);
        printf("Encapsulated EtherType: 0x%04X\n", ethertype);
        network_layer_data += sizeof(vlan_header_t);
        network_layer_length -= sizeof(vlan_header_t);
    }

    /* Hand off to network layer parser */
    parse_network_layer_header(file_ctx, network_layer_data, network_layer_length, ethertype);
}