#ifndef COM_GITHUB_CODERODDE_PERL_ALGORITHM_H
#define COM_GITHUB_CODERODDE_PERL_ALGORITHM_H

#include "graph.h"
#include "list.h"

list* find_shortest_distance(Graph* p_graph,
                             size_t source_vertex_id,
                             size_t target_vertex_id);

#endif /* COM_GITHUB_CODERODDE_PERL_ALGORITHM_H */