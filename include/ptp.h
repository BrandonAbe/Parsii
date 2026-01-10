#ifndef PTP_H
#define PTP_H

#include <stdint.h>
#include "pcap.h" // For file_context_t

/* PTP Message Types */ 
typedef enum {
    PTP_MESSAGE_SYNC                = 0x0,
    PTP_MESSAGE_DELAY_REQ           = 0x1,
    PTP_MESSAGE_PDELAY_REQ          = 0x2,
    PTP_MESSAGE_PDELAY_RESP         = 0x3,
    PTP_MESSAGE_FOLLOW_UP           = 0x8,
    PTP_MESSAGE_DELAY_RESP          = 0x9,
    PTP_MESSAGE_PDELAY_RESP_FOLLOW_UP = 0xA,
    PTP_MESSAGE_ANNOUNCE            = 0xB,
    PTP_MESSAGE_SIGNALING           = 0xC,
    PTP_MESSAGE_MANAGEMENT          = 0xD
} ptp_message_type_t;

/* PTP Common Header (34 bytes)*/
typedef struct {
    uint8_t  transportSpecific_messageType; // 4 bits transportSpecific, 4 bits messageType
    uint8_t  versionPTP_reserved;           // 4 bits versionPTP, 4 bits reserved
    uint16_t messageLength;                 // Message length in bytes
    uint8_t  domainNumber;                  // Domain number
    uint8_t  reserved1;
    uint16_t flags;
    uint64_t correctionField;               // Correction field
    uint32_t reserved2;
    uint8_t  sourcePortIdentity[10];        // ClockIdentity (8 bytes) + PortNumber (2 bytes)
    uint16_t sequenceId;                    // Sequence ID
    uint8_t  controlField;                  // Control field
    int8_t   logMessageInterval;            // Log message interval
} ptp_common_header_t;

/*  PTP Sync Message (Common header + 10 bytes) */
typedef struct {
    ptp_common_header_t header;
    uint64_t  originTimestamp_seconds;      // Origin Timestamp seconds
    uint32_t  originTimestamp_nanoseconds;  // Origin Timestamp nanoseconds
} ptp_sync_message_t;

/* PTP Follow_Up Message (Common header + 10 bytes) */
typedef struct {
    ptp_common_header_t header;
    uint64_t  preciseOriginTimestamp_seconds;     // Precise Origin Timestamp seconds
    uint32_t  preciseOriginTimestamp_nanoseconds; // Precise Origin Timestamp nanoseconds
} ptp_follow_up_message_t;

/* PTP Pdelay_Req Message (Common header + 10 bytes) */ 
typedef struct {
    ptp_common_header_t header;
    uint64_t  originTimestamp_seconds;      // Origin Timestamp seconds
    uint32_t  originTimestamp_nanoseconds;  // Origin Timestamp nanoseconds
} ptp_pdelay_req_message_t;

/* PTP Pdelay_Resp Message (Common header + 20 bytes) */
typedef struct {
    ptp_common_header_t header;
    uint64_t  requestReceiptTimestamp_seconds;      // Request Receipt Timestamp seconds
    uint32_t  requestReceiptTimestamp_nanoseconds;  // Request Receipt Timestamp nanoseconds
    uint8_t   requestingPortIdentity[10];           // Requesting PortIdentity
} ptp_pdelay_resp_message_t;

/* PTP Pdelay_Resp_Follow_Up Message (Common header + 20 bytes) */
typedef struct {
    ptp_common_header_t header;
    uint64_t  responseOriginTimestamp_seconds;      // Response Origin Timestamp seconds
    uint32_t  responseOriginTimestamp_nanoseconds;  // Response Origin Timestamp nanoseconds
    uint8_t   requestingPortIdentity[10];           // Requesting PortIdentity
} ptp_pdelay_resp_follow_up_message_t;

void process_ptp_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length);

#endif // PTP_H
