#include <stdio.h>
#include "../include/pcap.h" // Include pcap.h for process_pcap_file declaration

int main(int argc, char* argv[]) {
    printf("PCAP Parser - Main application (under development)\n");

    // Define the path to the test PCAP file
    const char* file_path = "tests/test_samples/Data.pcap";

    // Call the process_pcap_file function
    if (process_pcap_file(file_path) != 0) {
        fprintf(stderr, "Failed to process PCAP file: %s\n", file_path);
        return 1;
    }

    return 0;
}