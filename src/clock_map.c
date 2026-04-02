#include "../include/clock_map.h"
#include <stdlib.h>
#include <stdio.h>

#define CLOCK_MAP_INITIAL_SIZE 1024 // Initial number of buckets for the hash map

/*
 * Initializes a new clock hash map.
 * @param map Pointer to the clock_map_t structure.
 * @return 0 on success, -1 on memory allocation failure.
 */
int clock_map_init(clock_map_t *map) {
    if (map == NULL) {
        return -1;
    }
    map->size = CLOCK_MAP_INITIAL_SIZE;
    map->buckets = (map_entry_t **)calloc(map->size, sizeof(map_entry_t *));
    if (map->buckets == NULL) {
        perror("Failed to allocate memory for clock map buckets");
        return -1;
    }
    return 0;
}

/*
 * Simple hash function for a 64-bit key.
 * @param map The hash map.
 * @param key The 64-bit key.
 * @return The hash index.
 */
static size_t clock_map_hash(const clock_map_t *map, uint64_t key) {
    return (size_t)(key % map->size);
}

/*
 * Inserts a key-value pair into the hash map. Updates value if key exists.
 * @param map The hash map.
 * @param key The 64-bit key to insert.
 * @param value The 64-bit value.
 * @return 0 on success, -1 on memory allocation failure.
 */
int clock_map_insert(clock_map_t *map, uint64_t key, uint64_t value) {
    if (map == NULL) {
        return -1;
    }

    size_t index = clock_map_hash(map, key);
    map_entry_t *current = map->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            current->value = value;
            return 0;
        }
        current = current->next;
    }

    map_entry_t *new_entry = (map_entry_t *)malloc(sizeof(map_entry_t));
    if (new_entry == NULL) {
        perror("Failed to allocate memory for map entry");
        return -1;
    }
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
    return 0;
}

/*
 * Looks up a value by key in the hash map.
 * @param map The hash map.
 * @param key The 64-bit key to look up.
 * @param value_out Pointer to store the found value.
 * @return 0 if key found, -1 otherwise.
 */
int clock_map_lookup(const clock_map_t *map, uint64_t key, uint64_t *value_out) {
    if (map == NULL || value_out == NULL) {
        return -1;
    }

    size_t index = clock_map_hash(map, key);
    map_entry_t *current = map->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            *value_out = current->value;
            return 0;
        }
        current = current->next;
    }
    return -1;
}

/*
 * Frees all memory allocated by the hash map.
 * @param map The hash map to free.
 */
void clock_map_free(clock_map_t *map) {
    if (map == NULL || map->buckets == NULL) {
        return;
    }

    for (size_t i = 0; i < map->size; i++) {
        map_entry_t *current = map->buckets[i];
        while (current != NULL) {
            map_entry_t *to_free = current;
            current = current->next;
            free(to_free);
        }
    }
    free(map->buckets);
    map->buckets = NULL;
    map->size = 0;
}
