#include "fibonacci_heap.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const double LOG_PHI = 0.438;
static const size_t DEFAULT_NODE_ARRAY_CAPACITY = 8;

/***************************************************
Map from vertex IDs to Fibonacci heap nodes follows:
***************************************************/

typedef struct fibonacci_heap_node {
    size_t               vertex_id;
    double               priority;
    fibonacci_heap_node* parent;
    fibonacci_heap_node* left;
    fibonacci_heap_node* right;
    fibonacci_heap_node* child;
    size_t               degree;
    int                  marked;
} fibonacci_heap_node;

typedef struct heap_node_map_entry {
    size_t               vertex_id;
    fibonacci_heap_node* heap_node;
    heap_node_map_entry* chain_next;
    heap_node_map_entry* prev;
    heap_node_map_entry* next;
} heap_node_map_entry;

typedef struct heap_node_map {
    heap_node_map_entry** table;
    heap_node_map_entry*  head;
    heap_node_map_entry*  tail;
    size_t                table_capacity;
    size_t                size;
    size_t                max_allowed_size;
    size_t                mask;
    float                 load_factor;
} heap_node_map;

typedef struct fibonacci_heap {
    heap_node_map*        node_map;
    fibonacci_heap_node*  minimum_node;
    fibonacci_heap_node** node_array;
    size_t                node_array_capacity;
} fibonacci_heap;

static heap_node_map_entry*
heap_node_map_entry_alloc(size_t vertex_id,
                          fibonacci_heap_node* heap_node)
{
    heap_node_map_entry* entry = malloc(sizeof(*entry));

    if (!entry)
    {
        return NULL;
    }

    entry->vertex_id = vertex_id;
    entry->heap_node = heap_node;
    entry->chain_next = NULL;
    entry->next = NULL;
    entry->prev = NULL;

    return entry;
}

static const float  MINIMUM_LOAD_FACTOR = 0.2f;
static const size_t MINIMUM_INITIAL_CAPACITY = 16;

static float maxf(float a, float b)
{
    return a < b ? b : a;
}

static int maxi(int a, int b)
{
    return a < b ? b : a;
}

/*******************************************************************************
* Makes sure that the load factor is no less than a minimum threshold.         *
*******************************************************************************/
static float fix_load_factor(float load_factor)
{
    return maxf(load_factor, MINIMUM_LOAD_FACTOR);
}

/*******************************************************************************
* Makes sure that the initial capacity is no less than a minimum allowed and   *
* is a power of two.                                                           *
*******************************************************************************/
static size_t fix_initial_capacity(size_t initial_capacity)
{
    size_t ret;

    initial_capacity = maxi(initial_capacity, MINIMUM_INITIAL_CAPACITY);
    ret = 1;

    while (ret < initial_capacity)
    {
        ret <<= 1;
    }

    return ret;
}

heap_node_map* heap_node_map_alloc(
    size_t initial_capacity,
    float load_factor)
{
    heap_node_map* map = malloc(sizeof(*map));

    if (!map)
    {
        return NULL;
    }

    load_factor = fix_load_factor(load_factor);
    initial_capacity = fix_initial_capacity(initial_capacity);

    map->load_factor = load_factor;
    map->table_capacity = initial_capacity;
    map->size = 0;
    map->head = NULL;
    map->tail = NULL;
    map->table = calloc(initial_capacity, sizeof(heap_node_map_entry*));

    map->mask = initial_capacity - 1;
    map->max_allowed_size = (size_t)(initial_capacity * load_factor);

    return map;
}

static void ensure_capacity(heap_node_map* map)
{
    size_t new_capacity;
    size_t new_mask;
    size_t index;
    heap_node_map_entry* entry;
    heap_node_map_entry** new_table;

    if (map->size < map->max_allowed_size)
    {
        return;
    }

    new_capacity = 2 * map->table_capacity;
    new_mask = new_capacity - 1;
    new_table = calloc(new_capacity, sizeof(heap_node_map_entry*));

    if (!new_table)
    {
        return;
    }

    /* Rehash the entries. */
    for (entry = map->head; entry; entry = entry->next)
    {
        index = entry->vertex_id & new_mask;
        entry->chain_next = new_table[index];
        new_table[index] = entry;
    }

    free(map->table);

    map->table = new_table;
    map->table_capacity = new_capacity;
    map->mask = new_mask;
    map->max_allowed_size = (size_t)(new_capacity * map->load_factor);
}

