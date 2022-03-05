#include "algorithm.h"
#include "fibonacci_heap.h"
#include "graph.h"
#include "list.h"
#include "unordered_map.h"
#include "unordered_set.h"
#include <float.h>
#include <stdlib.h>

static const size_t INITIAL_MAP_CAPACITY = 1024;
static const float LOAD_FACTOR = 1.3f;

extern const int RETURN_STATUS_OK               = 0;
extern const int RETURN_STATUS_NO_PATH          = 1;
extern const int RETURN_STATUS_NO_MEMORY        = 2;
extern const int RETURN_STATUS_NO_GRAPH         = 3;
extern const int RETURN_STATUS_NO_SOURCE_VERTEX = 4;
extern const int RETURN_STATUS_NO_TARGET_VERTEX = 8;

static size_t hash_function(void* node_id) {
    return (size_t) node_id;
}

static int equals_function(void* node_1_id, void* node_2_id) {
    return node_1_id == node_2_id;
}

static int priority_compare_function(void* a, void* b) {
    double* p_a = (double*) a;
    double* p_b = (double*) b;

    double num_a = *p_a;
    double num_b = *p_b;

    if (num_a < num_b) {
        return -1;
    } else if (num_a > num_b) {
        return 1;
    } else {
        return 0;
    }
}

list* find_shortest_path(Graph* p_graph,
                         size_t source_vertex_id,
                         size_t target_vertex_id,
                         int* p_return_status) {
    fibonacci_heap* open_forward;
    fibonacci_heap* open_backward;

    unordered_set* closed_forward;
    unordered_set* closed_backward;

    unordered_map* distance_forward;
    unordered_map* distance_backward;

    unordered_map* parent_forward;
    unordered_map* parent_backward;

    double best_path_length = DBL_MAX;
    size_t* p_touch_node_id = NULL;
    int return_status;

    if (!p_graph) {
        *p_return_status = RETURN_STATUS_NO_GRAPH;
        return NULL;
    }

    return_status = 0;

    if (!hasVertex(p_graph, source_vertex_id)) {
        return_status |= RETURN_STATUS_NO_SOURCE_VERTEX;
    }

    if (!hasVertex(p_graph, target_vertex_id)) {
        return_status |= RETURN_STATUS_NO_TARGET_VERTEX;
    }

    if (return_status) {
        *p_return_status = return_status;
        return NULL;
    }

    open_forward = 
        fibonacci_heap_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function,
            priority_compare_function);

    if (!open_forward) {
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    open_backward = 
        fibonacci_heap_alloc(INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function,
            priority_compare_function);

    if (!open_backward) {
        fibonacci_heap_free(open_forward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    closed_forward = 
        unordered_set_alloc(INITIAL_MAP_CAPACITY,
                            LOAD_FACTOR,
                            hash_function,
                            equals_function);

    if (!closed_forward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    closed_backward = 
        unordered_set_alloc(INITIAL_MAP_CAPACITY,
                            LOAD_FACTOR,
                            hash_function,
                            equals_function);

    if (!closed_backward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        unordered_set_free(closed_forward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    distance_forward =
        unordered_map_alloc(INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    if (!distance_forward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        unordered_set_free(closed_forward);
        unordered_set_free(closed_backward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    distance_backward =
        unordered_map_alloc(INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    if (!distance_backward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        unordered_set_free(closed_forward);
        unordered_set_free(closed_backward);
        unordered_map_free(distance_forward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    parent_forward =
        unordered_map_alloc(INITIAL_MAP_CAPACITY,
                            LOAD_FACTOR,
                            hash_function,
                            equals_function);

    if (!parent_forward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        unordered_set_free(closed_forward);
        unordered_set_free(closed_backward);
        unordered_map_free(distance_forward);
        unordered_map_free(distance_backward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    parent_backward =
        unordered_map_alloc(INITIAL_MAP_CAPACITY,
                            LOAD_FACTOR,
                            hash_function,
                            equals_function);

    if (!parent_backward) {
        fibonacci_heap_free(open_forward);
        fibonacci_heap_free(open_backward);
        unordered_set_free(closed_forward);
        unordered_set_free(closed_backward);
        unordered_map_free(distance_forward);
        unordered_map_free(distance_backward);
        unordered_map_free(parent_forward);
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }


    /* Clean the state: */
    fibonacci_heap_free(open_forward);
    fibonacci_heap_free(open_backward);
    unordered_set_free(closed_forward);
    unordered_set_free(closed_backward);
    unordered_map_free(distance_forward);
    unordered_map_free(distance_backward);
    unordered_map_free(parent_forward);
    unordered_map_free(parent_backward);
    return NULL;
}