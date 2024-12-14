#include "frontend.h"
#include "custom_assert.h"
#include "operators.h"
#include "dsl.h"
#include "node_allocator.h"

//———————————————————————————————————————————————————————————————————//

#define _RED(str)       "\033[31m" str "\033[0m"
#define _GREEN(str)     "\033[32m" str "\033[0m"
#define _YELLOW(str)    "\033[33m" str "\033[0m"
#define _BLUE(str)      "\033[34m" str "\033[0m"
#define _PURPLE(str)    "\033[35m" str "\033[0m"
#define _TURQUOISE(str) "\033[36m" str "\033[0m"

#define _CURRENT_NODE ctx->nodes[ctx->pos]
#define _ID(node) ctx->name_table.table[node->value.id_index]

//===================================================================//

#define _SYNTAX_ERROR(__condition__, ...)                              \
                                                                       \
    if (__condition__)                                                 \
    {                                                                  \
        fprintf(stderr, _GREEN("\nDEBUG in %s:%d:%s\n\n"),             \
                        __FILE__, __LINE__, __PRETTY_FUNCTION__);      \
                                                                       \
        fprintf(stderr, ##__VA_ARGS__);                                \
                                                                       \
        return nullptr;                                                \
    }                                                                  \

//===================================================================//

#define _CHECK_TYPE(__type__)                                          \
                                                                       \
    _SYNTAX_ERROR(_CURRENT_NODE->value_type != __type__,               \
                  _RED("Syntax error.")                                \
                  " Line " _YELLOW("%ld") ". "                         \
                  "Expected type " _PURPLE("%s") ". "                  \
                  "Got type " _PURPLE("%s") " instead.\n",             \
                  _CURRENT_NODE->line_number,                          \
                  type_name(__type__),                                 \
                  type_name(_CURRENT_NODE->value_type));               \

//=========================================================================//

#define _CHECK_OPERATOR(__code__)                                           \
                                                                            \
    _CHECK_TYPE(OPERATOR);                                                  \
                                                                            \
    _SYNTAX_ERROR(_CURRENT_NODE->value.operator_code != __code__,           \
                  _RED("Syntax error.")                                     \
                  " Line " _YELLOW("%ld") ". "                              \
                  "Expected " _PURPLE("\"%s\"") ". "                        \
                  "Got " _PURPLE("\"%s\"") " instead.\n",                   \
                  ctx->nodes[ctx->pos]->line_number,                        \
                  OperatorsTable[__code__].name,                            \
                  OperatorsTable[_CURRENT_NODE->value.operator_code].name); \

//=========================================================================//

//———————————————————————————————————————————————————————————————————//

static node_t*     get_global_statement (frontend_ctx_t* ctx);
static node_t*     get_declaration      (frontend_ctx_t* ctx);
static node_t*     get_func_declaration (frontend_ctx_t* ctx);
static node_t*     get_func_params      (frontend_ctx_t* ctx);
static node_t*     get_body             (frontend_ctx_t* ctx);
static node_t*     get_var_declaration  (frontend_ctx_t* ctx);
static void        connect_nodes        (node_t* parent, node_t* left, node_t* right);
lang_status_t      syntax_error         (bool condition, const char* str, ...);
static const char* type_name            (value_type_t type);

//———————————————————————————————————————————————————————————————————//

static const char* type_name(value_type_t type)
{
    switch (type)
    {
        case OPERATOR:   return "OPERATOR";
        case NUMBER:     return "NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        default:         return "GOUDA";
    }
}

//===================================================================//

static void connect_nodes (node_t* parent, node_t* left, node_t* right)
{
    ASSERT(parent);

    //-------------------------------------------------------------------//

    if (left)  parent->left  = left;
    if (right) parent->right = right;
}

//===================================================================//

node_t* syntax_analyze(frontend_ctx_t* ctx)
{
    ASSERT(ctx)

    //-------------------------------------------------------------------//

    return get_global_statement(ctx);
}

//===================================================================//

node_t* get_global_statement(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    node_t* global_statement = ctx->nodes[ctx->pos];

    _CHECK_OPERATOR(STATEMENT);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* global_statement_body = get_declaration(ctx);
    node_t* next_global_statement = nullptr;

    if (ctx->pos < ctx->n_nodes - 1                    &&
        _CURRENT_NODE->value_type          == OPERATOR &&
        _CURRENT_NODE->value.operator_code == STATEMENT)
    {
        next_global_statement = get_global_statement(ctx);
    }

    connect_nodes(global_statement,
                  global_statement_body,
                  next_global_statement);

    //-------------------------------------------------------------------//

    return global_statement;
}

//===================================================================//

node_t* get_declaration(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* declaration = ctx->nodes[ctx->pos];

    //-------------------------------------------------------------------//

    if (declaration->value.operator_code == NEW_FUNC)
    {
        declaration = get_func_declaration(ctx);
    }
    else if (declaration->value.operator_code == NEW_VAR)
    {
        declaration = get_var_declaration(ctx);
    }
    else
    {
        _SYNTAX_ERROR(true,
                      _RED("Syntax error.")
                      " Line " _YELLOW("%ld") ". "
                      "Here should be declaration of func or variable\n",
                      declaration->line_number);
    }

    //-------------------------------------------------------------------//

    return declaration;
}

//===================================================================//

node_t* get_func_declaration(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    node_t* func_declaration = _CURRENT_NODE;

    _CHECK_OPERATOR(NEW_FUNC);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* func_name = _CURRENT_NODE;

    _CHECK_TYPE(IDENTIFIER);

    _ID(func_name).is_inited = true;
    _ID(func_name).type      = FUNC;

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* func_params = get_func_params(ctx);

    //-------------------------------------------------------------------//

    node_t* func_body = get_body(ctx);

    //-------------------------------------------------------------------//

    connect_nodes(func_declaration, func_name,   nullptr);
    connect_nodes(func_name,        func_params, func_body);

    //-------------------------------------------------------------------//

    return func_declaration;
}

//===================================================================//

node_t* get_func_params(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    //-------------------------------------------------------------------//

    node_t* param     = _OPERATOR(PARAM_LINKER);
    node_t* cur_param = param;

    ctx->pos++;

    while (_CURRENT_NODE->value.operator_code != CLOSE_BRACKET)
    {
        _CHECK_TYPE(OPERATOR);
        _CHECK_OPERATOR(NEW_VAR);

        node_t* cur_declaration = _CURRENT_NODE;

        connect_nodes(cur_param, cur_declaration, nullptr);

        ctx->pos++;

        //-------------------------------------------------------------------//

        node_t* cur_var = _CURRENT_NODE;

        _CHECK_TYPE(IDENTIFIER);

        _ID(cur_var).is_inited = true;
        _ID(cur_var).type      = VAR;

        connect_nodes(cur_declaration, cur_var, nullptr);

        ctx->pos++;

        //-------------------------------------------------------------------//

        if (_CURRENT_NODE->value.operator_code == PARAM_LINKER)
        {
            node_t* next_param = _CURRENT_NODE;

            connect_nodes(cur_param, nullptr, next_param);

            cur_param = next_param;

            ctx->pos++;

            _CHECK_TYPE(OPERATOR);
            _SYNTAX_ERROR(_CURRENT_NODE->value.operator_code == CLOSE_BRACKET,
                          _RED("Syntax Error.")
                          "Line " _YELLOW("%ld") ". "
                          "You can't place " _PURPLE(",")
                          " before " _PURPLE(")") ".",
                          _CURRENT_NODE->line_number);
        }
    }

    //-------------------------------------------------------------------//

    ctx->pos++;

    //-------------------------------------------------------------------//

    return param;
}

//===================================================================//

node_t* get_body(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_START);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* body = ctx->nodes[ctx->pos];

    // node_t* statement = get_statement(ctx);

    while(ctx->nodes[ctx->pos]->value.operator_code != BODY_END)
    {
        ctx->pos++;
    }

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_END);

    ctx->pos++;

    //-------------------------------------------------------------------//

    // connect_nodes(body, statement, nullptr);

    connect_nodes(body, nullptr, nullptr);

    return body;
}

