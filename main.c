#include "algorithm.h"
#include "graph.h"
#include "vertex_list.h"
#include "vertex_set.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static clock_t milliseconds()
{
    return clock() / (CLOCKS_PER_SEC / 1000);
}

static const size_t NODES = 100 * 1000;
static const size_t EDGES = 500 * 1000;

static void testRemoveNode()
{
    Graph* graph = allocGraph();

    puts("--- testRemoveNode() begin.");

    addEdge(graph, 1, 2, 1.0);
    addEdge(graph, 2, 3, 2.0);
    addEdge(graph, 3, 1, 3.0);

    removeVertex(graph, 3);

    if (hasEdge(graph, 2, 3))
    {
        puts("Test 1 failed.");
        abort();
    }

    if (hasEdge(graph, 3, 1))
    {
        puts("Test 2 failed.");
        abort();
    }

    if (!hasEdge(graph, 1, 2))
    {
        puts("Test 3 failed.");
        abort();
    }

    if (getEdgeWeight(graph, 1, 2) != 1.0)
    {
        puts("Test 4 failed.");
        abort();
    }

    freeGraph(graph);
    puts("--- testRemoveNode() passed.");
}

static int paths_are_equal(vertex_list* path_1,
                           vertex_list* path_2) {
    size_t i;
    
    if (vertex_list_size(path_1) != vertex_list_size(path_2)) {
        return FALSE;
    }

    for (i = 0; i < vertex_list_size(path_1); ++i) {
        if (vertex_list_get(path_1, i) != vertex_list_get(path_2, i)) {
            return FALSE;
        }
    }

    return TRUE;
}

static double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

static size_t intrand() {
    size_t a = rand();
    size_t b = rand();
    return ((a << 15) | b);
}

double get_path_length(vertex_list* path,
                       Graph* graph) {
    size_t i;
    size_t vertex_id_1;
    size_t vertex_id_2;
    double length = 0.0;

    for (i = 0; i < vertex_list_size(path) - 1; ++i) {
        vertex_id_1 = vertex_list_get(path, i);
        vertex_id_2 = vertex_list_get(path, i + 1);

        length += getEdgeWeight(graph, 
                                vertex_id_1, 
                                vertex_id_2);
    }

    return length;
}

Graph* buildGraph() {
    Graph* p_graph = allocGraph();

    size_t i;
    size_t id1;
    size_t id2;
    double weight;
    size_t source_vertex_id = 0;
    size_t target_vertex_id = 0;
    clock_t milliseconds_a;
    clock_t milliseconds_b;
    vertex_list* path;
    vertex_list* path_2;
    int rs = -1;    
    unsigned random_seed;
    initGraph(p_graph);

    random_seed = 1; //(unsigned) time(NULL);
    srand(random_seed);
    printf("Seed = %d\n\n", random_seed);

    milliseconds_a = milliseconds();

    for (size_t edge = 0; edge < EDGES; ++edge)
    {
        id1 = intrand() % NODES;
        id2 = intrand() % NODES;
        weight = randfrom(0.0, 10.0);
        addEdge(p_graph, id1, id2, weight);

        if (edge == 0) {
            source_vertex_id = id1;
        }
        else if (edge == NODES / 2) {
            target_vertex_id = id2;
        }
    }

    milliseconds_b = milliseconds();
    printf("Built the graph in %d milliseconds.\n", 
           (milliseconds_b - milliseconds_a));

    printf("Source node: %zu\n", source_vertex_id);
    printf("Target node: %zu\n", target_vertex_id);

    puts("--- Bidirectional Dijkstra:");

    milliseconds_a = milliseconds();
    path = find_shortest_path(p_graph,
                              source_vertex_id,
                              target_vertex_id,
                              &rs);

    milliseconds_b = milliseconds();

    for (i = 0; i < vertex_list_size(path); ++i) {
        printf("%zu\n", vertex_list_get(path, i));
    }

    printf("Path length: %f\n", get_path_length(path, p_graph));
    printf("Duration: %d milliseconds.\n",
        (milliseconds_b - milliseconds_a));

    printf("Result status: %d\n\n", rs);
    puts("--- Original Dijkstra:");

    milliseconds_a = milliseconds();
    path_2 = find_shortest_path_2(p_graph,
                                  source_vertex_id,
                                  target_vertex_id,
                                  &rs);

    milliseconds_b = milliseconds();

    for (i = 0; i < vertex_list_size(path_2); ++i) {
        printf("%zu\n", vertex_list_get(path_2, i));
    }

    printf("Path length: %f\n", get_path_length(path_2, p_graph));
    printf("Duration: %d milliseconds.\n",
        (milliseconds_b - milliseconds_a));

    printf("Result status: %d\n", rs);

    printf("Algorithms agree: %d\n", paths_are_equal(path, path_2));

    vertex_list_free(path);
    vertex_list_free(path_2);
    return p_graph;
}

int main(int argc, char* argv[])
{
    Graph* p_graph = buildGraph();

    // vertex_set* pvs = vertex_set_alloc(10, 1.0f, hash, equals);

    // testRemoveNode();

    freeGraph(p_graph);
    return 0;
}