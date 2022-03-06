#include "graph.h"

static const size_t initial_capacity = 1024;
static const float load_factor = 1.3f;

void initGraphVertex(GraphVertex* p_graph_vertex, size_t id)
{
	p_graph_vertex->p_children =
		unordered_map_alloc(initial_capacity,
			load_factor,
			hash_function,
			equals_function);

	p_graph_vertex->p_parents =
		unordered_map_alloc(initial_capacity,
			load_factor,
			hash_function,
			equals_function);

	p_graph_vertex->id = id;
}

void freeGraphVertex(GraphVertex* p_graph_vertex)
{
	unordered_map_free(p_graph_vertex->p_children);
	unordered_map_free(p_graph_vertex->p_parents);
}

void initGraph(Graph* p_graph)
{
	p_graph->p_nodes =
		graph_vertex_alloc(initial_capacity, 
						   load_factor);
}

void freeGraph(Graph* p_graph)
{
	unordered_map_iterator* p_iterator =
		unordered_map_iterator_alloc(p_graph->p_nodes);

	GraphVertex* p_graph_vertex;

	while (unordered_map_iterator_has_next(p_iterator))
	{
		void* p_key;
		void* p_value;

		unordered_map_iterator_next(
			p_iterator,
			&p_key,
			&p_value);

		p_graph_vertex = p_value;
		freeGraphVertex(p_graph_vertex);
	}

	unordered_map_free(p_graph->p_nodes);
	p_graph->p_nodes = NULL;
}

GraphVertex* addVertex(Graph* p_graph, size_t vertex_id)
{
	GraphVertex* p_graph_vertex =
		unordered_map_get(p_graph->p_nodes, vertex_id);

	if (p_graph_vertex)
	{
		return p_graph_vertex;
	}

	p_graph_vertex = malloc(sizeof(GraphVertex));

	initGraphVertex(p_graph_vertex, vertex_id);

	unordered_map_put(
		p_graph->p_nodes,
		vertex_id,
		p_graph_vertex);

	return p_graph_vertex;
}

void removeVertex(Graph* p_graph, size_t vertex_id)
{
	GraphVertex* p_graph_vertex;
	GraphVertex* p_child_vertex;
	GraphVertex* p_parent_vertex;

	size_t child_vertex_id;
	size_t parent_vertex_id;

	unordered_map_iterator* p_child_iterator;
	unordered_map_iterator* p_parent_iterator;

	double* p_weight;

	p_graph_vertex = unordered_map_get(p_graph->p_nodes, vertex_id);

	if (!p_graph_vertex) {
		return;
	}

	p_child_iterator =
		unordered_map_iterator_alloc(p_graph_vertex->p_children);

	// Disconnect from children:
	while (unordered_map_iterator_has_next(p_child_iterator))
	{
		unordered_map_iterator_next(
			p_child_iterator,
			&child_vertex_id,
			&p_weight);

		removeEdge(p_graph, p_graph_vertex->id, child_vertex_id);
	}

	p_parent_iterator =
		unordered_map_iterator_alloc(p_graph_vertex->p_parents);

	// Disconnect from parents:
	while (unordered_map_iterator_has_next(p_parent_iterator))
	{
		unordered_map_iterator_next(
		    p_parent_iterator,
			&parent_vertex_id,
			&p_weight);

		removeEdge(p_graph, parent_vertex_id, p_graph_vertex->id);
	}

	unordered_map_remove(p_graph->p_nodes, vertex_id);

	// Free the children/parents maps:
	freeGraphVertex(p_graph_vertex);
}

int hasVertex(Graph* p_graph, size_t vertex_id)
{
	return unordered_map_contains_key(
	    p_graph->p_nodes,
		vertex_id);
}

GraphVertex* getVertex(Graph* p_graph, size_t vertex_id)
{
	void* p_value = unordered_map_get(p_graph->p_nodes, vertex_id);

	if (p_value)
	{
		return (GraphVertex*) p_value;
	}

	return NULL;
}

void addEdge(Graph* p_graph,
			 size_t tail_vertex_id,
			 size_t head_vertex_id,
			 double weight)
{
	GraphVertex* p_head_vertex;
	GraphVertex* p_tail_vertex;

	if (hasEdge(p_graph, tail_vertex_id, head_vertex_id)) {
		return;
	}

	p_tail_vertex = addVertex(p_graph, tail_vertex_id);
	p_head_vertex = addVertex(p_graph, head_vertex_id);

	double* p_weight = malloc(sizeof(*p_weight));
	*p_weight = weight;

	unordered_map_put(p_tail_vertex->p_children,
		p_head_vertex->id,
		(void*) p_weight);

	unordered_map_put(p_head_vertex->p_parents,
		p_tail_vertex->id,
		(void*) p_weight);
}

void removeEdge(Graph* p_graph,
				size_t tail_vertex_id, 
				size_t head_vertex_id)
{
	GraphVertex* p_tail_vertex;
	GraphVertex* p_head_vertex;

	p_tail_vertex = getVertex(p_graph, tail_vertex_id);
	p_head_vertex = getVertex(p_graph, head_vertex_id);

	if (!p_tail_vertex || !p_head_vertex) {
		return;
	}

	unordered_map_remove(p_head_vertex->p_parents, tail_vertex_id);
	unordered_map_remove(p_tail_vertex->p_children, head_vertex_id);
}

int hasEdge(Graph* p_graph,
			size_t tail_vertex_id,
			size_t head_vertex_id) 
{
	GraphVertex* p_graph_vertex =
		unordered_map_get(p_graph->p_nodes, tail_vertex_id);

	if (!p_graph_vertex) {
		return 0;
	}

	return unordered_map_contains_key(
		p_graph_vertex->p_children,
		head_vertex_id);
}

double getEdgeWeight(
	Graph* p_graph,
	size_t tail_vertex_id,
	size_t head_vertex_id)
{
    GraphVertex* p_graph_vertex = 
		unordered_map_get(p_graph->p_nodes, tail_vertex_id);

	void* p_value = 
		unordered_map_get(
			p_graph_vertex->p_children, 
			head_vertex_id);

	double* p_weight = (double*)(p_value);
	return *p_weight;
}