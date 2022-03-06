#ifndef FIBONACCI_HEAP_H
#define	FIBONACCI_HEAP_H

#include "util.h"
#include <stdbool.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct fibonacci_heap fibonacci_heap;
    typedef struct fibonacci_heap_node fibonacci_heap_node;
    typedef struct heap_node_map_entry heap_node_map_entry;
    typedef struct heap_node_map heap_node_map;

    /***************************************************************************
    * Allocates a new empty Fibonacci heap.                                    *
    ***************************************************************************/
    fibonacci_heap* fibonacci_heap_alloc(size_t initial_capacity,
                                         float  load_factor);

    /***************************************************************************
    * Adds a new element and its priority to the heap only if it is not        *
    * already present. Returns true only if the input element was not already  *
    * in the heap.                                                             *
    ***************************************************************************/
    int fibonacci_heap_add(fibonacci_heap* heap,
                           size_t vertex_id,
                           double priority);

    /***************************************************************************
    * Attempts to assign a higher priority to the element. Returns true only   *
    * if the structure of the heap changed due to this call.                   *
    ***************************************************************************/
    bool fibonacci_heap_decrease_key(fibonacci_heap* heap,
                                     size_t vertex_id,
                                     double new_priority);

    /***************************************************************************
    * Returns true only if the element is in the heap.                         *
    ***************************************************************************/
    bool fibonacci_heap_contains_key(fibonacci_heap* heap, size_t vertex_id);

    /***************************************************************************
    * Removes the highest priority element and returns it.                     *
    ***************************************************************************/
    size_t fibonacci_heap_extract_min(fibonacci_heap* heap);

    /***************************************************************************
    * Returns the highest priority element without removing it.                *
    ***************************************************************************/
    size_t fibonacci_heap_min(fibonacci_heap* heap);

    /***************************************************************************
    * Returns the size of this heap.                                           *
    ***************************************************************************/
    size_t fibonacci_heap_size(fibonacci_heap* heap);

    /***************************************************************************
    * Drops all the contents of the heap. Only internal structures are         *
    * deallocated; the user is responsible for memory-managing the contents.   *
    ***************************************************************************/
    void fibonacci_heap_clear(fibonacci_heap* heap);

    /***************************************************************************
    * Deallocates the entire heap with its internal structures. The client     *
    * programmer must, however, memory-manage the contents.                    *
    ***************************************************************************/
    void fibonacci_heap_free(fibonacci_heap* heap);

    bool fibonacci_heap_is_healthy(fibonacci_heap* heap);

#ifdef	__cplusplus
}
#endif

#endif	/* FIBONACCI_HEAP_H */