bool heap_node_map_put(
    heap_node_map* map,
    size_t vertex_id,
    fibonacci_heap_node* heap_node)
{
    size_t index;
    size_t hash_value;
    void* old_value;
    heap_node_map_entry* entry;

    if (!map)
    {
        return false;
    }

    hash_value = vertex_id;
    index = hash_value & map->mask;

    for (entry = map->table[index]; entry; entry = entry->chain_next)
    {
        if (entry->vertex_id == vertex_id)
        {
            entry->heap_node = heap_node;
            return true;
        }
    }
    // todo : add mem check
    ensure_capacity(map);

    /* Recompute the index since it is possibly changed by 'ensure_capacity' */
    index = hash_value & map->mask;
    entry = heap_node_map_entry_alloc(vertex_id, heap_node);

    if (!entry) {
        return false;
    }

    entry->chain_next = map->table[index];
    map->table[index] = entry;

    /* Link the new entry to the tail of the list. */
    if (!map->tail)
    {
        map->head = entry;
        map->tail = entry;
    }
    else
    {
        map->tail->next = entry;
        entry->prev = map->tail;
        map->tail = entry;
    }

    map->size++;
    return true;
}

size_t heap_node_map_size(heap_node_map* map)
{
    if (!map) {
        abort();
        return;
    }

    return map->size;
}

bool heap_node_map_contains_key(heap_node_map* map, size_t vertex_id) 
{
    size_t index;
    heap_node_map_entry* entry;

    if (!map)
    {
        return false;
    }

    index = vertex_id & map->mask;

    for (entry = map->table[index]; entry; entry = entry->chain_next)
    {
        if (vertex_id == entry->vertex_id)
        {
            return 1;
        }
    }

    return 0;
}

fibonacci_heap_node* heap_node_map_get(heap_node_map* map, size_t vertex_id)
{
    size_t index;
    heap_node_map_entry* p_entry;

    if (!map)
    {
        abort();
    }

    index = vertex_id & map->mask;

    for (p_entry = map->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (vertex_id == p_entry->vertex_id)
        {
            return p_entry->heap_node;
        }
    }

    return NULL;

}

int heap_node_map_remove(heap_node_map* map, size_t vertex_id)
{
    size_t index;
    heap_node_map_entry* prev_entry;
    heap_node_map_entry* current_entry;

    if (!map)
    {
        return RETURN_STATUS_NO_HEAP;
    }

    index = vertex_id & map->mask;

    prev_entry = NULL;

    for (current_entry = map->table[index];
        current_entry;
        current_entry = current_entry->chain_next)
    {
        if (vertex_id == current_entry->vertex_id)
        {
            if (prev_entry)
            {
                /* Omit the 'p_current_entry' in the collision chain. */
                prev_entry->chain_next = current_entry->chain_next;
            }
            else
            {
                map->table[index] = current_entry->chain_next;
            }

            /* Unlink from the global iteration chain. */
            if (current_entry->prev)
            {
                current_entry->prev->next = current_entry->next;
            }
            else
            {
                map->head = current_entry->next;
            }

            if (current_entry->next)
            {
                current_entry->next->prev = current_entry->prev;
            }
            else
            {
                map->tail = current_entry->prev;
            }

            map->size--;
            free(current_entry);
            return RETURN_STATUS_OK;
        }

        prev_entry = current_entry;
    }

    return RETURN_STATUS_NOTHING_TO_REMOVE;
}


static void heap_node_map_clear(heap_node_map* map)
{
    heap_node_map_entry* entry;
    heap_node_map_entry* next_entry;
    size_t index;

    if (!map)
    {
        return;
    }

    entry = map->head;

    while (entry)
    {
        index = entry->vertex_id & map->mask;
        next_entry = entry->next;
        free(entry);
        entry = next_entry;
        map->table[index] = NULL;
    }

    map->size = 0;
    map->head = NULL;
    map->tail = NULL;
}

