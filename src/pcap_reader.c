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
    FILE* file = fopen(filepath, "rb"); // Read pcap file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        printf("File closed.\n");
        
        return -1; 
    }

    printf("File opened successfully: %s\n", filepath);

    //TODO: Add logic to read global header and packet records
    /* Read global header */
    pcap_global_header_t global_header;
    // from documentation, size_t fread( void *buffer, size_t size, size_t count, FILE *stream );
    size_t bytes_read = fread(&global_header, sizeof(pcap_global_header_t), 1, file); // Read content at location of global_header and write into bytes_read
    if (bytes_read != 1) {
        perror("Error reading global header");
        fclose(file);
        return -1; 
    }
    printf("PCAP Global Header read successfully.\n");


    /* Check magic number to determine endianness based on definition from PCAP standard */
    if (global_header.magic_number == 0xa1b2c3d4) {
        printf("PCAP file is in native byte order (no byte swapping needed)\n");
    } else if (global_header.magic_number == 0xd4c3b2a1) {
        printf("PCAP file is in swapped byte order (byte swapping needed)\n");
        global_header.version_major = swap_uint16(global_header.version_major);
        global_header.version_minor = swap_uint16(global_header.version_minor);
        global_header.thiszone = swap_int32(global_header.thiszone);
        global_header.sigfigs = swap_uint32(global_header.sigfigs);
        global_header.snaplen = swap_uint32(global_header.snaplen);
        global_header.network = swap_uint32(global_header.network);
    } else {
        frpintf(stderr, "Error: Not a valid PCAP file (invalid magic number).\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0; //File processed successfully
} 