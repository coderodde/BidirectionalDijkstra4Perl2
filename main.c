#include "algorithm.h"
#include "graph.h"
#include "list.h"
#include "vertex_set.h"
#include <stdio.h>
#include <stdlib.h>


static const size_t NODES = 1000;
static const size_t EDGES = 6000;

static void testRemoveNode()
{
    Graph* p_graph = malloc(sizeof(Graph));

    puts("--- testRemoveNode() begin.");

    initGraph(p_graph);

    addEdge(p_graph, 1, 2, 1.0);
    addEdge(p_graph, 2, 3, 2.0);
    addEdge(p_graph, 3, 1, 3.0);

    removeVertex(p_graph, 3);

    if (hasEdge(p_graph, 2, 3))
    {
        puts("Test 1 failed.");
        abort();
    }

    if (hasEdge(p_graph, 3, 1))
    {
        puts("Test 2 failed.");
        abort();
    }

    if (!hasEdge(p_graph, 1, 2))
    {
        puts("Test 4 failed.");
        abort();
    }

    if (getEdgeWeight(p_graph, 1, 2) != 1.0)
    {
        puts("Test 5 failed.");
        abort();
    }

    freeGraph(p_graph);
    puts("--- testRemoveNode() passed.");
}

double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

Graph* buildGraph() {
    Graph* p_graph = malloc(sizeof(Graph));
    size_t id1;
    size_t id2;
    double weight;
    size_t source_vertex_id;
    size_t target_vertex_id;
    list* path;

    initGraph(p_graph);

    srand((unsigned) time(NULL));

    for (size_t edge = 0; edge < EDGES; ++edge)
    {
        id1 = rand() % NODES;
        id2 = rand() % NODES;
        weight = randfrom(0.0, 10.0);
        addEdge(p_graph, id1, id2, weight);

        if (edge == 0) {
            source_vertex_id = id1;
        }
        else if (edge == NODES / 2) {
            target_vertex_id = id2;
        }
    }

    int result_status = 0;

    path = find_shortest_path(p_graph,
                              source_vertex_id,
                              target_vertex_id,
                              &result_status);

    printf("%d\n", result_status);

    return p_graph;
}

int equals(size_t a, size_t b) {
    return a == b;
}

size_t hash(size_t a) {
    return a;
}

int main(int argc, char* argv[])
{
    Graph* p_graph = buildGraph();

    vertex_set* pvs = vertex_set_alloc(10, 1.0f, hash, equals);

    testRemoveNode();

    freeGraph(p_graph);
    return 0;
}