void heap_node_map_free(heap_node_map* map)
{
    if (!map)
    {
        return;
    }

    heap_node_map_clear(map);
    free(map->table);
    free(map);
}


/*************************************
Fibonacci heap implementation follows:
*************************************/

static fibonacci_heap_node* 
fibonacci_heap_node_alloc(size_t vertex_id, 
                          double priority) {

    fibonacci_heap_node* node = malloc(sizeof(*node));
    
    if (!node)
    {
        return NULL;
    }

    node->vertex_id = vertex_id;
    node->priority = priority;
    node->parent = NULL;
    node->left = node;
    node->right = node;
    node->child = NULL;
    node->degree = 0U;
    node->marked = false;

    return node;
}
static void fibonacci_heap_node_free(fibonacci_heap_node* node)
{
    fibonacci_heap_node* child;
    fibonacci_heap_node* first_child;
    fibonacci_heap_node* sibling;

    child = node->child;

    if (!child)
    {
        free(node);
        return;
    }

    first_child = child;

    while (true)
    {
        sibling = child->right;
        fibonacci_heap_node_free(child);
        child = sibling;

        if (child == first_child)
        {
            break;
        }
    }

    free(node);
}

static void fibonacci_heap_node_free_cascade(fibonacci_heap_node* node)
{
    fibonacci_heap_node* child;
    fibonacci_heap_node* first_child;
    fibonacci_heap_node* sibling;

    child = node->child;

    if (!child)
    {
        free(node);
        return;
    }

    first_child = child;

    while (true)
    {
        sibling = child->right;
        fibonacci_heap_node_free(child);
        child = sibling;

        if (child == first_child)
        {
            break;
        }
    }

    free(node);
}

fibonacci_heap*
fibonacci_heap_alloc(size_t map_initial_capacity,
                     float map_load_factor)
{
    fibonacci_heap* heap = malloc(sizeof(fibonacci_heap));

    if (!heap)
    {
        return NULL;
    }

    heap->node_array = 
        malloc(sizeof(fibonacci_heap_node*) 
               * DEFAULT_NODE_ARRAY_CAPACITY);

    if (!heap->node_array)
    {
        free(heap);
        return NULL;
    }

    heap->node_array_capacity = DEFAULT_NODE_ARRAY_CAPACITY;
    heap->node_map = heap_node_map_alloc(map_initial_capacity,
                                         map_load_factor);

    if (!heap->node_map)
    {
        free(heap->node_array);
        free(heap);
        return NULL;
    }

    heap->minimum_node = NULL;
    return heap;
}

int fibonacci_heap_add(fibonacci_heap* heap, 
                       size_t vertex_id, 
                       double priority)
{
    fibonacci_heap_node* node;

    if (!heap) {
        return RETURN_STATUS_NO_HEAP;
    }

    if (heap_node_map_contains_key(heap->node_map, vertex_id))
    {
        return RETURN_STATUS_ADDING_DUPLICATE_VERTEX;
    }

    node = fibonacci_heap_node_alloc(vertex_id, priority);

    if (!node) {
        return RETURN_STATUS_NO_MEMORY;
    }

    if (heap->minimum_node)
    {
        node->left = heap->minimum_node;
        node->right = heap->minimum_node->right;
        heap->minimum_node->right = node;
        node->right->left = node;

        if (priority < heap->minimum_node->priority) {
            heap->minimum_node = node;
        }
    }
    else
    {
        heap->minimum_node = node;
    }

    if (!heap_node_map_put(heap->node_map, vertex_id, node)) {
        free(node);
        return RETURN_STATUS_NO_MEMORY;
    }

    return RETURN_STATUS_OK;
}

static void cut(fibonacci_heap* heap,
                fibonacci_heap_node* x, 
                fibonacci_heap_node* y)
{
    x->left->right = x->right;
    x->right->left = x->left;
    y->degree--;

    if (y->child == x)
    {
        y->child = x->right;
    }

    if (y->degree == 0)
    {
        y->child = NULL;
    }

    x->left = heap->minimum_node;
    x->right = heap->minimum_node->right;
    heap->minimum_node->right = x;
    x->right->left = x;

    x->parent = NULL;
    x->marked = false;
}

