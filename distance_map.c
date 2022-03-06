#include "distance_map.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct distance_map_entry {
    size_t vertex_id;
    double distance;
    struct distance_map_entry* chain_next;
    struct distance_map_entry* prev;
    struct distance_map_entry* next;
} distance_map_entry;

typedef struct distance_map {
    distance_map_entry** table;
    distance_map_entry* head;
    distance_map_entry* tail;
    size_t              table_capacity;
    size_t              size;
    size_t              max_allowed_size;
    size_t              mask;
    float               load_factor;
} distance_map;

static distance_map_entry* 
distance_map_entry_alloc(size_t vertex_id,
                         double distance)
{
    distance_map_entry* entry = malloc(sizeof(*entry));

    if (!entry)
    {
        return NULL;
    }

    entry->vertex_id = vertex_id;
    entry->distance = distance;
    entry->chain_next = NULL;
    entry->next = NULL;
    entry->prev = NULL;

    return entry;
}

static const float  MINIMUM_LOAD_FACTOR = 0.2f;
static const size_t MINIMUM_INITIAL_CAPACITY = 16;

static float maxf(float a, float b)
{
    return a < b ? b : a;
}

static int maxi(int a, int b)
{
    return a < b ? b : a;
}

/*******************************************************************************
* Makes sure that the load factor is no less than a minimum threshold.         *
*******************************************************************************/
static float fix_load_factor(float load_factor)
{
    return maxf(load_factor, MINIMUM_LOAD_FACTOR);
}

/*******************************************************************************
* Makes sure that the initial capacity is no less than a minimum allowed and   *
* is a power of two.                                                           *
*******************************************************************************/
static size_t fix_initial_capacity(size_t initial_capacity)
{
    size_t ret;

    initial_capacity = maxi(initial_capacity, MINIMUM_INITIAL_CAPACITY);
    ret = 1;

    while (ret < initial_capacity)
    {
        ret <<= 1;
    }

    return ret;
}

distance_map* distance_map_alloc(size_t initial_capacity,
                                 float load_factor)
{
    distance_map* map = malloc(sizeof(*map));

    if (!map)
    {
        return NULL;
    }

    load_factor = fix_load_factor(load_factor);
    initial_capacity = fix_initial_capacity(initial_capacity);

    map->load_factor = load_factor;
    map->table_capacity = initial_capacity;
    map->size = 0;
    map->head = NULL;
    map->tail = NULL;
    map->table = calloc(initial_capacity,
                        sizeof(distance_map_entry*));

    map->mask = initial_capacity - 1;
    map->max_allowed_size = (size_t)(initial_capacity * load_factor);

    return map;
}

static void ensure_capacity(distance_map* map)
{
    size_t new_capacity;
    size_t new_mask;
    size_t index;
    distance_map_entry* entry;
    distance_map_entry** new_table;

    if (map->size < map->max_allowed_size)
    {
        return;
    }

    new_capacity = 2 * map->table_capacity;
    new_mask = new_capacity - 1;
    new_table = calloc(new_capacity, sizeof(distance_map_entry*));

    if (!new_table)
    {
        return;
    }

    /* Rehash the entries. */
    for (entry = map->head; entry; entry = entry->next)
    {
        index = entry->vertex_id & new_mask;
        entry->chain_next = new_table[index];
        new_table[index] = entry;
    }

    free(map->table);

    map->table = new_table;
    map->table_capacity = new_capacity;
    map->mask = new_mask;
    map->max_allowed_size = (size_t)(new_capacity * map->load_factor);
}

bool distance_map_put(distance_map* map, size_t vertex_id, double distance)
{
    size_t index;
    size_t hash_value;
    void* old_value;
    distance_map_entry* entry;

    if (!map)
    {
        return false;
    }

    hash_value = vertex_id;
    index = hash_value & map->mask;

    for (entry = map->table[index]; entry; entry = entry->chain_next)
    {
        if (entry->vertex_id == vertex_id)
        {
            entry->distance = distance;
            return true;
        }
    }

    ensure_capacity(map);

    /* Recompute the index since it is possibly changed by 'ensure_capacity' */
    index = hash_value & map->mask;
    entry = distance_map_entry_alloc(vertex_id, distance);

    if (!entry) {
        return false;
    }

    entry->chain_next = map->table[index];
    map->table[index] = entry;

    /* Link the new entry to the tail of the list. */
    if (!map->tail)
    {
        map->head = entry;
        map->tail = entry;
    }
    else
    {
        map->tail->next = entry;
        entry->prev = map->tail;
        map->tail = entry;
    }

    map->size++;
    return true;
}

bool distance_map_contains_key(distance_map* map, size_t vertex_id)
{
    size_t index;
    distance_map_entry* entry;

    if (!map)
    {
        return false;
    }

    index = vertex_id & map->mask;

    for (entry = map->table[index]; entry; entry = entry->chain_next)
    {
        if (vertex_id == entry->vertex_id)
        {
            return true;
        }
    }

    return false;
}

double distance_map_get(distance_map* map, size_t vertex_id)
{
    size_t index;
    distance_map_entry* p_entry;

    if (!map)
    {
        abort();
    }

    index = vertex_id & map->mask;

    for (p_entry = map->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (vertex_id == p_entry->vertex_id)
        {
            return p_entry->distance;
        }
    }

    abort();
    return 0.0; /* Compiler, shut up! */
}

static void distance_map_clear(distance_map* map)
{
    distance_map_entry* entry;
    distance_map_entry* next_entry;
    size_t index;

    if (!map)
    {
        return;
    }

    entry = map->head;

    while (entry)
    {
        index = entry->vertex_id & map->mask;
        next_entry = entry->next;
        free(entry);
        entry = next_entry;
        map->table[index] = NULL;
    }

    map->size = 0;
    map->head = NULL;
    map->tail = NULL;
}

void distance_map_free(distance_map* map)
{
    if (!map)
    {
        return;
    }

    distance_map_clear(map);
    free(map->table);
    free(map);
}
