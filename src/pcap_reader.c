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

    fclose(file);
    return 0; //File processed successfully
} 