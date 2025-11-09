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
#include "pcap.h"

/* Function to read and process a pcap file */
int process_pcap_file(const char* filepath){
    FILE* *file = fopen(filepath, "rb"); // Read pcap file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
}