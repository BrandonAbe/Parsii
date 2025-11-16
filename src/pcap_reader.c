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
    int swap_bytes = 0;
    if (global_header.magic_number == 0xa1b2c3d4) {
        printf("PCAP file is in native byte order (no byte swapping needed)\n");
    } else if (global_header.magic_number == 0xd4c3b2a1) {
        printf("PCAP file is in swapped byte order (byte swapping needed)\n");
        swap_bytes = 1;
        global_header.version_major = swap_uint16(global_header.version_major);
        global_header.version_minor = swap_uint16(global_header.version_minor);
        global_header.thiszone = swap_uint32(global_header.thiszone);
        global_header.sigfigs = swap_uint32(global_header.sigfigs);
        global_header.snaplen = swap_uint32(global_header.snaplen);
        global_header.network = swap_uint32(global_header.network);
    } else {
        fprintf(stderr, "Error: Not a valid PCAP file (invalid magic number).\n");
        fclose(file);
        return -1;
    }

    /* Print info of PCAP read */
    printf("PCAP Version: %d.%d\n", global_header.version_major, global_header.version_minor);
    printf("Snaplen: %u\n", global_header.snaplen);
    printf("Network: %u\n", global_header.network);

    /* Read the first packet's record header */
    pcap_record_header_t record_header;
    bytes_read = fread(&record_header, sizeof(pcap_record_header_t), 1, file);
    if (bytes_read != 1) {
        if (feof(file)) {
            printf("End of file reached while reading record header.\n");
        } else {
            perror("Error reading record header");
        }
        fclose(file);
        return feof(file) ? 0 : -1; // Return 0 if EOF, else -1 for error
    }

    if (swap_bytes) {
        record_header.ts_sec = swap_uint32(record_header.ts_sec);
        record_header.ts_usec = swap_uint32(record_header.ts_usec);
        record_header.incl_len = swap_uint32(record_header.incl_len);
        record_header.orig_len = swap_uint32(record_header.orig_len);
    }

    fclose(file);
    return 0; //File processed successfully
} 