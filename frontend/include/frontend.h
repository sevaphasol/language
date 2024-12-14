#ifndef FRONTEND_H__
#define FRONTEND_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include "lang.h"

//———————————————————————————————————————————————————————————————————//

const char* const DefaultInput  = "exmpl.txt";
const char* const DefaultOutput = "exmpl.asm";

//-------------------------------------------------------------------//
//———————————————————————————————————————————————————————————————————//

struct node_allocator_t;

struct name_table_t
{
    size_t        n_names;
    identifier_t* table;
};

struct frontend_ctx_t
{
    FILE*             input_file;
    FILE*             output_file;

    node_allocator_t* node_allocator;

    node_t**          nodes;
    size_t            n_nodes;
    name_table_t      name_table;
    size_t            cur_line;
    char*             code;

    size_t            pos;
};

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize  (frontend_ctx_t* ctx);
node_t*  syntax_analyze (frontend_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

#endif // FRONTEND_H__
