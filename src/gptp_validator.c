#include "../include/gptp_validator.h"
#include "../include/ptp.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CYCLE_MAP_INITIAL_SIZE 1024 // Initial number of buckets for the hash map

/*
 * Simple hash function for a 64-bit key.
 * @param map The hash map.
 * @param key The 64-bit key.
 * @return The hash index.
 */
static size_t gptp_cycle_map_hash(const gptp_cycle_map_t *map, uint64_t key) {
    return (size_t)(key % map->size);
}

/*
 * Initializes a new gptp_cycle hash map.
 * @param map Pointer to the gptp_cycle_map_t structure.
 * @return 0 on success, -1 on memory allocation failure.
 */
int gptp_cycle_map_init(gptp_cycle_map_t *map) {
    if (map == NULL) {
        return -1;
    }
    map->size = CYCLE_MAP_INITIAL_SIZE;
    map->buckets = (cycle_map_entry_t **)calloc(map->size, sizeof(cycle_map_entry_t *));
    if (map->buckets == NULL) {
        perror("Failed to allocate memory for gPTP cycle map buckets");
        return -1;
    }
    return 0;
}

/*
 * Creates a combined key from sourcePortIdentity (first 8 bytes) and sequenceId.
 * @param source_port_identity The 10-byte sourcePortIdentity.
 * @param sequence_id The sequenceId.
 * @return The combined 64-bit key.
 */
uint64_t create_cycle_key(const uint8_t source_port_identity[10], uint16_t sequence_id, int swap_bytes) {
    uint64_t clock_id = 0;
    memcpy(&clock_id, source_port_identity, sizeof(uint64_t));
    if (swap_bytes) {
        clock_id = swap_uint64(clock_id);
    }
    // Combines clock_id and sequence_id into a single 64-bit key.
    return (clock_id << 16) | sequence_id; 
}


/*
 * Inserts or updates a gptp_cycle_t in the hash map.
 * If the key exists, returns its associated cycle for update.
 * If not, creates a new cycle and returns it.
 * @param map The hash map.
 * @param key The 64-bit key for the cycle.
 * @return Pointer to the gptp_cycle_t, or NULL on memory allocation failure.
 */
gptp_cycle_t* gptp_cycle_map_insert_or_get(gptp_cycle_map_t *map, uint64_t key) {
    if (map == NULL) {
        return NULL;
    }

    size_t index = gptp_cycle_map_hash(map, key);
    cycle_map_entry_t *current = map->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            return current->cycle; 
        }
        current = current->next;
    }

    cycle_map_entry_t *new_entry = (cycle_map_entry_t *)malloc(sizeof(cycle_map_entry_t));
    if (new_entry == NULL) {
        perror("Failed to allocate memory for cycle map entry");
        return NULL;
    }
    gptp_cycle_t *new_cycle = (gptp_cycle_t *)calloc(1, sizeof(gptp_cycle_t));
    if (new_cycle == NULL) {
        perror("Failed to allocate memory for gPTP cycle");
        free(new_entry);
        return NULL;
    }

    new_entry->key = key;
    new_entry->cycle = new_cycle;
    new_entry->next = map->buckets[index]; 
    map->buckets[index] = new_entry;
    
    new_cycle->state = GPTP_CYCLE_STATE_INIT;
    new_cycle->sequence_id = (uint16_t)(key & 0xFFFF);
    new_cycle->last_update_time = time(NULL);

    return new_cycle;
}

/*
 * Looks up a gptp_cycle_t in the hash map.
 * @param map The hash map.
 * @param key The 64-bit key to look up.
 * @return Pointer to the gptp_cycle_t if found, otherwise NULL.
 */
gptp_cycle_t* gptp_cycle_map_lookup(const gptp_cycle_map_t *map, uint64_t key) {
    if (map == NULL) {
        return NULL;
    }

    size_t index = gptp_cycle_map_hash(map, key);
    cycle_map_entry_t *current = map->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            return current->cycle;
        }
        current = current->next;
    }
    return NULL;
}

/*
 * Removes a gptp_cycle_t from the hash map and frees its memory.
 * @param map The hash map.
 * @param key The 64-bit key of the cycle to remove.
 * @return 0 on success, -1 if key not found.
 */
int gptp_cycle_map_remove(gptp_cycle_map_t *map, uint64_t key) {
    if (map == NULL) {
        return -1;
    }

    size_t index = gptp_cycle_map_hash(map, key);
    cycle_map_entry_t *current = map->buckets[index];
    cycle_map_entry_t *prev = NULL;

    while (current != NULL) {
        if (current->key == key) {
            if (prev == NULL) {
                map->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->cycle);
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return -1;
}

/*
 * Frees all memory allocated by the gptp_cycle hash map.
 * @param map The hash map to free.
 */
void gptp_cycle_map_free(gptp_cycle_map_t *map) {
    if (map == NULL || map->buckets == NULL) {
        return;
    }

    for (size_t i = 0; i < map->size; i++) {
        cycle_map_entry_t *current = map->buckets[i];
        while (current != NULL) {
            cycle_map_entry_t *to_free = current;
            current = current->next;
            free(to_free->cycle);
            free(to_free);
        }
    }
    free(map->buckets);
    map->buckets = NULL;
    map->size = 0;
}

/*
 * Processes gPTP messages to track gPTP cycle states.
 * @param file_ctx The file context.
 * @param common_header The PTP common header.
 * @param packet_data Raw packet data.
 * @param data_length Length of the PTP message data.
 * @param eth_src_mac Source MAC address.
 * @param eth_dst_mac Destination MAC address.
 */
void process_gptp_message(file_context_t* file_ctx, const ptp_common_header_t* common_header, const uint8_t* packet_data, uint32_t data_length, const uint8_t* eth_src_mac, const uint8_t* eth_dst_mac) {
    (void)packet_data; 
    (void)data_length;
    (void)eth_src_mac;
    (void)eth_dst_mac;

    ptp_message_type_t messageType = common_header->transportSpecific_messageType & 0x0F;

    printf("        process_gptp_message called for message type: 0x%x\n", messageType);
}
