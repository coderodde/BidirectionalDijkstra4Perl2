#include "graph.h"
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

    initGraph(p_graph);

    srand((unsigned) time(NULL));

    for (size_t edge = 0; edge < EDGES; ++edge)
    {
        id1 = rand() % NODES;
        id2 = rand() % NODES;
        weight = randfrom(0.0, 10.0);
        addEdge(p_graph, id1, id2, weight);
    }

    return p_graph;
}

int main(int argc, char* argv[])
{
    Graph* p_graph = buildGraph();

    testRemoveNode();

    freeGraph(p_graph);
    return 0;
}