#include "../include/ptp.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h> // For memcpy

/* Function to print common PTP header information */
static void print_common_ptp_header_info(const ptp_common_header_t* header) {
    // Extract transportSpecific and messageType
    uint8_t transportSpecific = (header->transportSpecific_messageType >> 4) & 0x0F;
    ptp_message_type_t messageType = header->transportSpecific_messageType & 0x0F;

    // Extract versionPTP and reserved
    uint8_t versionPTP = (header->versionPTP_reserved >> 4) & 0x0F;

    // Port Identity (ClockIdentity and PortNumber)
    char port_identity_str[22]; // 8 bytes ClockIdentity + 2 bytes PortNumber + null terminator
    snprintf(port_identity_str, sizeof(port_identity_str), "%02x%02x%02x%02x%02x%02x%02x%02x:%04x",
             header->sourcePortIdentity[0], header->sourcePortIdentity[1],
             header->sourcePortIdentity[2], header->sourcePortIdentity[3],
             header->sourcePortIdentity[4], header->sourcePortIdentity[5],
             header->sourcePortIdentity[6], header->sourcePortIdentity[7],
             (header->sourcePortIdentity[8] << 8) | header->sourcePortIdentity[9]);


    printf("        PTP Common Header:\n");
    printf("            Transport Specific: 0x%x\n", transportSpecific);
    printf("            Message Type: 0x%x (%s)\n", messageType,
           messageType == PTP_MESSAGE_SYNC ? "Sync" :
           messageType == PTP_MESSAGE_DELAY_REQ ? "Delay_Req" :
           messageType == PTP_MESSAGE_PDELAY_REQ ? "Pdelay_Req" :
           messageType == PTP_MESSAGE_PDELAY_RESP ? "Pdelay_Resp" :
           messageType == PTP_MESSAGE_FOLLOW_UP ? "Follow_Up" :
           messageType == PTP_MESSAGE_DELAY_RESP ? "Delay_Resp" :
           messageType == PTP_MESSAGE_PDELAY_RESP_FOLLOW_UP ? "Pdelay_Resp_Follow_Up" :
           messageType == PTP_MESSAGE_ANNOUNCE ? "Announce" :
           messageType == PTP_MESSAGE_SIGNALING ? "Signaling" :
           messageType == PTP_MESSAGE_MANAGEMENT ? "Management" : "Unknown");
    printf("            Version PTP: %d\n", versionPTP);
    printf("            Message Length: %u\n", header->messageLength);
    printf("            Domain Number: %d\n", header->domainNumber);
    printf("            Flags: 0x%04x\n", header->flags);
    printf("            Correction Field: %llu\n", (unsigned long long)header->correctionField);
    printf("            Source Port Identity: %s\n", port_identity_str);
    printf("            Sequence ID: %u\n", header->sequenceId);
    printf("            Control Field: %d\n", header->controlField);
    printf("            Log Message Interval: %d\n", header->logMessageInterval);
}

/* Function to process PTP Sync messages */
static void process_sync_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { // Common header + 10 bytes for originTimestamp
        fprintf(stderr, "Truncated PTP Sync message\n");
        return;
    }

    ptp_sync_message_t sync_message;
    memcpy(&sync_message.header, packet_data, sizeof(ptp_common_header_t));

    // Common header fields (already parsed in process_ptp_header, but useful for context here)
    // Note: messageLength from common header can be used to determine overall PTP message length

    // Parse Sync message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    sync_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    sync_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Sync Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)sync_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", sync_message.originTimestamp_nanoseconds);
}


/* Function to process PTP Follow_Up messages */
static void process_follow_up_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { // Common header + 10 bytes for preciseOriginTimestamp
        fprintf(stderr, "Truncated PTP Follow_Up message\n");
        return;
    }

    ptp_follow_up_message_t follow_up_message;
    memcpy(&follow_up_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Follow_Up message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    follow_up_message.preciseOriginTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    follow_up_message.preciseOriginTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Follow_Up Message:\n");
    printf("            Precise Origin Timestamp (seconds): %llu\n", (unsigned long long)follow_up_message.preciseOriginTimestamp_seconds);
    printf("            Precise Origin Timestamp (nanoseconds): %u\n", follow_up_message.preciseOriginTimestamp_nanoseconds);
}

