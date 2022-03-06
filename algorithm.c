#include "algorithm.h"
#include "distance_map.h"
#include "fibonacci_heap.h"
#include "graph.h"
#include "list.h"
#include "parent_map.h"
#include "vertex_set.h"
#include <float.h>
#include <stdlib.h>

#define TRY_REPORT_RETURN_STATUS(RETURN_STATUS) \
if (p_return_status) {                          \
    *p_return_status = RETURN_STATUS;           \
}                                               \

#define CLEAN_SEARCH_STATE search_state_init(&search_state_)

static size_t hash_function(void* node_id) {
    return (size_t)node_id;
}

static int equals_function(void* node_1_id, void* node_2_id) {
    return node_1_id == node_2_id;
}

static int priority_compare_function(void* p_a, void* p_b) {
    double num_a = *(double*)p_a;
    double num_b = *(double*)(p_b);

    if (num_a < num_b) {
        return -1;
    }
    else if (num_a > num_b) {
        return 1;
    }
    else {
        return 0;
    }
}

typedef struct search_tate {
    fibonacci_heap* p_open_forward;
    fibonacci_heap* p_open_backward;
    vertex_set*     p_closed_forward;
    vertex_set*     p_closed_backward;
    distance_map*   p_distance_forward;
    distance_map*   p_distance_backward;
    parent_map*     p_parent_forward;
    parent_map*     p_parent_backward;
} search_state;

static void search_state_init(search_state* p_state) {
    p_state->p_open_forward =
        fibonacci_heap_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function,
            priority_compare_function);

    p_state->p_open_backward =
        fibonacci_heap_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function,
            priority_compare_function);

    p_state->p_closed_forward =
        vertex_set_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);
        
    p_state->p_closed_backward =
        vertex_set_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    p_state->p_distance_forward =
        distance_map_alloc( 
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    p_state->p_distance_backward =
        distance_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    p_state->p_parent_forward =
        parent_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    p_state->p_parent_backward =
        parent_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);
}

static int search_state_ok(search_state* p_search_state) {
    return p_search_state->p_open_forward &&
           p_search_state->p_open_backward &&
           p_search_state->p_closed_forward &&
           p_search_state->p_closed_backward &&
           p_search_state->p_distance_forward &&
           p_search_state->p_distance_backward &&
           p_search_state->p_parent_forward &&
           p_search_state->p_parent_backward;
}

static void search_state_free(search_state* p_search_state) {
    if (p_search_state->p_open_forward) {
        fibonacci_heap_free(p_search_state->p_open_forward);
    }

    if (p_search_state->p_open_backward) {
        fibonacci_heap_free(p_search_state->p_open_backward);
    }

    if (p_search_state->p_closed_forward) {
        vertex_set_free(p_search_state->p_closed_forward);
    }

    if (p_search_state->p_closed_backward) {
        vertex_set_free(p_search_state->p_closed_backward);
    }

    if (p_search_state->p_distance_forward) {
        distance_map_free(p_search_state->p_distance_forward);
    }

    if (p_search_state->p_distance_backward) {
        distance_map_free(p_search_state->p_distance_backward);
    }

    if (p_search_state->p_parent_forward) {
        parent_map_free(p_search_state->p_parent_forward);
    }

    if (p_search_state->p_parent_backward) {
        parent_map_free(p_search_state->p_parent_backward);
    }
}

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

    search_state search_state_;
    double best_path_length = DBL_MAX;
    double temporary_path_length;
    size_t* p_touch_node_id = NULL;
    int return_status;

    fibonacci_heap* p_open_forward;
    fibonacci_heap* p_open_backward;
    vertex_set*     p_closed_forward;
    vertex_set*     p_closed_backward;
    distance_map*   p_distance_forward;
    distance_map*   p_distance_backward;
    parent_map*     p_parent_forward;
    parent_map*     p_parent_backward;

    if (!p_graph) {
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_GRAPH);
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
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(return_status);
        return NULL;
    }

    if (!search_state_ok(&search_state_)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    fibonacci_heap* p_open_forward    = search_state_.p_open_forward;
    fibonacci_heap* p_open_backward   = search_state_.p_open_backward;
    vertex_set* p_closed_forward      = search_state_.p_closed_forward;
    vertex_set* p_closed_backward     = search_state_.p_closed_backward;
    distance_map* p_distance_forward  = search_state_.p_distance_forward;
    distance_map* p_distance_backward = search_state_.p_distance_backward;
    parent_map* p_parent_forward      = search_state_.p_parent_forward;
    parent_map* p_parent_backward     = search_state_.p_parent_backward;

    /* Initialize the state: */
    if (!fibonacci_heap_add(p_open_forward, source_vertex_id, (void*) 0)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }
    
    if (!fibonacci_heap_add(p_open_backward, target_vertex_id, (void*) 0)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (!unordered_map_put(p_distance_forward, source_vertex_id, (void*)0)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (!unordered_map_put(p_distance_backward, target_vertex_id, (void*)0)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (!unordered_map_put(p_parent_forward, source_vertex_id, NULL)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (!unordered_map_put(p_parent_backward, target_vertex_id, NULL)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    while (fibonacci_heap_size(p_open_forward) > 0 &&
           fibonacci_heap_size(p_open_backward) > 0) {
        if (p_touch_node_id) {
         
            temporary_path_length = 
                distance_map_get(p_distance_forward, 1) +
                distance_map_get(p_distance_backward, 2);

            if (temporary_path_length > best_path_length) {
                return traceback_path(p_touch_node_id,
                                      p_parent_forward,
                                      p_parent_backward);
            }
        }

        fibonacci_heap_extract_min(p_open_forward);
    }


    return NULL;
}