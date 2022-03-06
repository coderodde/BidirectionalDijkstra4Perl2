#ifndef COM_GITHUB_CODERODDE_BIDIR_SEARCH_VERTEX_SET_H
#define	COM_GITHUB_CODERODDE_BIDIR_SEARCH_VERTEX_SET_H

#include "util.h"
#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct vertex_set_entry vertex_set_entry;
    typedef struct vertex_set vertex_set;

    /***************************************************************************
    * Allocates a new, empty set with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    vertex_set* vertex_set_alloc
       (size_t initial_capacity,
        float load_factor);

    /***************************************************************************
    * Adds 'p_element' to the set if not already there. Returns true if the    *
    * structure of the set changed.                                            *
    ***************************************************************************/
    bool vertex_set_add(vertex_set* p_set, size_t vertex_id);

    /***************************************************************************
    * Returns true if the set contains the element.                            *
    ***************************************************************************/
    bool vertex_set_contains(vertex_set* p_set, size_t vertex_id);

    /***************************************************************************
    * Removes all the contents of the set.                                     *
    ***************************************************************************/
    static void vertex_set_clear(vertex_set* p_set);

    /***************************************************************************
    * Deallocates the entire set. Only the set and its nodes are deallocated.  *
    * The user is responsible for deallocating the actual data stored in the   *
    * set.                                                                     *
    ***************************************************************************/
    void vertex_set_free(vertex_set* p_set);

#ifdef	__cplusplus
}
#endif

#endif	/* COM_GITHUB_CODERODDE_BIDIR_SEARCH_VERTEX_SET_H */