/* Function to process PTP Pdelay_Req messages */
static void process_pdelay_req_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { // Common header + 10 bytes for originTimestamp
        fprintf(stderr, "Truncated PTP Pdelay_Req message\n");
        return;
    }

    ptp_pdelay_req_message_t pdelay_req_message;
    memcpy(&pdelay_req_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Pdelay_Req message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_req_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    pdelay_req_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Pdelay_Req Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)pdelay_req_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", pdelay_req_message.originTimestamp_nanoseconds);
}

/* Function to process PTP Pdelay_Resp messages */
static void process_pdelay_resp_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { // Common header + 20 bytes for requestReceiptTimestamp + requestingPortIdentity
        fprintf(stderr, "Truncated PTP Pdelay_Resp message\n");
        return;
    }

    ptp_pdelay_resp_message_t pdelay_resp_message;
    memcpy(&pdelay_resp_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Pdelay_Resp message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_resp_message.requestReceiptTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    pdelay_resp_message.requestReceiptTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; // 4 bytes for 32-bit field

    memcpy(pdelay_resp_message.requestingPortIdentity, packet_data + offset, 10);

    // Port Identity (ClockIdentity and PortNumber)
    char requesting_port_identity_str[22]; // 8 bytes ClockIdentity + 2 bytes PortNumber + null terminator
    snprintf(requesting_port_identity_str, sizeof(requesting_port_identity_str), "%02x%02x%02x%02x%02x%02x%02x%02x:%04x",
             pdelay_resp_message.requestingPortIdentity[0], pdelay_resp_message.requestingPortIdentity[1],
             pdelay_resp_message.requestingPortIdentity[2], pdelay_resp_message.requestingPortIdentity[3],
             pdelay_resp_message.requestingPortIdentity[4], pdelay_resp_message.requestingPortIdentity[5],
             pdelay_resp_message.requestingPortIdentity[6], pdelay_resp_message.requestingPortIdentity[7],
             (pdelay_resp_message.requestingPortIdentity[8] << 8) | pdelay_resp_message.requestingPortIdentity[9]);

    printf("        PTP Pdelay_Resp Message:\n");
    printf("            Request Receipt Timestamp (seconds): %llu\n", (unsigned long long)pdelay_resp_message.requestReceiptTimestamp_seconds);
    printf("            Request Receipt Timestamp (nanoseconds): %u\n", pdelay_resp_message.requestReceiptTimestamp_nanoseconds);
    printf("            Requesting Port Identity: %s\n", requesting_port_identity_str);
}


