/* Header Guard(s) */
#ifndef PCAP_H
#define PCAP_H

/* Include Statements */
#include <stdint.h>
#include "clock_map.h"      // Include clock_map.h
#include "gptp_validator.h" // Include gptp_validator.h

typedef struct pcap_global_header {
    uint32_t magic_number;   /* magic number to detect file format and byte ordering */
    uint16_t version_major;  /* major version number */
    uint16_t version_minor;  /* minor version number */
    int32_t  thiszone;       /* GMT to local correction */
    uint32_t sigfigs;        /* accuracy of timestamps */
    uint32_t snaplen;        /* max length of captured packets, in octets */
    uint32_t network;        /* data link type as defined by https://www.tcpdump.org/linktypes.html*/
} pcap_global_header_t;

typedef struct pcap_record_header {
    uint32_t ts_sec;         /* timestamp seconds */
    uint32_t ts_usec;        /* timestamp microseconds */
    uint32_t incl_len;       /* number of octets of packet saved in file */
    uint32_t orig_len;       /* actual length of packet */
} pcap_record_header_t;

typedef struct{
    int swap_bytes; /* Flag to indicate if byte swapping is needed */
    int filter_ptp; /* Flag to indicate if PTP filtering is enabled */
    clock_map_t clock_map;      // Add clock map to file context
    gptp_cycle_map_t cycle_map; // Add gPTP cycle map to file context
} file_context_t;

int process_pcap_file(const char* filepath, file_context_t* file_ctx);

#endif