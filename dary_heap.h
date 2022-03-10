#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_DARY_HEAP_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_DARY_HEAP_H

#include <stdlib.h>

typedef struct dary_heap dary_heap;
typedef struct dary_heap_node dary_heap_node;
typedef struct dary_heap_node_map_entry dary_heap_node_map_entry;
typedef struct dary_heap_node_map dary_heap_node_map;

dary_heap* dary_heap_alloc(size_t degree,
                           size_t initial_capacity,
                           float  load_factor);
                   
int dary_heap_add (dary_heap* heap, 
                   size_t vertex_id,
                   double priority);

void   dary_heap_decrease_key (dary_heap* heap,
                               size_t vertex_id, 
                               double priority);

size_t dary_heap_extract_min  (dary_heap* heap);
size_t dary_heap_min          (dary_heap* heap);
size_t dary_heap_size         (dary_heap* heap);
void   dary_heap_clear        (dary_heap* heap);
void   dary_heap_free         (dary_heap* heap);

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_DARY_HEAP_H */