static void cascading_cut(fibonacci_heap* heap, 
                          fibonacci_heap_node* y)
{
    fibonacci_heap_node* z = y->parent;

    if (z)
    {
        if (y->marked)
        {
            cut(heap, y, z);
            cascading_cut(heap, z);
        }
        else
        {
            y->marked = true;
        }
    }
}

void fibonacci_heap_decrease_key(fibonacci_heap* heap,
                                 size_t vertex_id,
                                 double priority)
{
    fibonacci_heap_node* x;
    fibonacci_heap_node* y;

    if (!heap)
    {
        return false;
    }

    x = heap_node_map_get(heap->node_map, vertex_id);

    if (!x)
    {
        return false;
    }

    if (x->priority <= priority)
    {
        /* Cannot improve priority of the input element. */
        return false;
    }

    x->priority = priority;
    y = x->parent;

    if (y && x->priority < y->priority) {
        cut(heap, x, y);
        cascading_cut(heap, y);
    }

    if (x->priority < heap->minimum_node->priority) {
        heap->minimum_node = x;
    }

    return true;
}

static bool try_expand_array(fibonacci_heap* heap, size_t size)
{
    if (heap->node_array_capacity < size)
    {
        free(heap->node_array);
        heap->node_array = 
            malloc(sizeof(fibonacci_heap_node*) * size);

        if (!heap->node_array)
        {
            return false;
        }

        heap->node_array_capacity = size;
        return true;
    } else {
        return true;
    }
}

static void link(fibonacci_heap_node* y, 
                 fibonacci_heap_node* x)
{
    y->left->right = y->right;
    y->right->left = y->left;

    y->parent = x;

    if (!x->child)
    {
        x->child = y;
        y->right = y;
        y->left = y;
    }
    else
    {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right = y;
        y->right->left = y;
    }

    x->degree++;
    y->marked = false;
}

static void consolidate(fibonacci_heap* heap)
{
    size_t array_size = (size_t)(floor
    (log(heap_node_map_size(heap->node_map))
        / LOG_PHI)) + 1;

    size_t               number_of_roots;
    size_t               degree;
    size_t               i;
    fibonacci_heap_node* x;
    fibonacci_heap_node* y;
    fibonacci_heap_node* tmp;
    fibonacci_heap_node* next;

    try_expand_array(heap, array_size);

    /* Set the internal node array components to NULL. */
    memset(heap->node_array, 
           0, 
           array_size * sizeof(fibonacci_heap_node*));

    number_of_roots = 0;
    x = heap->minimum_node;

    if (x)
    {
        ++number_of_roots;
        x = x->right;

        while (x != heap->minimum_node)
        {
            ++number_of_roots;
            x = x->right;
        }
    }

    while (number_of_roots > 0)
    {
        degree = x->degree;
        next = x->right;

        while (true)
        {
            y = heap->node_array[degree];

            if (!y) break;

            if (x->priority > y->priority) {
                tmp = y;
                y = x;
                x = tmp;
            }

            link(y, x);
            heap->node_array[degree] = NULL;
            ++degree;
        }

        heap->node_array[degree] = x;
        x = next;
        --number_of_roots;
    }

    heap->minimum_node = NULL;

    for (i = 0; i < array_size; ++i)
    {
        y = heap->node_array[i];

        if (!y)
        {
            continue;
        }

        if (heap->minimum_node)
        {
            y->left->right = y->right;
            y->right->left = y->left;

            y->left = heap->minimum_node;
            y->right = heap->minimum_node->right;
            heap->minimum_node->right = y;
            y->right->left = y;

            if (heap->minimum_node->priority > y->priority) {
                heap->minimum_node = y;
            }
        }
        else
        {
            heap->minimum_node = y;
        }
    }
}

