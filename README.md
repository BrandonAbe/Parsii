# ✅ PCAP Parser for PTP/gPTP Analysis (C) 

A lightweight, high-performance C-based parser for analyzing `.pcap` files with a focus on **Precision Time Protocol (PTP)** and **generalized PTP (gPTP)** frames.  

Designed for time synchronization validation, clock comparison, and cycle integrity checks between networked devices.

---
### ❔ Problem Statement ❔
Analyzing PTP/gPTP synchronization across networked devices presents significant challenges when using standard packet capture tools like Wireshark. Key issues include:

- Difficulty in tracking complete sync cycles between specific device pairs
- Complex manual effort required to verify protocol compliance
- No straightforward way to calculate clock offsets between master/slave devices
- Limited visibility into timing accuracy and sync failures
- Time-consuming analysis of large packet captures

This project aims to simplify PTP/gPTP analysis by providing a dedicated CLI tool that automatically validates timing synchronization, verifies protocol adherence, and calculates clock offsets between specified devices.

---

### 💡 Project Overview

This tool reads `.pcap` files directly (no dependencies on `libpcap`), decodes Ethernet and IP headers, identifies PTP/gPTP packets, and extracts synchronization-related information such as:

- Master/slave clock identity
- Sync, Follow_Up, Delay_Req, and Delay_Resp sequence verification
- Complete gPTP cycle validation between specific IP/MAC pairs
- Timing offsets and cycle timing accuracy
---
### 📁 Project Structure

```text
pcap_parser/
├── src/
│   ├── main.c                      # Entry point, argument parsing, and high-level control flow
│   ├── pcap_reader.c               # Functions for reading and validating PCAP file format
│   ├── ethernet_parser.c           # Layer 2 frame parsing and MAC address handling
│   ├── ip_parser.c                 # IPv4/IPv6 header parsing and address management
│   ├── ptp_parser.c                # PTP/gPTP protocol specific message parsing
│   └── utils.c                     # Common utilities, logging, and helper functions
├── include/
│   ├── pcap.h                      # PCAP format structures and reader declarations
│   ├── ethernet.h                  # Ethernet frame structures and parser declarations
│   ├── ip.h                        # IP protocol structures and parser declarations
│   ├── ptp.h                       # PTP message structures and parser declarations
│   └── utils.h                     # Utility function declarations and common macros
├── tests/
│   ├── test_samples/
│   │   └── ptp_cycle_example.pcap  # Sample PCAP file for testing
│   └── parser_tests.c              # Unit tests for parsing functions
└── README.md                       # Project documentation and build instructions
```
---
### **Phase 1️⃣: Initial PCAP Reading**
- [❌] Support basic PCAP format parsing
- [❌] Extract packet timestamps
- [❌] Handle different link types
- [❌] Validate file integrity
---
### **Phase 2️⃣: Protocol Identification**
- [❌] Detect Ethernet frame type
- [❌] Parse IP headers (v4/v6)
- [❌] Identify PTP/gPTP traffic
- [❌] Filter non-PTP packets
---
### **Phase 3️⃣: PTP Message Parsing**
- [❌] Support message types:
  - [❌] Sync (0x0)
  - [❌] Delay_Req (0x1)
  - [❌] Follow_Up (0x8)
  - [❌] Delay_Resp (0x9)
  - [❌] Announce (0xB)
- [❌] Extract key fields:
  - [❌] Sequence ID
  - [❌] Source clock identity
  - [❌] Correction field
  - [❌] Timestamp values
- [❌] Handle domain numbers and message flags
---
### **Phase 4️⃣: gPTP Cycle Verification**
- [❌] Implement mapping between master/slave clock IDs
- [❌] Track full cycle sequence:
  - [❌] Sync → Follow_Up → Delay_Req → Delay_Resp
- [❌] Verify that all message types occurred between given MAC/IP pairs
- [❌] Detect missing or out-of-order packets
- [❌] Compute timing deltas and cycle duration
- [❌] Report if a full valid gPTP cycle occurred
---
### 🏁 **Phase 5️⃣: Analysis and Reporting**
- [❌] Add summary statistics:
  - [❌] Total PTP packets
  - [❌] Completed cycles
  - [❌] Missing sequence IDs
- [❌] Export data as CSV or JSON for post-analysis
- [❌] Optional: CLI filters
  ```bash
  ./pcap_parser test.pcap --filter gptp --summary
  ./pcap_parser test.pcap --verify-cycle --src 192.168.0.10 --dst 192.168.0.20
---
### Reference Documentation
- https://wiki.wireshark.org/Development/LibpcapFileFormat
- https://cplusplus.com/reference/cstdio/fopen/
- https://cplusplus.com/reference/cstdio/perror/ 
- https://www.tcpdump.org/linktypes.html
- https://eci.intel.com/docs/3.0/development/tsnrefsw/tsn-overview.html 