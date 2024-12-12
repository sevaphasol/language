#ifndef FRONTEND_H__
#define FRONTEND_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include "lang.h"

//———————————————————————————————————————————————————————————————————//

const char* const DefaultInput  = "exmpl.txt";
const char* const DefaultOutput = "exmpl.asm";

//-------------------------------------------------------------------//

const int MaxStrLength = 100;

//———————————————————————————————————————————————————————————————————//

struct node_allocator_t;
struct node_t;
struct identifier_t;

struct tokenizer_ctx_t
{
    node_allocator_t* node_allocator;
    FILE*             input_file;
    size_t            n_dumps;
    char*             code;
    node_t**          nodes;
    size_t            n_nodes;
    identifier_t*     identifiers_table;
    size_t            n_identifiers;
};

//———————————————————————————————————————————————————————————————————//

node_t** tokenize       (FILE*    input_file, size_t* n_nodes);
node_t*  syntax_analyze (node_t** nodes,      size_t n_nodes);
void     syntax_error   ();

//———————————————————————————————————————————————————————————————————//

#endif // FRONTEND_H__
