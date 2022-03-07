#include "algorithm.h"
#include "distance_map.h"
#include "fibonacci_heap.h"
#include "graph.h"
#include "list.h"
#include "parent_map.h"
#include "util.h"
#include "vertex_set.h"
#include <float.h>
#include <stdlib.h>

#define TRY_REPORT_RETURN_STATUS(RETURN_STATUS) \
if (p_return_status) {                          \
    *p_return_status = RETURN_STATUS;           \
}                                               \

#define CLEAN_SEARCH_STATE search_state_init(&search_state_)

static const size_t INITIAL_MAP_CAPACITY = 1024;
static const float LOAD_FACTOR = 1.3f;

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
            LOAD_FACTOR);

    p_state->p_open_backward =
        fibonacci_heap_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);

    p_state->p_closed_forward =
        vertex_set_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);
        
    p_state->p_closed_backward =
        vertex_set_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);

    p_state->p_distance_forward =
        distance_map_alloc( 
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);

    p_state->p_distance_backward =
        distance_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);

    p_state->p_parent_forward =
        parent_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);

    p_state->p_parent_backward =
        parent_map_alloc(
            INITIAL_MAP_CAPACITY,
            LOAD_FACTOR);
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

static list* traceback_path(size_t* p_touch_vertex,
                            parent_map* parent_forward,
                            parent_map* parent_backward) {

    list* path = list_alloc(100);
    /*size_t* p_current_vertex = p_touch_vertex;

    while (p_current_vertex) {
        list_push_front(path, p_current_vertex);
        p_current_vertex = parent_map_get(parent_forward, 
                                          p_current_vertex);
    }

    p_current_vertex = parent_map_get(parent_backward, 
                                      p_touch_vertex);

    while (p_current_vertex) {
        list_push_back(path, p_current_vertex);
        p_current_vertex = parent_map_get(parent_backward,   
                                             p_current_vertex);
    }*/

    return path;
}

