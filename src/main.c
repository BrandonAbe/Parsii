#include <stdio.h>
#include <string.h>
#include "../include/pcap.h" // Include pcap.h for process_pcap_file declaration

int main(int argc, char* argv[]) {
    const char* file_path = NULL;
    file_context_t file_ctx = {0}; // Initialize file context

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--filter-ptp") == 0) {
            file_ctx.filter_ptp = 1;
            printf("PTP/gPTP filtering enabled.\n");
        } else {
            file_path = argv[i];
        }
    }

    // If no file path is provided, print usage and exit
    if (file_path == NULL) {
        fprintf(stderr, "Usage: %s <file_path> [--filter-ptp]\n", argv[0]);
        return 1;
    }

    printf("PCAP Parser - Main application (under development)\n");

    // Call the process_pcap_file function
    if (process_pcap_file(file_path, &file_ctx) != 0) {
        fprintf(stderr, "Failed to process PCAP file: %s\n", file_path);
        return 1;
    }

    return 0;
}