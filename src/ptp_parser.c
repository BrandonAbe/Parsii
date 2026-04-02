#include "../include/ptp.h"
#include "../include/utils.h"
#include "../include/clock_map.h" 
#include "../include/gptp_validator.h" 
#include <stdio.h>
#include <string.h> // For memcpy

// Prints common PTP header information.
static void print_common_ptp_header_info(const ptp_common_header_t* header) {
    uint8_t transportSpecific = (header->transportSpecific_messageType >> 4) & 0x0F;
    ptp_message_type_t messageType = header->transportSpecific_messageType & 0x0F;
    uint8_t versionPTP = (header->versionPTP_reserved >> 4) & 0x0F;

    char port_identity_str[22]; 
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

// Processes PTP Sync messages.
static void process_sync_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { 
        fprintf(stderr, "Truncated PTP Sync message\n");
        return;
    }

    ptp_sync_message_t sync_message;
    memcpy(&sync_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    sync_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    sync_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Sync Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)sync_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", sync_message.originTimestamp_nanoseconds);
}


// Processes PTP Follow_Up messages.
static void process_follow_up_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { 
        fprintf(stderr, "Truncated PTP Follow_Up message\n");
        return;
    }

    ptp_follow_up_message_t follow_up_message;
    memcpy(&follow_up_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    follow_up_message.preciseOriginTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    follow_up_message.preciseOriginTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Follow_Up Message:\n");
    printf("            Precise Origin Timestamp (seconds): %llu\n", (unsigned long long)follow_up_message.preciseOriginTimestamp_seconds);
    printf("            Precise Origin Timestamp (nanoseconds): %u\n", follow_up_message.preciseOriginTimestamp_nanoseconds);
}

// Processes PTP Pdelay_Req messages.
static void process_pdelay_req_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { 
        fprintf(stderr, "Truncated PTP Pdelay_Req message\n");
        return;
    }

    ptp_pdelay_req_message_t pdelay_req_message;
    memcpy(&pdelay_req_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_req_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    pdelay_req_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Pdelay_Req Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)pdelay_req_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", pdelay_req_message.originTimestamp_nanoseconds);
}

// Processes PTP Pdelay_Resp messages.
static void process_pdelay_resp_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { 
        fprintf(stderr, "Truncated PTP Pdelay_Resp message\n");
        return;
    }

    ptp_pdelay_resp_message_t pdelay_resp_message;
    memcpy(&pdelay_resp_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_resp_message.requestReceiptTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    pdelay_resp_message.requestReceiptTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; 

    memcpy(pdelay_resp_message.requestingPortIdentity, packet_data + offset, 10);

    char requesting_port_identity_str[22]; 
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


// Processes PTP Pdelay_Resp_Follow_Up messages.
static void process_pdelay_resp_follow_up_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { 
        fprintf(stderr, "Truncated PTP Pdelay_Resp_Follow_Up message\n");
        return;
    }

    ptp_pdelay_resp_follow_up_message_t pdelay_resp_follow_up_message;
    memcpy(&pdelay_resp_follow_up_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    pdelay_resp_follow_up_message.responseOriginTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    pdelay_resp_follow_up_message.responseOriginTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; 

    memcpy(pdelay_resp_follow_up_message.requestingPortIdentity, packet_data + offset, 10);

    char requesting_port_identity_str[22]; 
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

// Processes PTP Delay_Req messages.
static void process_delay_req_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 10) { 
        fprintf(stderr, "Truncated PTP Delay_Req message\n");
        return;
    }

    ptp_delay_req_message_t delay_req_message;
    memcpy(&delay_req_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    delay_req_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    delay_req_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);

    printf("        PTP Delay_Req Message:\n");
    printf("            Origin Timestamp (seconds): %llu\n", (unsigned long long)delay_req_message.originTimestamp_seconds);
    printf("            Origin Timestamp (nanoseconds): %u\n", delay_req_message.originTimestamp_nanoseconds);
}

