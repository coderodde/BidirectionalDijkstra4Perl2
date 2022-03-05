#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct distance_map distance_map;

    /***************************************************************************
    * Allocates a new, empty map with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    distance_map* distance_map_alloc
       (size_t   initial_capacity,
        float    load_factor,
        size_t (*hash_function)(void*),
        bool   (*equals_function)(void*, void*));

    /***************************************************************************
    * If p_map does not contain the key p_key, inserts it in the map,          *
    * associates value with it and return NULL. Otherwise updates the value    *
    * and returns the old value.                                               *
    ***************************************************************************/
    bool distance_map_put(distance_map* map, void* key, double value);

    /***************************************************************************
    * Returns a positive value if p_key is mapped to some value in this map.   *
    ***************************************************************************/
    bool distance_map_contains_key(distance_map* map, void* key);

    /***************************************************************************
    * Returns the value associated with the p_key, or NULL if p_key is not     *
    * mapped in the map.                                                       *
    ***************************************************************************/
    double distance_map_get(distance_map* map, void* key);

    /*************************************************************************
    Clears entirely the distance map.                    
    *************************************************************************/
    void unordered_map_free(distance_map* map);

#ifdef	__cplusplus
}
#endif

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H */
