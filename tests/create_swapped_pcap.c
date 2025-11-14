#include <stdio.h>
#include <stdint.h>
#include <string.h> // For memcpy if needed, though not strictly for this simple header
#include "../include/pcap.h" // Assuming pcap.h is in the include directory

int main() {
    pcap_global_header_t header;

    // Use the swapped magic number
    header.magic_number = 0xd4c3b2a1; 
    
    // Fill the rest with dummy data (already in host byte order)
    header.version_major = 2;
    header.version_minor = 4;
    header.thiszone = 0;
    header.sigfigs = 0;
    header.snaplen = 65535;
    header.network = 1; // LINKTYPE_ETHERNET

    // Define the output file path
    const char* output_filepath = "tests/test_samples/swapped_endian.pcap";

    // Open a new file to write to
    FILE *file = fopen(output_filepath, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return 1;
    }

    // Write the header to the file
    size_t written = fwrite(&header, sizeof(pcap_global_header_t), 1, file);
    if (written != 1) {
        fprintf(stderr, "Error writing to file\n");
        fclose(file);
        return 1;
    }

    printf("Successfully created '%s'\n", output_filepath);
    fclose(file);
    return 0;
}