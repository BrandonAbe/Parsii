#include <stdio.h>
#include <string.h>
#include "../include/pcap.h" 
#include "../include/clock_map.h" 
#include "../include/gptp_validator.h" 

int main(int argc, char* argv[]) {
    const char* file_path = NULL;
    file_context_t file_ctx = {0}; // Initialize file context

    // Initialize clock map and gPTP cycle map
    if (clock_map_init(&file_ctx.clock_map) != 0) {
        fprintf(stderr, "Failed to initialize clock map.\n");
        return 1;
    }
    if (gptp_cycle_map_init(&file_ctx.cycle_map) != 0) {
        fprintf(stderr, "Failed to initialize gPTP cycle map.\n");
        clock_map_free(&file_ctx.clock_map); // Free clock map if cycle map init fails
        return 1;
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--filter-ptp") == 0) {
            file_ctx.filter_ptp = 1;
            printf("PTP/gPTP filtering enabled.\n");
        } else {
            file_path = argv[i];
        }
    }

    // Print usage and exit if no file path is provided
    if (file_path == NULL) {
        fprintf(stderr, "Usage: %s <file_path> [--filter-ptp]\n", argv[0]);
        clock_map_free(&file_ctx.clock_map);
        gptp_cycle_map_free(&file_ctx.cycle_map);
        return 1;
    }

    printf("PCAP Parser - Main application (under development)\n");

    // Process the PCAP file
    if (process_pcap_file(file_path, &file_ctx) != 0) {
        fprintf(stderr, "Failed to process PCAP file: %s\n", file_path);
        clock_map_free(&file_ctx.clock_map);
        gptp_cycle_map_free(&file_ctx.cycle_map);
        return 1;
    }

    // Free all allocated maps
    clock_map_free(&file_ctx.clock_map);
    gptp_cycle_map_free(&file_ctx.cycle_map);

    return 0;
}