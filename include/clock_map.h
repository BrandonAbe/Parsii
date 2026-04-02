#ifndef CLOCK_MAP_H
#define CLOCK_MAP_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Represents a single entry in a hash map bucket.
 */
typedef struct map_entry_s {
    uint64_t key;
    uint64_t value;
    struct map_entry_s *next;
} map_entry_t;

/**
 * @brief Represents the clock hash map.
 */
typedef struct {
    size_t size;
    map_entry_t **buckets;
} clock_map_t;

/* Function Prototypes for Clock Map */
int clock_map_init(clock_map_t *map);
int clock_map_insert(clock_map_t *map, uint64_t key, uint64_t value);
int clock_map_lookup(const clock_map_t *map, uint64_t key, uint64_t *value_out);
void clock_map_free(clock_map_t *map);

#endif // CLOCK_MAP_H