// Processes PTP Delay_Resp messages.
static void process_delay_resp_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 20) { 
        fprintf(stderr, "Truncated PTP Delay_Resp message\n");
        return;
    }

    ptp_delay_resp_message_t delay_resp_message;
    memcpy(&delay_resp_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    delay_resp_message.receiveTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6; 
    delay_resp_message.receiveTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4; 

    memcpy(delay_resp_message.requestingPortIdentity, packet_data + offset, 10);

    char requesting_port_identity_str[22]; 
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

// Processes PTP Announce messages.
static void process_announce_message(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length) {
    if (data_length < sizeof(ptp_common_header_t) + 30) { 
        fprintf(stderr, "Truncated PTP Announce message\n");
        return;
    }

    ptp_announce_message_t announce_message;
    memcpy(&announce_message.header, packet_data, sizeof(ptp_common_header_t));

    uint32_t offset = sizeof(ptp_common_header_t);
    announce_message.originTimestamp_seconds = bytes_to_uint48(packet_data + offset, file_ctx->swap_bytes);
    offset += 6;
    announce_message.originTimestamp_nanoseconds = BYTES_TO_UINT32(packet_data + offset, file_ctx->swap_bytes);
    offset += 4;
    announce_message.currentUtcOffset = BYTES_TO_UINT16(packet_data + offset, file_ctx->swap_bytes);
    offset += 2;
    offset += 1; 
    announce_message.grandmasterPriority1 = packet_data[offset++];
    memcpy(announce_message.grandmasterClockQuality, packet_data + offset, 4);
    offset += 4;
    announce_message.grandmasterPriority2 = packet_data[offset++];
    memcpy(announce_message.grandmasterIdentity, packet_data + offset, 8);
    offset += 8;
    announce_message.stepsRemoved = BYTES_TO_UINT16(packet_data + offset, file_ctx->swap_bytes);
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
    printf("            Grandmaster Clock Quality: 0x%02x%02x%02x%02x\n",
        announce_message.grandmasterClockQuality[0], announce_message.grandmasterClockQuality[1],
        announce_message.grandmasterClockQuality[2], announce_message.grandmasterClockQuality[3]);
    printf("            Grandmaster Priority 2: %u\n", announce_message.grandmasterPriority2);
    printf("            Grandmaster Identity: %s\n", grandmaster_identity_str);
    printf("            Steps Removed: %u\n", announce_message.stepsRemoved);
    printf("            Time Source: 0x%x\n", announce_message.timeSource);

    uint64_t grandmaster_id = 0;
    uint64_t source_port_id = 0;
    memcpy(&grandmaster_id, announce_message.grandmasterIdentity, sizeof(uint64_t));
    memcpy(&source_port_id, announce_message.header.sourcePortIdentity, sizeof(uint64_t));

    if (file_ctx->swap_bytes) {
        grandmaster_id = swap_uint64(grandmaster_id);
        source_port_id = swap_uint64(source_port_id);
    }
    
    if (clock_map_insert(&file_ctx->clock_map, grandmaster_id, source_port_id) != 0) {
        fprintf(stderr, "Failed to insert into clock map.\n");
    } else {
        printf("            Clock Map: Added Grandmaster ID %016llx -> Source Port ID %016llx\n", 
               (unsigned long long)grandmaster_id, (unsigned long long)source_port_id);
    }
}


// Processes PTP header and dispatches to specific message handlers.
void process_ptp_header(file_context_t* file_ctx, const uint8_t* packet_data, uint32_t data_length, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac) {
    if (data_length < sizeof(ptp_common_header_t)) {
        fprintf(stderr, "Truncated PTP common header\n");
        return;
    }

    ptp_common_header_t common_header;
    memcpy(&common_header, packet_data, sizeof(ptp_common_header_t));

    if (file_ctx->swap_bytes) {
        common_header.messageLength = swap_uint16(common_header.messageLength);
        common_header.flags = swap_uint16(common_header.flags);
        common_header.correctionField = swap_uint64(common_header.correctionField);
        common_header.sequenceId = swap_uint16(common_header.sequenceId);
    }

    print_common_ptp_header_info(&common_header);

    process_gptp_message(file_ctx, &common_header, packet_data, data_length, eth_src_mac, eth_dst_mac);

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
        case PTP_MESSAGE_SIGNALING:
        case PTP_MESSAGE_MANAGEMENT:
            printf("        PTP Message Type 0x%x parsing not yet implemented.\n", messageType);
            break;
        default:
            printf("        Unknown PTP Message Type 0x%x\n", messageType);
            break;
    }
}
