#ifndef FRONTEND_H__
#define FRONTEND_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include "lang.h"

//———————————————————————————————————————————————————————————————————//

const char* const DefaultInput  = "exmpl_src.txt";
const char* const DefaultOutput = "exmpl_res.txt";

//———————————————————————————————————————————————————————————————————//

struct node_allocator_t;

struct name_table_t
{
    size_t        n_names;
    identifier_t* table;
};

struct backend_ctx_t
{
    FILE*             input_file;
    FILE*             output_file;

    node_allocator_t* node_allocator;

    node_t**          nodes;
    size_t            n_nodes;
    name_table_t      name_table;
};

//———————————————————————————————————————————————————————————————————//

#endif // FRONTEND_H__
