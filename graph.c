#include "graph.h"
#include "graph_vertex_map.h"
#include "weight_map.h"

static const size_t initial_capacity = 1024;
static const float load_factor = 1.3f;

void initGraphVertex(GraphVertex* p_graph_vertex, size_t id)
{
	p_graph_vertex->p_children =
		weight_map_alloc(initial_capacity,
						 load_factor);

	p_graph_vertex->p_parents =
		weight_map_alloc(initial_capacity,
						 load_factor);

	p_graph_vertex->id = id;
}

void freeGraphVertex(GraphVertex* p_graph_vertex)
{
	weight_map_free(p_graph_vertex->p_children);
	weight_map_free(p_graph_vertex->p_parents);
}

void initGraph(Graph* p_graph)
{
	p_graph->p_nodes =
		graph_vertex_alloc(initial_capacity, 
						   load_factor);
}

void freeGraph(Graph* p_graph)
{
	graph_vertex_map_iterator* p_iterator =
		graph_vertex_map_iterator_alloc(p_graph->p_nodes);

	GraphVertex* p_graph_vertex;

	while (graph_vertex_map_iterator_has_next(p_iterator))
	{
		size_t* p_vertex_id;
		GraphVertex* p_graph_vertex;

		graph_vertex_map_iterator_next(
			p_iterator,
			&p_vertex_id,
			&p_graph_vertex);

		freeGraphVertex(p_graph_vertex);
	}

	graph_vertex_map_free(p_graph->p_nodes);
	p_graph->p_nodes = NULL;
}

GraphVertex* addVertex(Graph* p_graph, size_t vertex_id)
{
	GraphVertex* p_graph_vertex =
		graph_vertex_map_get(p_graph->p_nodes, vertex_id);

	if (p_graph_vertex)
	{
		return p_graph_vertex;
	}

	p_graph_vertex = malloc(sizeof(GraphVertex));

	initGraphVertex(p_graph_vertex, vertex_id);

	graph_vertex_map_put(
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

	weight_map_iterator* p_child_iterator;
	weight_map_iterator* p_parent_iterator;

	double* p_weight;

	p_graph_vertex = graph_vertex_map_get(p_graph->p_nodes, vertex_id);

	if (!p_graph_vertex) {
		return;
	}

	p_child_iterator =
		weight_map_iterator_alloc(p_graph_vertex->p_children);

	// Disconnect from children:
	while (weight_map_iterator_has_next(p_child_iterator))
	{
		weight_map_iterator_next(
			p_child_iterator,
			&child_vertex_id,
			&p_weight);

		removeEdge(p_graph, p_graph_vertex->id, child_vertex_id);
	}

	p_parent_iterator =
		weight_map_iterator_alloc(p_graph_vertex->p_parents);

	// Disconnect from parents:
	while (weight_map_iterator_has_next(p_parent_iterator))
	{
		weight_map_iterator_next(
		    p_parent_iterator,
			&parent_vertex_id,
			&p_weight);

		removeEdge(p_graph, parent_vertex_id, p_graph_vertex->id);
	}

	graph_vertex_map_remove(p_graph->p_nodes, vertex_id);

	// Free the children/parents maps:
	freeGraphVertex(p_graph_vertex);
}

int hasVertex(Graph* p_graph, size_t vertex_id)
{
	return graph_vertex_map_contains_key(
	    p_graph->p_nodes,
		vertex_id);
}

GraphVertex* getVertex(Graph* p_graph, size_t vertex_id)
{
	GraphVertex* p_graph_vertex =
		graph_vertex_map_get(p_graph->p_nodes, 
						     vertex_id);

	return p_graph_vertex;
}

void addEdge(Graph* p_graph,
			 size_t tail_vertex_id,
			 size_t head_vertex_id,
			 double weight)
{
	GraphVertex* p_head_vertex;
	GraphVertex* p_tail_vertex;
	GraphVertex* p_temp_vertex;

	if (hasEdge(p_graph, tail_vertex_id, head_vertex_id)) {
		/* Update weight: */
		p_temp_vertex = 
			graph_vertex_map_get(p_graph->p_nodes, 
								 tail_vertex_id);

		weight_map_put(p_temp_vertex->p_children, 
					   head_vertex_id, 
					   weight);
		return;
	}

	p_tail_vertex = addVertex(p_graph, tail_vertex_id);
	p_head_vertex = addVertex(p_graph, head_vertex_id);

	weigh_map_put(p_tail_vertex->p_children,
				  p_head_vertex->id,
				  weight);

	weight_map_put(p_head_vertex->p_parents,
				   p_tail_vertex->id,
				   weight);
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

	weight_map_remove(p_head_vertex->p_parents,  tail_vertex_id);
	weight_map_remove(p_tail_vertex->p_children, head_vertex_id);
}

int hasEdge(Graph* p_graph,
			size_t tail_vertex_id,
			size_t head_vertex_id) 
{
	GraphVertex* p_graph_vertex =
		graph_vertex_map_get(p_graph->p_nodes, tail_vertex_id);

	if (!p_graph_vertex) {
		return 0;
	}

	return weight_map_contains_key(p_graph_vertex->p_children,
								   head_vertex_id);
}

double getEdgeWeight(
	Graph* p_graph,
	size_t tail_vertex_id,
	size_t head_vertex_id)
{
    GraphVertex* p_graph_vertex = 
		unordered_map_get(p_graph->p_nodes, tail_vertex_id);

	if (!p_graph_vertex) {
		abort();
	}

	return weight_map_get(p_graph_vertex->p_children,
						  head_vertex_id);
}