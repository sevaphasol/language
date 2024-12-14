#ifndef NODE_ALLOCATOR_H__
#define NODE_ALLOCATOR_H__

//———————————————————————————————————————————————————————————————————//

#include <stdint.h>
#include <stdlib.h>

#include "lang.h"

//———————————————————————————————————————————————————————————————————//

const int nAllocatedNodes = 1024*1024;

//———————————————————————————————————————————————————————————————————//

enum node_allocator_status_t
{
    NODE_ALLOCATOR_SUCCESS = 0,
    NODE_ALLOCATOR_STRUCT_NULL_PTR_ERROR,
    NODE_ALLOCATOR_STD_CALLOC_ERROR,
    NODE_ALLOCATOR_INVALID_NEW_NODE_ERROR,
    NODE_ALLOCATOR_BIG_ARRAY_REALLOC_ERROR,
    NODE_ALLOCATOR_ARRAYS_CALLOC_ERROR,
    BIG_ARRAY_REALLOC_ERROR,
};

//———————————————————————————————————————————————————————————————————//

struct node_t;

//-------------------------------------------------------------------//

struct node_allocator_t
{
    size_t   n_arrays;
    size_t   array_len;
    node_t** big_array;
    int      free_place;
};

//———————————————————————————————————————————————————————————————————//

node_allocator_status_t node_allocator_ctor(node_allocator_t* allocator,
                                            size_t array_len);

node_allocator_status_t node_allocator_dtor(node_allocator_t* allocator);

//———————————————————————————————————————————————————————————————————//

node_t* node_ctor (node_allocator_t* node_allocator,
                   value_type_t      value_type,
                   value_t           value,
                   size_t            line_number,
                   node_t*           left,
                   node_t*           right);

//———————————————————————————————————————————————————————————————————//

#endif // NODE_ALLOCATOR_H__