//===================================================================//

node_t* get_var_declaration(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

//     SYNTAX_ERROR(nodes[*pos]->value_type          != OPERATOR)
//     SYNTAX_ERROR(nodes[*pos]->value.operator_code != NEW_VAR)
//
//     pos++;
//
//     VERIFY(get_identifier(nodes, pos),
//            return LANG_GET_FUNC_DECLARATION_ERROR);
//
//     //-------------------------------------------------------------------//
//
//     SYNTAX_ERROR(nodes[*pos]->value_type          != OPERATOR)
//     SYNTAX_ERROR(nodes[*pos]->value.operator_code != ASSIGNMENT)
//
//     pos++;
//
//     VERIFY(get_expression(nodes, pos),
//            return LANG_GET_FUNC_DECLARATION_ERROR);

    //-------------------------------------------------------------------//

    return nullptr;
}

//===================================================================//
//
// lang_status_t get_expression(node_t** nodes, size_t* pos)
// {
//     ASSERT(nodes);
//     ASSERT(pos);
//
//     //-------------------------------------------------------------------//
//
//     SYNTAX_ERROR(nodes[*pos]->value_type          != OPERATOR)
//     SYNTAX_ERROR(nodes[*pos]->value.operator_code != NEW_VAR)
//
//     pos++;
//
//     VERIFY(get_identifier(nodes, pos),
//            return LANG_GET_FUNC_DECLARATION_ERROR);
//
//     //-------------------------------------------------------------------//
//
//     SYNTAX_ERROR(nodes[*pos]->value_type          != OPERATOR)
//     SYNTAX_ERROR(nodes[*pos]->value.operator_code != ASSIGNMENT)
//
//     pos++;
//
//     VERIFY(get_expression(nodes, pos),
//            return LANG_GET_FUNC_DECLARATION_ERROR);
//
//     //-------------------------------------------------------------------//
//
//     return LANG_SUCCESS;
// }
