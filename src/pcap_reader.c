/* The Plan */
//
// 1. Read pcap file header
// 2. Validate pcap file header
// 3. Read each pcap record header and data
// 4. Process each packet data as needed
// 5. Handle end-of-file and cleanup
// 6. Return success or error codes

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/pcap.h"
#include "../include/utils.h"

/* Function to read and process a pcap file */
int process_pcap_file(const char* filepath){
    // Open the PCAP file in binary read mode
    FILE* file = fopen(filepath, "rb"); 
    if (file == NULL) {
        perror("Error opening file");
        return -1; 
    }

    printf("File opened successfully: %s\n", filepath);

    /* Read global header */
    pcap_global_header_t global_header;
    // Read the global header from the file
    size_t bytes_read = fread(&global_header, sizeof(pcap_global_header_t), 1, file);
    if (bytes_read != 1) {
        perror("Error reading global header");
        fclose(file);
        return -1; 
    }
    printf("PCAP Global Header read successfully.\n");

    /* Check magic number to determine endianness */
    int swap_bytes = 0; // Flag to indicate if byte swapping is needed
    if (global_header.magic_number == 0xa1b2c3d4) {
        // Magic number for native byte order
        printf("PCAP file is in native byte order (no byte swapping needed)\n");
    } else if (global_header.magic_number == 0xd4c3b2a1) {
        // Magic number for swapped byte order
        printf("PCAP file is in swapped byte order (byte swapping needed)\n");
        swap_bytes = 1;
    } else {
        // Invalid magic number
        fprintf(stderr, "Error: Not a valid PCAP file (invalid magic number).\n");
        fclose(file);
        return -1;
    }

    // If byte swapping is needed, swap the fields in the global header
    if (swap_bytes) {
        global_header.version_major = swap_uint16(global_header.version_major);
        global_header.version_minor = swap_uint16(global_header.version_minor);
        global_header.thiszone = swap_uint32(global_header.thiszone);
        global_header.sigfigs = swap_uint32(global_header.sigfigs);
        global_header.snaplen = swap_uint32(global_header.snaplen);
        global_header.network = swap_uint32(global_header.network);
    }

    // Print the global header information
    printf("PCAP Version: %u.%u\n", global_header.version_major, global_header.version_minor);
    printf("Snapshot Length: %u\n", global_header.snaplen);
    printf("Link-Layer Header Type: %u\n", global_header.network);

    /* Loop through all packets in the file */
    pcap_record_header_t record_header;
    int packet_count = 0;
    // Read packet records until the end of the file
    while(fread(&record_header, sizeof(pcap_record_header_t), 1, file) == 1) {
        packet_count++;
        printf("\n--- Packet %d ---\n", packet_count);

        // If byte swapping is needed, swap the fields in the record header
        if (swap_bytes) {
            record_header.ts_sec = swap_uint32(record_header.ts_sec);
            record_header.ts_usec = swap_uint32(record_header.ts_usec);
            record_header.incl_len = swap_uint32(record_header.incl_len);
            record_header.orig_len = swap_uint32(record_header.orig_len);
        }

        // Print the record header information
        printf("Timestamp (Seconds): %u\n", record_header.ts_sec);
        printf("Timestamp (Microseconds): %u\n", record_header.ts_usec);
        printf("Included Length: %u\n", record_header.incl_len);
        printf("Original Length: %u\n", record_header.orig_len);

        // Allocate memory for the packet data based on the included length
        unsigned char* packet_data = (unsigned char*)malloc(record_header.incl_len);
        if (packet_data == NULL) {
            perror("Failed to allocate memory for packet data");
            break; // Exit the loop if memory allocation fails
        }

        // Read the packet data from the file
        size_t data_bytes_read = fread(packet_data, 1, record_header.incl_len, file);
        if (data_bytes_read != record_header.incl_len) {
            perror("Error reading packet data");
            free(packet_data);
            break; // Exit the loop if the packet data cannot be fully read
        }

        // TODO: Process the packet data (e.g., parse Ethernet, IP, TCP/UDP headers)
        printf("Packet data read successfully (%zu bytes).\n", data_bytes_read);


        // Free the allocated memory for the packet data
        free(packet_data);
    }

    printf("\nTotal packets processed: %d\n", packet_count);

    // Close the file
    fclose(file);
    return 0; //File processed successfully
}