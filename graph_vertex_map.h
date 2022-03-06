#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_GRAPH_VERTEX_MAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_GRAPH_VERTEX_MAP_H

#include "graph.h"
#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct GraphVertex GraphVertex;
    typedef struct graph_vertex_map graph_vertex_map;
    typedef struct graph_vertex_map_entry graph_vertex_map_entry;
    typedef struct graph_vertex_map_iterator graph_vertex_map_iterator;

    /***************************************************************************
    * Allocates a new, empty map with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    graph_vertex_map* graph_vertex_map_alloc(
        size_t initial_capacity,
        float load_factor);

    /***************************************************************************
    * If p_map does not contain the key p_key, inserts it in the map,          *
    * associates value with it and return NULL. Otherwise updates the value    *
    * and returns the old value.                                               *
    ***************************************************************************/
    int graph_vertex_map_put(graph_vertex_map* map,
                             size_t vertex_id, 
                             GraphVertex* vertex);

    /***************************************************************************
    * Returns a positive value if p_key is mapped to some value in this map.   *
    ***************************************************************************/
    int graph_vertex_map_contains_key(graph_vertex_map* map, size_t vertex_id);

    /***************************************************************************
    * Returns the value associated with the p_key, or NULL if p_key is not     *
    * mapped in the map.                                                       *
    ***************************************************************************/
    GraphVertex* graph_vertex_map_get(graph_vertex_map * map, 
                                      size_t vertex_id);

    int graph_vertex_map_remove(graph_vertex_map* map, size_t vertex_id);

    /*************************************************************************
    Clears entirely the distance map.
    *************************************************************************/
    void graph_vertex_map_free(graph_vertex_map* map);

    graph_vertex_map_iterator* graph_vertex_map_iterator_alloc
    (graph_vertex_map* map);

    /***************************************************************************
    * Returns the number of keys not yet iterated over.                        *
    ***************************************************************************/
    int graph_vertex_map_iterator_has_next
    (graph_vertex_map_iterator* iterator);

    /***************************************************************************
    * Loads the next entry in the iteration order.                             *
    ***************************************************************************/
    int unordered_map_iterator_next(
        graph_vertex_map_iterator* iterator,
        size_t* key_pointer,
        GraphVertex** value_pointer);

    /***************************************************************************
    * Deallocates the map iterator.                                            *
    ***************************************************************************/
    void graph_vertex_map_iterator_free(
        graph_vertex_map_iterator* iterator);


#ifdef	__cplusplus
}
#endif

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_DISTANCE_MAP_H */
