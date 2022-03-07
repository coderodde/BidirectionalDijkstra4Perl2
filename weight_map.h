#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_WEIGHT_MAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_WEIGHT_MAP_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct weight_map_entry weight_map_entry;
    typedef struct weight_map weight_map;
    typedef struct weight_map_iterator weight_map_iterator;

    /***************************************************************************
    * Allocates a new, empty map with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    weight_map* weight_map_alloc(size_t initial_capacity,
                                 float load_factor);

    /***************************************************************************
    * If p_map does not contain the key p_key, inserts it in the map,          *
    * associates p_value with it and return NULL. Otherwise updates the value  *
    * and returns the old value.                                               *
    ***************************************************************************/
    int weight_map_put(weight_map* map, size_t vertex_id, double weight);

    /***************************************************************************
    * Returns a positive value if p_key is mapped to some value in this map.   *
    ***************************************************************************/
    int weight_map_contains_key(weight_map* map, size_t vertex_id);

    /***************************************************************************
    * Returns the value associated with the p_key, or NULL if p_key is not     *
    * mapped in the map.                                                       *
    ***************************************************************************/
    double weight_map_get(weight_map* map, size_t vertex_id);

    /***************************************************************************
    * If p_key is mapped in the map, removes the mapping and returns the value *
    * of that mapping. If the map did not contain the mapping, returns NULL.   *
    ***************************************************************************/
    void weight_map_remove(weight_map* map, size_t vertex_id);

    /***************************************************************************
    * Removes all the contents of the map.                                     *
    ***************************************************************************/
    void weight_map_clear(weight_map* map);

    /***************************************************************************
    * Returns the size of the map, or namely, the amount of key/value mappings *
    * in the map.                                                              *
    ***************************************************************************/
    size_t weight_map_size(weight_map* map);

    /***************************************************************************
    * Deallocates the entire map. Only the map and its nodes are deallocated.  *
    * The user is responsible for deallocating the actual data stored in the   *
    * map.                                                                     *
    ***************************************************************************/
    void weight_map_free(weight_map* map);

    /***************************************************************************
    * Returns the iterator over the map. The entries are iterated in insertion *
    * order.                                                                   *
    ***************************************************************************/
    weight_map_iterator* weight_map_iterator_alloc
    (weight_map* map);

    /***************************************************************************
    * Returns the number of keys not yet iterated over.                        *
    ***************************************************************************/
    int weight_map_iterator_has_next
    (weight_map_iterator* iterator);

    /***************************************************************************
    * Loads the next entry in the iteration order.                             *
    ***************************************************************************/
    int weight_map_iterator_next(weight_map_iterator* iterator,
                                 size_t* vertex_id_pointer,
                                 double* weight_pointer);

    /***************************************************************************
    * Deallocates the map iterator.                                            *
    ***************************************************************************/
    void weight_map_iterator_free(weight_map_iterator* iterator);

    void weight_map_iterator_visit(weight_map_iterator* p_iterator,
                                   size_t* p_vertex_id);

    void weight_map_iterator_remove(weight_map_iterator* p_iterator);

#ifdef	__cplusplus
}
#endif

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_WEIGHT_MAP_H */
