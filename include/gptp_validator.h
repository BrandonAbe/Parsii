#ifndef GPTP_VALIDATOR_H
#define GPTP_VALIDATOR_H

#include <stdint.h>
#include <time.h> 

// Enum to represent the state of a gPTP cycle
typedef enum {
    GPTP_CYCLE_STATE_INIT = 0,
    GPTP_CYCLE_STATE_SYNC_RECEIVED,
    GPTP_CYCLE_STATE_FOLLOW_UP_RECEIVED,
    GPTP_CYCLE_STATE_PDELAY_REQ_RECEIVED,
    GPTP_CYCLE_STATE_PDELAY_RESP_RECEIVED,
    GPTP_CYCLE_STATE_COMPLETE,
    GPTP_CYCLE_STATE_INVALID
} gptp_cycle_state_t;

/**
 * @brief Stores information about a gPTP cycle.
 */
typedef struct gptp_cycle_s {
    uint64_t master_id; 
    uint64_t slave_id;  
    uint16_t sequence_id; 

    // Timestamps and correction fields for Sync message
    uint64_t sync_ts_s;
    uint32_t sync_ts_ns;
    uint64_t sync_correction_field;
    uint8_t  sync_mac_src[6];
    uint8_t  sync_mac_dst[6];
    
    // Timestamps and correction fields for Follow_Up message
    uint64_t follow_up_ts_s;
    uint32_t follow_up_ts_ns;
    uint64_t follow_up_correction_field;

    // Timestamps and correction fields for Pdelay_Req message
    uint64_t pdelay_req_ts_s;
    uint32_t pdelay_req_ts_ns;
    uint64_t pdelay_req_correction_field;
    uint8_t  pdelay_req_mac_src[6];
    uint8_t  pdelay_req_mac_dst[6];

    // Timestamps and correction fields for Pdelay_Resp message
    uint64_t pdelay_resp_ts_s;
    uint32_t pdelay_resp_ts_ns;
    uint64_t pdelay_resp_correction_field;
    uint8_t  pdelay_resp_mac_src[6];
    uint8_t  pdelay_resp_mac_dst[6];

    gptp_cycle_state_t state; 
    time_t last_update_time; 

    struct gptp_cycle_s *next; // For linked list in hash map bucket
} gptp_cycle_t;

/**
 * @brief Entry in the gPTP cycle hash map.
 */
typedef struct cycle_map_entry_s {
    uint64_t key;               
    gptp_cycle_t *cycle;        
    struct cycle_map_entry_s *next;   
} cycle_map_entry_t;

/**
 * @brief Hash map for tracking gPTP cycles.
 */
typedef struct {
    size_t size;                
    cycle_map_entry_t **buckets; 
} gptp_cycle_map_t;

/* Function Prototypes for gPTP Cycle Map */
int gptp_cycle_map_init(gptp_cycle_map_t *map);
uint64_t create_cycle_key(const uint8_t source_port_identity[10], uint16_t sequence_id, int swap_bytes);
gptp_cycle_t* gptp_cycle_map_insert_or_get(gptp_cycle_map_t *map, uint64_t key);
gptp_cycle_t* gptp_cycle_map_lookup(const gptp_cycle_map_t *map, uint64_t key);
int gptp_cycle_map_remove(gptp_cycle_map_t *map, uint64_t key);
void gptp_cycle_map_free(gptp_cycle_map_t *map);

#endif // GPTP_VALIDATOR_H