/* Function to process PTP Pdelay_Resp_Follow_Up messages */
static void process_pdelay_resp_follow_up_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { // Common header + 20 bytes for responseOriginTimestamp + requestingPortIdentity
        fprintf(stderr, "Truncated PTP Pdelay_Resp_Follow_Up message\n");
        return;
    }

    ptp_pdelay_resp_follow_up_message_t pdelay_resp_follow_up_message;
    memcpy(&pdelay_resp_follow_up_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Pdelay_Resp_Follow_Up message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_resp_follow_up_message.responseOriginTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    pdelay_resp_follow_up_message.responseOriginTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; // 4 bytes for 32-bit field

    memcpy(pdelay_resp_follow_up_message.requestingPortIdentity, packet_data + offset, 10);

    // Port Identity (ClockIdentity and PortNumber)
    char requesting_port_identity_str[22]; // 8 bytes ClockIdentity + 2 bytes PortNumber + null terminator
    snprintf(requesting_port_identity_str, sizeof(requesting_port_identity_str), "%02x%02x%02x%02x%02x%02x%02x%02x:%04x",
             pdelay_resp_follow_up_message.requestingPortIdentity[0], pdelay_resp_follow_up_message.requestingPortIdentity[1],
             pdelay_resp_follow_up_message.requestingPortIdentity[2], pdelay_resp_follow_up_message.requestingPortIdentity[3],
             pdelay_resp_follow_up_message.requestingPortIdentity[4], pdelay_resp_follow_up_message.requestingPortIdentity[5],
             pdelay_resp_follow_up_message.requestingPortIdentity[6], pdelay_resp_follow_up_message.requestingPortIdentity[7],
             (pdelay_resp_follow_up_message.requestingPortIdentity[8] << 8) | pdelay_resp_follow_up_message.requestingPortIdentity[9]);

    printf("        PTP Pdelay_Resp_Follow_Up Message:\n");
    printf("            Response Origin Timestamp (seconds): %llu\n", (unsigned long long)pdelay_resp_follow_up_message.responseOriginTimestamp_seconds);
    printf("            Response Origin Timestamp (nanoseconds): %u\n", pdelay_resp_follow_up_message.responseOriginTimestamp_nanoseconds);
    printf("            Requesting Port Identity: %s\n", requesting_port_identity_str);
}

/* Function to process PTP Delay_Req messages */
static void process_delay_req_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { // Common header + 10 bytes for originTimestamp
        fprintf(stderr, "Truncated PTP Delay_Req message\n");
        return;
    }

    ptp_delay_req_message_t delay_req_message;
    memcpy(&delay_req_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Delay_Req message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    delay_req_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    delay_req_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Delay_Req Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)delay_req_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", delay_req_message.originTimestamp_nanoseconds);
}

/* Function to process PTP Delay_Resp messages */ 
static void process_delay_resp_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { // Common header + 20 bytes for receiveTimestamp + requestingPortIdentity
        fprintf(stderr, "Truncated PTP Delay_Resp message\n");
        return;
    }

    ptp_delay_resp_message_t delay_resp_message;
    memcpy(&delay_resp_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Delay_Resp message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    delay_resp_message.receiveTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; // 6 bytes for 48-bit field
    delay_resp_message.receiveTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; // 4 bytes for 32-bit field

    memcpy(delay_resp_message.requestingPortIdentity, packet_data + offset, 10);

    // Port Identity (ClockIdentity and PortNumber)
    char requesting_port_identity_str[22]; // 8 bytes ClockIdentity + 2 bytes PortNumber + null terminator
    snprintf(requesting_port_identity_str, sizeof(requesting_port_identity_str), "%02x%02x%02x%02x%02x%02x%02x%02x:%04x",
             delay_resp_message.requestingPortIdentity[0], delay_resp_message.requestingPortIdentity[1],
             delay_resp_message.requestingPortIdentity[2], delay_resp_message.requestingPortIdentity[3],
             delay_resp_message.requestingPortIdentity[4], delay_resp_message.requestingPortIdentity[5],
             delay_resp_message.requestingPortIdentity[6], delay_resp_message.requestingPortIdentity[7],
             (delay_resp_message.requestingPortIdentity[8] << 8) | delay_resp_message.requestingPortIdentity[9]);

    printf("        PTP Delay_Resp Message:\n");
    printf("            Receive Timestamp (seconds): %llu\n", (unsigned long long)delay_resp_message.receiveTimestamp_seconds);
    printf("            Receive Timestamp (nanoseconds): %u\n", delay_resp_message.receiveTimestamp_nanoseconds);
    printf("            Requesting Port Identity: %s\n", requesting_port_identity_str);
}

