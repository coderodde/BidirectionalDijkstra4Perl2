#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H

#include <stdlib.h>

    typedef struct distance_map_entry {
        size_t                     vertex_id;
        double                     distance;
        struct distance_map_entry* chain_next;
        struct distance_map_entry* prev;
        struct distance_map_entry* next;
    } distance_map_entry;

    typedef struct distance_map {
        distance_map_entry** table;
        distance_map_entry*  head;
        distance_map_entry*  tail;
        size_t               table_capacity;
        size_t               size;
        size_t               max_allowed_size;
        size_t               mask;
        float                load_factor;
    } distance_map;

    /***************************************************************************
    * Allocates a new, empty map with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    distance_map* distance_map_alloc(size_t initial_capacity, 
                                     float load_factor);

    /***************************************************************************
    * If p_map does not contain the key p_key, inserts it in the map,          *
    * associates value with it and return NULL. Otherwise updates the value    *
    * and returns the old value.                                               *
    ***************************************************************************/
    int distance_map_put(distance_map* map, 
                         size_t vertex_id,
                         double distance);

    /***************************************************************************
    * Returns a positive value if p_key is mapped to some value in this map.   *
    ***************************************************************************/
    int distance_map_contains_vertex_id(distance_map* map, 
                                        size_t vertex_id);

    /***************************************************************************
    * Returns the value associated with the p_key, or NULL if p_key is not     *
    * mapped in the map.                                                       *
    ***************************************************************************/
    double distance_map_get(distance_map* map, size_t vertex_id);

    /*************************************************************************
    Clears entirely the distance map.                    
    *************************************************************************/
    void unordered_map_free(distance_map* map);

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H */
