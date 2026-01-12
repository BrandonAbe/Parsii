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
#include "../include/ip.h"
#include "../include/utils.h"
#include "../include/ethernet.h"

/* Function to read and process a pcap file */
int process_pcap_file(const char* filepath, file_context_t* file_ctx){
    // Open the PCAP file in binary read mode
    FILE* file = fopen(filepath, "rb"); 
    if (file == NULL) {
        perror("Error opening file");
        return -1; 
    }

    printf("File opened successfully: %s\n", filepath);

    /* Read global header */
    pcap_global_header_t global_header;
    // Read the global header from the file, populating the global_header structure
    size_t bytes_read = fread(&global_header, sizeof(pcap_global_header_t), 1, file);
    if (bytes_read != 1) {
        perror("Error reading global header");
        fclose(file);
        return -1; 
    }
    printf("PCAP Global Header read successfully.\n");

    /* Check magic number to determine endianness */
    file_ctx->swap_bytes = 0; // Initialize to no swapping
    if (global_header.magic_number == 0xa1b2c3d4) {
        printf("PCAP file is in native byte order (no byte swapping needed)\n");
    } else if (global_header.magic_number == 0xd4c3b2a1) {
        printf("PCAP file is in swapped byte order (byte swapping needed)\n");
        file_ctx->swap_bytes = 1;
    } else {
        fprintf(stderr, "Error: Not a valid PCAP file (invalid magic number).\n");
        fclose(file);
        return -1;
    }

    /* If byte swapping is needed, swap the fields in the global header */
    if (file_ctx->swap_bytes) {
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

    /* Verify PCAP Version is at least 2. this will allow verification that PCAP standard from 1998 is met before we process the data. */
    if (global_header.version_major == 2) {
        printf("PCAP version is valid (>= 2.0)\n");
    } else {
        fprintf(stderr, "Error: Unsupported PCAP version %u.%u. Minimum required is 2.0.\n", global_header.version_major, global_header.version_minor);
        fclose(file);
        return -1;
    }

    /* Verify snapshot length, should be 0 by default */
    if (global_header.snaplen == 0) {
        printf("Snapshot length is valid (0 means no limit)\n");
    }
    else {
        printf("Warning: Snapshot length is set to %u. This may limit packet capture size.\n", global_header.snaplen);
    }

    /* Verify network type */
    if (global_header.network == 1) { // 1 indicates Ethernet
        printf("Link-layer header type is Ethernet (1)\n");
    } else {
        printf("Warning: Unsupported link-layer header type %u. Only Ethernet (1) is fully supported.\n", global_header.network);
        fclose(file);
        return -1;
    }


    /* After all checks pass, loop through all packets in the file */
    pcap_record_header_t record_header;
    int packet_count = 0;
    uint32_t prev_ts_sec = 0;
    uint32_t prev_ts_usec = 0;
    // Read packet records until the end of the file
    while(fread(&record_header, sizeof(pcap_record_header_t), 1, file) == 1) {
        packet_count++;
        printf("\n--- Packet %d ---\n", packet_count);

        // If byte swapping is needed, swap the fields in the record header
        if (file_ctx->swap_bytes) {
            record_header.ts_sec = swap_uint32(record_header.ts_sec);
            record_header.ts_usec = swap_uint32(record_header.ts_usec);
            record_header.incl_len = swap_uint32(record_header.incl_len);
            record_header.orig_len = swap_uint32(record_header.orig_len);
        }

        /* Check included length vs original length */
        if (record_header.incl_len > record_header.orig_len){
            fprintf(stderr, "Error: Included length (%u) exceeds original length (%u) in packet %d. Skipping packet.\n", record_header.incl_len, record_header.orig_len, packet_count);
            continue;
        }

        // Print the record header information
        printf("Timestamp (Seconds): %u\n", record_header.ts_sec);
        printf("Timestamp (Microseconds): %u\n", record_header.ts_usec);
        printf("Included Length: %u\n", record_header.incl_len);
        printf("Original Length: %u\n", record_header.orig_len);

        /* Calculate and print time delta between packets if not the first packet */
        if (packet_count > 1) {
            double time_delta = (double)(record_header.ts_sec - prev_ts_sec) + 
                                (double)(record_header.ts_usec - prev_ts_usec) / 1000000.0;
            printf("Time since last packet: %.6f seconds\n", time_delta);
        }

        /* Store current packet's timestamp for the next iteration */
        prev_ts_sec = record_header.ts_sec;
        prev_ts_usec = record_header.ts_usec;

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

        // Process the packet data (e.g., parse Ethernet, IP, TCP/UDP headers)
        process_ethernet_header(file_ctx, packet_data, record_header.incl_len);
        printf("Packet data read successfully (%zu bytes).\n", data_bytes_read);
        // Free the allocated memory for the packet data
        free(packet_data);
    }

    printf("\nTotal packets processed: %d\n", packet_count);

    // Close the file
    fclose(file);
    return 0; //File processed successfully
}