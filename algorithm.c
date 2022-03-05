#include "algorithm.h"
#include "fibonacci_heap.h"
#include "graph.h"
#include "list.h"
#include "unordered_map.h"
#include "unordered_set.h"
#include <float.h>
#include <stdlib.h>

#define CLEANUP                        \
fibonacci_heap_free(open_forward);     \
fibonacci_heap_free(open_backward);    \
unordered_set_free(closed_forward);    \
unordered_set_free(closed_backward);   \
unordered_map_free(distance_forward);  \
unordered_map_free(distance_backward); \
unordered_map_free(parent_forward);    \
unordered_map_free(parent_backward);   \


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

static int priority_compare_function(void* p_a, void* p_b) {
    double num_a = *(double*) p_a;
    double num_b = *(double*)(p_b);

    if (num_a < num_b) {
        return -1;
    } else if (num_a > num_b) {
        return 1;
    } else {
        return 0;
    }
}

static list* traceback_path(size_t* p_touch_vertex,
                            unordered_map* parent_forward,
                            unordered_map* parent_backward) {

    list* path = list_alloc(100);
    size_t* p_current_vertex = p_touch_vertex;

    while (p_current_vertex) {
        list_push_front(path, p_current_vertex);
        p_current_vertex = unordered_map_get(parent_forward, 
                                             p_current_vertex);
    }

    p_current_vertex = unordered_map_get(parent_backward, p_touch_vertex);

    while (p_current_vertex) {
        list_push_back(path, p_current_vertex);
        p_current_vertex = unordered_map_get(parent_backward,   
                                             p_current_vertex);
    }

    return path;
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
    double temporary_path_length;
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

    /* Initialize the state: */
    if (!fibonacci_heap_add(open_forward, source_vertex_id, (void*) 0)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }
    
    if (!fibonacci_heap_add(open_backward, target_vertex_id, (void*) 0)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    if (!unordered_map_put(distance_forward, source_vertex_id, (void*)0)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    if (!unordered_map_put(distance_backward, target_vertex_id, (void*)0)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    if (!unordered_map_put(parent_forward, source_vertex_id, NULL)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    if (!unordered_map_put(parent_backward, target_vertex_id, NULL)) {
        CLEANUP;
        *p_return_status = RETURN_STATUS_NO_MEMORY;
        return NULL;
    }

    while (fibonacci_heap_size(open_forward) > 0 &&
           fibonacci_heap_size(open_backward) > 0) {
        if (p_touch_node_id) {
         
            temporary_path_length = 
                (double) 
                unordered_map_get(
                    distance_forward,
                    (size_t) fibonacci_heap_min(open_forward))
                +
                (double) 
                unordered_map_get(
                    distance_backward,
                    (size_t) fibonacci_heap_min(open_backward));

            if (temporary_path_length > best_path_length) {
                return traceback_path(p_touch_node_id,
                                      parent_forward,
                                      parent_backward);
            }
        }

        fibonacci_heap_extract_min(open_forward);
    }

    /* Clean the state: */
    CLEANUP;
    return NULL;
}