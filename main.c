#include "algorithm.h"
#include "graph.h"
#include "list.h"
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

double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

size_t intrand() {
    size_t a = rand();
    size_t b = rand();
    return ((a << 15) | b);
}

double get_path_length(list* path,
                       Graph* graph) {
    size_t i;
    size_t vertex_id_1;
    size_t vertex_id_2;
    double length = 0.0;

    for (i = 0; i < list_size(path) - 1; ++i) {
        vertex_id_1 = list_get(path, i);
        vertex_id_2 = list_get(path, i + 1);

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
    size_t source_vertex_id;
    size_t target_vertex_id;
    clock_t milliseconds_a;
    clock_t milliseconds_b;
    list* path;

    initGraph(p_graph);

    srand((unsigned) time(NULL));

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
    printf("Built the graph in %zu milliseconds.\n", 
           (milliseconds_b - milliseconds_a));

    int result_status = 0;

    milliseconds_a = milliseconds();
    path = find_shortest_path(p_graph,
                              source_vertex_id,
                              target_vertex_id,
                              &result_status);

    milliseconds_b = milliseconds();

    for (i = 0; i < list_size(path); ++i) {
        printf("%zu\n", list_get(path, i));
    }

    printf("Path length: %f\n", get_path_length(path, p_graph));
    printf("Duration: %d milliseconds.\n",
           (milliseconds_b - milliseconds_a));

    printf("Result status: %d\n", result_status);
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