size_t fibonacci_heap_extract_min(fibonacci_heap* heap)
{
    fibonacci_heap_node* z;
    fibonacci_heap_node* x;
    fibonacci_heap_node* tmp_right;
    fibonacci_heap_node* node_to_free;

    size_t return_vertex_id;
    size_t number_of_children;

    if (!heap)
    {
        return NULL;
    }

    z = heap->minimum_node;

    if (!z)
    {
        return NULL; /* Heap is empty. */
    }

    number_of_children = z->degree;
    x = z->child;

    while (number_of_children > 0)
    {
        tmp_right = x->right;

        x->left->right = x->right;
        x->right->left = x->left;

        x->left = heap->minimum_node;
        x->right = heap->minimum_node->right;
        heap->minimum_node->right = x;
        x->right->left = x;

        x->parent = NULL;
        x = tmp_right;
        --number_of_children;
    }

    z->left->right = z->right;
    z->right->left = z->left;

    return_vertex_id = heap->minimum_node->vertex_id;

    if (z == z->right)
    {
        node_to_free = heap->minimum_node;
        heap->minimum_node = NULL;
    }
    else
    {
        node_to_free = heap->minimum_node;
        heap->minimum_node = z->right;
        consolidate(heap);
    }

    heap_node_map_remove(heap->node_map, return_vertex_id);
    free(node_to_free);
    return return_vertex_id;
}

bool fibonacci_heap_contains_key(fibonacci_heap* heap, void* element)
{
    if (!heap)
    {
        return false;
    }

    return heap_node_map_contains_key(heap->node_map, element);
}

size_t fibonacci_heap_min(fibonacci_heap* heap)
{
    if (!heap)
    {
        abort();
    }

    if (heap->minimum_node)
    {
        return heap->minimum_node->vertex_id;
    }

    return NULL;
}

size_t fibonacci_heap_size(fibonacci_heap* heap)
{
    if (!heap)
    {
        return 0;
    }

    return heap_node_map_size(heap->node_map);
}

void fibonacci_heap_clear(fibonacci_heap* heap)
{
    fibonacci_heap_node* current;
    fibonacci_heap_node* sibling;
    fibonacci_heap_node* first_root;

    if (!heap)
    {
        return;
    }

    if (!heap->minimum_node)
    {
        return;
    }

    current = heap->minimum_node;
    first_root = current;

    while (true)
    {
        sibling = current->right;
        fibonacci_heap_node_free(current);
        current = sibling;

        if (current == first_root)
        {
            break;
        }
    }

    heap->minimum_node = NULL;
    heap_node_map_clear(heap->node_map);
}

static bool tree_is_healthy(fibonacci_heap* heap, fibonacci_heap_node* node)
{
    fibonacci_heap_node* begin;

    if (!node)
    {
        return true;
    }

    begin = node;

    while (true)
    {
        if (node->priority < node->parent->priority) {
            return false;
        }

        if (!tree_is_healthy(heap, node))
        {
            return false;
        }

        begin = begin->right;

        if (begin == node)
        {
            return false;
        }
    }

    return true;
}

static bool check_root_list(fibonacci_heap* heap)
{
    fibonacci_heap_node* current = heap->minimum_node;

    while (true)
    {
        if (current->priority < heap->minimum_node->priority) {
            return false;
        }

        current = current->right;

        if (current == heap->minimum_node)
        {
            return true;
        }
    }
}

bool fibonacci_heap_is_healthy(fibonacci_heap* heap)
{
    fibonacci_heap_node* root;

    if (!heap)
    {
        return false;
    }

    if (!heap->minimum_node)
    {
        return true;
    }

    /* Check that in the root list, 'minimum_node' points to the node
       with largest priority.
     */
    if (!check_root_list(heap))
    {
        return false;
    }

    root = heap->minimum_node;

    /* Check that all trees are min-heap ordered: the priority of any child is
     * not higher than the priority of its parent. */
    while (root)
    {
        if (!tree_is_healthy(heap, root->child))
        {
            return false;
        }

        root = root->right;

        if (root == heap->minimum_node)
        {
            return true;
        }
    }

    return false;
}

void fibonacci_heap_free(fibonacci_heap* heap)
{
    if (!heap)
    {
        return;
    }

    if (heap->node_array)
    {
        free(heap->node_array);
    }

    fibonacci_heap_clear(heap);

    if (heap->node_map)
    {
        heap_node_map_free(heap->node_map);
    }

    free(heap);
}