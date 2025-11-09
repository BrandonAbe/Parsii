#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pcap.h"

// --- Temporary Main Function for testing ---
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pcap_file_path>\n", argv[0]); // If no file path provided
        return 1;
    }
    const char* pcap_file = argv[1];
    return process_pcap_file(pcap_file);
    }
// --- END TEMPORARY MAIN FUNCTION ---