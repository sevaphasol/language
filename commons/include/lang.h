#ifndef LANG_H__
#define LANG_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>

//———————————————————————————————————————————————————————————————————//

#include "operators.h"

//———————————————————————————————————————————————————————————————————//

const int MaxStrLength = 100;

const char* const FrontendDefaultInput  = "exmpl_src.txt";
const char* const FrontendDefaultOutput = "exmpl_res.txt";

const char* const BackendDefaultInput  = "../../frontend/exmpl_res.txt";
const char* const BackendDefaultOutput = "exmpl_res.asm";

//———————————————————————————————————————————————————————————————————//

enum lang_status_t
{
    LANG_SUCCESS = 0,
    LANG_ERROR   = 1,
    LANG_READ_CODE_ERROR = 2,
    LANG_PARSE_ARGV_ERROR = 3,
    LANG_FILE_OPEN_ERROR = 4,
    LANG_GET_FILE_SIZE_ERROR = 5,
    LANG_STD_ALLOCATE_ERROR = 6,
    LANG_FREAD_ERROR = 7,
    LANG_FCLOSE_ERROR = 8,
    LANG_NODE_ALLOCATOR_CTOR_ERROR = 9,
    LANG_TOKENIZER_CTX_CTOR_ERROR = 10,
    LANG_SYNTAX_ERROR = 11,
    LANG_OPEN_FILES_ERROR = 12,
    LANG_GET_TOKEN_ERROR = 13,
    LANG_NODE_ALLOCATOR_DTOR_ERROR = 14,
};

//———————————————————————————————————————————————————————————————————//

struct node_t
{
    value_type_t value_type;
    value_t      value;
    size_t       line_number;
    node_t*      left;
    node_t*      right;
};

//———————————————————————————————————————————————————————————————————//

struct name_table_t
{
    size_t        n_names;
    identifier_t* table;
};

//———————————————————————————————————————————————————————————————————//

struct node_allocator_t;

//———————————————————————————————————————————————————————————————————//

struct lang_ctx_t
{
    FILE*             input_file;
    FILE*             output_file;
    size_t            input_size;

    node_allocator_t* node_allocator;

    node_t**          nodes;
    size_t            n_nodes;
    name_table_t      name_table;
    size_t            cur_line;
    char*             code;

    node_t*           tree;
    size_t            pos;
};

//———————————————————————————————————————————————————————————————————//

lang_status_t lang_ctx_ctor (lang_ctx_t* ctx,
                             int argc,
                             const char* argv[],
                             const char* default_input,
                             const char* default_output);

lang_status_t lang_ctx_dtor (lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

#endif // LANG_H__
