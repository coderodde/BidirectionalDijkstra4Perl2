#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_PARENT_MAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_PARENT_MAP_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct parent_map_entry {
        size_t vertex_id;
        size_t predecessor_vertex_id;
        struct parent_map_entry* chain_next;
        struct parent_map_entry* prev;
        struct parent_map_entry* next;
    } parent_map_entry;

    typedef struct parent_map {
        parent_map_entry** table;
        parent_map_entry* head;
        parent_map_entry* tail;
        size_t             table_capacity;
        size_t             size;
        size_t             max_allowed_size;
        size_t             mask;
        float              load_factor;
    } parent_map;

    /***************************************************************************
    * Allocates a new, empty map with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    parent_map* parent_map_alloc(size_t initial_capacity,
                                 float load_factor);

    /***************************************************************************
    * If p_map does not contain the key p_key, inserts it in the map,          *
    * associates value with it and return NULL. Otherwise updates the value    *
    * and returns the old value.                                               *
    ***************************************************************************/
    int parent_map_put(parent_map* map,
                       size_t vertex_id,
                       size_t predecessor_vertex_id);

    /***************************************************************************
    * Returns the value associated with the p_key, or NULL if p_key is not     *
    * mapped in the map.                                                       *
    ***************************************************************************/
    double parent_map_get(parent_map* map, size_t vertex_id);

    /*************************************************************************
    Clears entirely the distance map.
    *************************************************************************/
    void parent_map_free(parent_map* map);

#ifdef	__cplusplus
}
#endif

#endif	/* #ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_PARENT_MAP_H */