list* find_shortest_path(Graph* p_graph,
                         size_t source_vertex_id,
                         size_t target_vertex_id,
                         int* p_return_status) {

    search_state search_state_;
    double best_path_length = DBL_MAX;
    double temporary_path_length;
    double tentative_length;
    double weight;
    size_t* p_touch_node_id = NULL;
    int return_status;
    int updated;
    size_t current_vertex_id;
    size_t child_vertex_id;
    size_t parent_vertex_id;
    GraphVertex* p_graph_vertex;

    fibonacci_heap* p_open_forward;
    fibonacci_heap* p_open_backward;
    vertex_set*     p_closed_forward;
    vertex_set*     p_closed_backward;
    distance_map*   p_distance_forward;
    distance_map*   p_distance_backward;
    parent_map*     p_parent_forward;
    parent_map*     p_parent_backward;

    weight_map_iterator* p_weight_map_children_iterator;
    weight_map_iterator* p_weight_map_parents_iterator;

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

    search_state_init(&search_state_);

    if (!search_state_ok(&search_state_)) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    p_open_forward      = search_state_.p_open_forward;
    p_open_backward     = search_state_.p_open_backward;
    p_closed_forward    = search_state_.p_closed_forward;
    p_closed_backward   = search_state_.p_closed_backward;
    p_distance_forward  = search_state_.p_distance_forward;
    p_distance_backward = search_state_.p_distance_backward;
    p_parent_forward    = search_state_.p_parent_forward;
    p_parent_backward   = search_state_.p_parent_backward;

    /* Initialize the state: */
    if (fibonacci_heap_add(p_open_forward, 
                           source_vertex_id, 
                           0.0) != RETURN_STATUS_OK) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }
    
    if (fibonacci_heap_add(p_open_backward,
                           target_vertex_id, 
                           0.0) != RETURN_STATUS_OK) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (distance_map_put(p_distance_forward, 
                         source_vertex_id, 
                         0.0) != RETURN_STATUS_OK) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (distance_map_put(p_distance_backward, 
                         target_vertex_id, 
                         0.0) != RETURN_STATUS_OK) {
        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (parent_map_put(p_parent_forward,
                       source_vertex_id, 
                       source_vertex_id) != RETURN_STATUS_OK) {

        CLEAN_SEARCH_STATE;
        TRY_REPORT_RETURN_STATUS(RETURN_STATUS_NO_MEMORY);
        return NULL;
    }

    if (parent_map_put(p_parent_backward,
                       target_vertex_id,
                       target_vertex_id) != RETURN_STATUS_OK) {

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

        if (fibonacci_heap_size(p_open_forward) +
            vertex_set_size(p_closed_forward) 
            <
            fibonacci_heap_size(p_open_backward) +
            vertex_set_size(p_closed_backward)) {

            current_vertex_id = fibonacci_heap_extract_min(p_open_forward);
            vertex_set_add(p_closed_forward, current_vertex_id);

            p_graph_vertex = 
                graph_vertex_map_get(p_graph->p_nodes, 
                                     current_vertex_id);

            p_weight_map_children_iterator = 
                weight_map_iterator_alloc(
                    p_graph_vertex->p_children);

            while (weight_map_iterator_has_next(
                    p_weight_map_children_iterator)) {

                updated = 0;

                weight_map_iterator_visit(
                    p_weight_map_children_iterator,
                    &child_vertex_id);

                weight_map_iterator_next(p_weight_map_children_iterator, 
                                         &child_vertex_id,
                                         &weight);

                if (vertex_set_contains(p_closed_forward, child_vertex_id)) {
                    continue;
                }

                /****************/
                tentative_length = distance_map_get�(p_distance_forward, 
                                                     current_vertex_id) + 
                                   weight;

                if (!distance_map_contains_vertex_id(p_distance_forward,
                                                     child_vertex_id)) {

                    distance_map_put(
                        p_distance_forward,
                        child_vertex_id,
                        tentative_length);

                    parent_map_put(
                        p_parent_forward,
                        child_vertex_id, 
                        current_vertex_id);

                    fibonacci_heap_add(
                        p_open_forward, 
                        child_vertex_id,
                        tentative_length);

                    updated = 1;
                }
                else if (distance_map_get(p_distance_forward, child_vertex_id) >
                    tentative_length) {

                    distance_map_put(
                        p_distance_forward,
                        child_vertex_id,
                        tentative_length);

                    parent_map_put(
                        p_parent_forward,
                        child_vertex_id,
                        current_vertex_id);

                    fibonacci_heap_decrease_key(
                        p_open_forward, 
                        child_vertex_id, 
                        tentative_length);

                    updated = 1;
                }

                if (updated) {
                    if (vertex_set_contains(p_closed_forward, child_vertex_id)) {
                        temporary_path_length = 
                            tentative_length + 
                            distance_map_get(p_distance_backward, 
                                             child_vertex_id);

                        if (best_path_length > temporary_path_length) {
                            best_path_length = temporary_path_length;
                            *p_touch_node_id = child_vertex_id;
                        }
                    }
                }
            }
            
        } else {
            current_vertex_id = fibonacci_heap_extract_min(p_open_backward);
            vertex_set_add(p_closed_backward, current_vertex_id);

            p_graph_vertex =
                graph_vertex_map_get(p_graph->p_nodes,
                    current_vertex_id);

            p_weight_map_parents_iterator = 
                weight_map_iterator_alloc(
                    p_graph_vertex->p_parents);

            while (weight_map_iterator_has_next(
                p_weight_map_parents_iterator)) {
                weight_map_iterator_visit(
                    p_weight_map_parents_iterator,
                    &visit_vertex_id);

                weight_map_iterator_next(p_weight_map_parents_iterator);

                if (vertex_set_contains(p_closed_backward,
                                        visit_vertex_id)) {
                    continue;
                }
                /*******************/
            }
        }

        fibonacci_heap_extract_min(p_open_forward);
    }


    return NULL;
}