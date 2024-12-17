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

const char* const BackendDefaultInput  = "exmpl_res.txt";
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
    LANG_PRINT_NODE_VALUE_ERROR = 15,
    LANG_TREE_OUTPUT_ERROR = 16,
    LANG_PUT_NODE_VALUE_ERROR = 17,
    LANG_UNKNOWN_TYPE_ERROR = 18,
    LANG_READ_LEFT_NODE_ERROR = 19,
    LANG_READ_RIGHT_NODE_ERROR = 20,
    LANG_INCORRECT_INPUT_SYNTAX_ERROR = 21,
    LANG_ASM_NODE_ERROR = 22,
    LANG_UNKNOWN_OPERATOR_ERROR = 23,
    LANG_GET_STR_NODE_ERROR = 24,
    LANG_GET_DECLARATION_ERROR = 25,
    LANG_GET_GLOBAL_STATEMENT_ERROR = 26,
    LANG_GET_FUNC_PARAMS_ERROR = 27,
    LANG_GET_BODY_ERROR = 28,
    LANG_GET_EXPRESSION_ERROR = 29,
    LANG_GET_STATEMENT_ERROR = 30,
    LANG_GET_STANDART_FUNC_ERROR = 31,
    LANG_GET_ASSIGNMENT_ERROR = 32,
    LANG_GET_IF_ERROR = 33,
    LANG_GET_RETURN_ERROR = 34,
    LANG_GET_FUNC_ERROR = 35,
    LANG_GET_SCAN_ERROR = 36,
    LANG_GET_VAR_DECLARATION_ERROR = 37,
    LANG_GET_CALL_ERROR = 38,
    LANG_GET_MUL_DIV_EXPRESSION_ERROR = 39,
    LANG_GET_MUL_DIV_EXPRESSION = 40,
    LANG_GET_SINGLE_EXPRESSION_ERROR = 41,
    LANG_NODES_OVERFLOW_ERROR = 42,
    LANG_GET_IN_PARENT_EXPRESSION_ERROR = 43,
    LANG_GET_FUNC_USE_PARAMS_ERROR = 44,
    LANG_GET_FUNC_DECLARATION_ERROR = 45,
    LANG_GET_VAR_DECLARATION = 46,
    LANG_ID_STACK_OVERFLOW_ERROR = 47,
    LANG_ID_STACK_UNDERFLOW_ERROR = 48,
    LANG_POP_LOCALES_ERROR = 49,
    LANG_REDECLARATION_ERROR = 50,
    LANG_NOT_INIT_ERROR = 51,
    LANG_PUSH_NEW_ID_COUNTER_ERROR = 52,
    LANG_ID_COUNTER_STACK_OVERFLOW_ERROR = 53,
};

//———————————————————————————————————————————————————————————————————//

enum value_type_t
{
    POISON     = 0,
    OPERATOR   = 1,
    NUMBER     = 2,
    IDENTIFIER = 3,
};

//———————————————————————————————————————————————————————————————————//

enum operator_code_t
{
    UNDEFINED     = 0,
    ADD           = 1,
    SUB           = 2,
    MUL           = 3,
    DIV           = 4,
    ASSIGNMENT    = 5,
    OPEN_BRACKET  = 6,
    CLOSE_BRACKET = 7,
    BODY_START    = 8,
    BODY_END      = 9,
    STATEMENT     = 10,
    PARAM_LINKER  = 11,
    IF            = 12,
    WHILE         = 13,
    NEW_VAR       = 14,
    NEW_FUNC      = 15,
    RET           = 16,
    COS           = 17,
    SIN           = 18,
    OUT           = 19,
    IN            = 20,
    CALL          = 21,
    HLT           = 22,
};

struct lang_ctx_t;
struct node_t;

struct operator_t
{
    operator_code_t code;
    const char*     name;
    size_t          len;
    int             n_children;
    lang_status_t   (*asm_func) (lang_ctx_t* ctx, node_t* cur_node);
};

//———————————————————————————————————————————————————————————————————//

lang_status_t asm_add(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_sub(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_mul(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_div(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_assignment(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_statement(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_param_linker(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_if(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_while(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_new_var(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_new_func(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_ret(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_cos(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_sin(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_out(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_in(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_call(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_hlt(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_identifier(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_number(lang_ctx_t* ctx, node_t* cur_node);
lang_status_t asm_node(lang_ctx_t* ctx, node_t* cur_node);

//———————————————————————————————————————————————————————————————————//

#define STR_AND_LEN(str) str, sizeof(str) / sizeof(char)

const operator_t OperatorsTable[] =
{ //  .code       .name and len           .n_childs
    {UNDEFINED    , nullptr, 0              , 0, nullptr},
    {ADD          , STR_AND_LEN("+")        , 2, &asm_add},
    {SUB          , STR_AND_LEN("-")        , 2, &asm_sub},
    {MUL          , STR_AND_LEN("*")        , 2, &asm_mul},
    {DIV          , STR_AND_LEN("/")        , 2, &asm_div},
    {ASSIGNMENT   , STR_AND_LEN("=")        , 2, &asm_assignment},
    {OPEN_BRACKET , STR_AND_LEN("(")        , 1, nullptr},
    {CLOSE_BRACKET, STR_AND_LEN(")")        , 1, nullptr},
    {BODY_START   , STR_AND_LEN("{")        , 1, nullptr},
    {BODY_END     , STR_AND_LEN("}")        , 1, nullptr},
    {STATEMENT    , STR_AND_LEN("sosal?")   , 2, &asm_statement},
    {PARAM_LINKER , STR_AND_LEN("krasivaya"), 2, &asm_param_linker},
    {IF           , STR_AND_LEN("if")       , 2, &asm_if},
    {WHILE        , STR_AND_LEN("while")    , 2, &asm_while},
    {NEW_VAR      , STR_AND_LEN("krosovka") , 2, &asm_new_var},
    {NEW_FUNC     , STR_AND_LEN("korobka")  , 2, &asm_new_func},
    {RET          , STR_AND_LEN("buyTNF")   , 0, &asm_ret},
    {COS          , STR_AND_LEN("cosipinus"), 1, &asm_cos},
    {SIN          , STR_AND_LEN("sipinus")  , 1, &asm_sin},
    {OUT          , STR_AND_LEN("print")    , 1, &asm_out},
    {IN           , STR_AND_LEN("scan")     , 1, &asm_in},
    {CALL         , STR_AND_LEN("please")   , 0, &asm_call},
    {HLT          , STR_AND_LEN("sosal!")   , 0, &asm_hlt},
};

const int nOperators = sizeof(OperatorsTable) / sizeof(operator_t);

#undef STR_AND_LEN

//———————————————————————————————————————————————————————————————————//

enum identifier_type_t
{
    UNKNOWN = 0,
    VAR     = 1,
    FUNC    = 2,
};

struct identifier_t
{
    identifier_type_t type;
    char*             name;
    size_t            len;
    int               n_params;
    bool              is_inited;
};

//———————————————————————————————————————————————————————————————————//

typedef int number_t;

//———————————————————————————————————————————————————————————————————//

union value_t
{
    operator_code_t   operator_code;
    size_t            id_index;
    number_t          number;
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

struct stack_t
{
    size_t  size;
    size_t  top;
    size_t* data;
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
    stack_t           id_stack;
    stack_t           id_counter_stack;
    size_t            cur_line;
    char*             code;

    node_t*           tree;
    size_t            pos;
    size_t            n_globals;
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