/* Function to process PTP Announce messages */
static void process_announce_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 30) { // Common header + 30 bytes for announce message
        fprintf(stderr, "Truncated PTP Announce message\n");
        return;
    }

    ptp_announce_message_t announce_message;
    memcpy(&announce_message.header, packet_data, sizeof(ptp_common_header_t));

    // Parse Announce message specific fields
    uint32_t offset = sizeof(ptp_common_header_t);
    announce_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6;
    announce_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4;
    announce_message.currentUtcOffset = BYTES_TO_UINT16(packet_data + offset, file_ctx->swap_bytes);
    offset += 2;
    offset += 1; // reserved
    announce_message.grandmasterPriority1 = packet_data[offset++];
    memcpy(announce_message.grandmasterClockQuality, packet_data + offset, 4);
    offset += 4;
    announce_message.grandmasterPriority2 = packet_data[offset++];
    memcpy(announce_message.grandmasterIdentity, packet_data + offset, 8);
    offset += 8;
    announce_message.stepsRemoved = BYTES_TO_UINT16(packet_data + offset, file_ctx->swap_bytes);
    offset += 2;
    announce_message.timeSource = packet_data[offset];

    char grandmaster_identity_str[24];
    snprintf(grandmaster_identity_str, sizeof(grandmaster_identity_str), "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
        announce_message.grandmasterIdentity[0], announce_message.grandmasterIdentity[1],
        announce_message.grandmasterIdentity[2], announce_message.grandmasterIdentity[3],
        announce_message.grandmasterIdentity[4], announce_message.grandmasterIdentity[5],
        announce_message.grandmasterIdentity[6], announce_message.grandmasterIdentity[7]);

    printf("        PTP Announce Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)announce_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", announce_message.originTimestamp_nanoseconds);
    printf("            Current UTC Offset: %d\n", announce_message.currentUtcOffset);
    printf("            Grandmaster Priority 1: %u\n", announce_message.grandmasterPriority1);
    // grandmasterClockQuality is a struct itself, printing as hex for now
    printf("            Grandmaster Clock Quality: 0x%02x%02x%02x%02x\n",
        announce_message.grandmasterClockQuality[0], announce_message.grandmasterClockQuality[1],
        announce_message.grandmasterClockQuality[2], announce_message.grandmasterClockQuality[3]);
    printf("            Grandmaster Priority 2: %u\n", announce_message.grandmasterPriority2);
    printf("            Grandmaster Identity: %s\n", grandmaster_identity_str);
    printf("            Steps Removed: %u\n", announce_message.stepsRemoved);
    printf("            Time Source: 0x%x\n", announce_message.timeSource);
}


void process_ptp_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t)) {
        fprintf(stderr, "Truncated PTP common header\n");
        return;
    }

    ptp_common_header_t common_header;
    memcpy(&common_header, packet_data, sizeof(ptp_common_header_t));

    // Apply endianness correction to multi-byte fields
    if (file_ctx->swap_bytes) {
        common_header.messageLength = swap_uint16(common_header.messageLength);
        common_header.flags = swap_uint16(common_header.flags);
        common_header.correctionField = swap_uint64(common_header.correctionField);
        // sourcePortIdentity is byte array, no swap
        common_header.sequenceId = swap_uint16(common_header.sequenceId);
        // controlField and logMessageInterval are single bytes, no swap
    }

    print_common_ptp_header_info(&common_header);

    // Determine message type and dispatch to specific handler
    ptp_message_type_t messageType = common_header.transportSpecific_messageType & 0x0F;

    switch (messageType) {
        case PTP_MESSAGE_SYNC:
            process_sync_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_DELAY_REQ:
            process_delay_req_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_FOLLOW_UP:
            process_follow_up_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_PDELAY_REQ:
            process_pdelay_req_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_PDELAY_RESP:
            process_pdelay_resp_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_PDELAY_RESP_FOLLOW_UP:
            process_pdelay_resp_follow_up_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_DELAY_RESP:
            process_delay_resp_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_ANNOUNCE:
            process_announce_message(file_ctx, packet_data, data_length);
            break;
        case PTP_MESSAGE_SIGNALING):
        case PTP_MESSAGE_MANAGEMENT:
            printf("        PTP Message Type 0x%x parsing not yet implemented.\n", messageType);
            break;
        default:
            printf("        Unknown PTP Message Type 0x%x\n", messageType);
            break;
    }
}
