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

#define _ID(node)     ctx->name_table.table[node->value.id_index]
#define _CURRENT_NODE ctx->nodes[ctx->pos]
#define _CURRENT_ID   _ID(_CURRENT_NODE)
#define _CURRENT_OP   OperatorsTable[_CURRENT_NODE->value.operator_code]

//===================================================================//

#define _SYNTAX_ERROR(...)                                            \
                                                                      \
    fprintf(stderr, _GREEN("\nDEBUG in %s:%d:%s\n\n"),                \
                    __FILE__, __LINE__, __PRETTY_FUNCTION__);         \
                                                                      \
    fprintf(stderr, _RED("syntax error")                              \
                    " in line "                                       \
                    _YELLOW("%ld") ": ",                              \
                    _CURRENT_NODE->line_number);                      \
                                                                      \
    fprintf(stderr, ##__VA_ARGS__);                                   \
                                                                      \
    fputc('\n', stderr);                                              \
                                                                      \
    return nullptr;                                                   \

//===================================================================//

#define _NOT_INIT_ERROR                                               \
    _SYNTAX_ERROR(_TURQUOISE("%s")                                    \
                             " not initialized.",                     \
                             _CURRENT_ID.name);                       \

//===================================================================//

#define _EXPECTED(str)                                                \
    _SYNTAX_ERROR("Expected " _TURQUOISE("%s") ".", str);             \

//===================================================================//

#define _CHECK_TYPE(__type__)                                         \
                                                                      \
    if (_CURRENT_NODE->value_type != __type__)                        \
    {                                                                 \
        _SYNTAX_ERROR("Expected type " _PURPLE("%s") ". "             \
                      "Got type " _PURPLE("%s") " instead.",          \
                      type_name(__type__),                            \
                      type_name(_CURRENT_NODE->value_type));          \
    }                                                                 \

//===================================================================//

#define _CHECK_OPERATOR(__code__)                                     \
                                                                      \
    if (ctx->pos >= ctx->n_nodes)                                     \
    {                                                                 \
        fprintf(stderr, "sosal?\n");                                  \
        return nullptr;                                               \
    }                                                                 \
                                                                      \
    _CHECK_TYPE(OPERATOR);                                            \
                                                                      \
    if (_CURRENT_NODE->value.operator_code != __code__)               \
    {                                                                 \
        _SYNTAX_ERROR("Expected " _PURPLE("\"%s\"") ". "              \
                      "Got " _PURPLE("\"%s\"") " instead.",           \
                      OperatorsTable[__code__].name,                  \
                      _CURRENT_OP.name);                              \
    }                                                                 \

//===================================================================//

#define _CHECK_REDECLARATION(node)                                    \
                                                                      \
    if (_ID(node).is_inited)                                          \
    {                                                                 \
        _SYNTAX_ERROR("Redeclaration of "                             \
                      _TURQUOISE("%s") ".",                           \
                      _ID(node).name)                                 \
    }                                                                 \

//===================================================================//

//———————————————————————————————————————————————————————————————————//

static node_t*     get_global_statement     (frontend_ctx_t* ctx);
static node_t*     get_declaration          (frontend_ctx_t* ctx);
static node_t*     get_func_declaration     (frontend_ctx_t* ctx);
static node_t*     get_func_params          (frontend_ctx_t* ctx);
static node_t*     get_body                 (frontend_ctx_t* ctx);
static node_t*     get_var_declaration      (frontend_ctx_t* ctx);
static node_t*     get_statement            (frontend_ctx_t* ctx);
static node_t*     get_standart_func        (frontend_ctx_t* ctx);
static node_t*     get_if                   (frontend_ctx_t* ctx);
static node_t*     get_return               (frontend_ctx_t* ctx);
static node_t*     get_func                 (frontend_ctx_t* ctx);
static node_t*     get_call                 (frontend_ctx_t* ctx);
static node_t*     get_scan                 (frontend_ctx_t* ctx);
static node_t*     get_assignment           (frontend_ctx_t* ctx);
static node_t*     get_expression           (frontend_ctx_t* ctx);
static node_t*     get_mul_div_expression   (frontend_ctx_t* ctx);
static node_t*     get_in_parent_expression (frontend_ctx_t* ctx);
static node_t*     get_single_expression    (frontend_ctx_t* ctx);

static void        connect_nodes            (node_t* parent, node_t* left, node_t* right);
lang_status_t      syntax_error             (bool condition, const char* str, ...);

//———————————————————————————————————————————————————————————————————//

void connect_nodes (node_t* parent, node_t* left, node_t* right)
{
    ASSERT(parent);

    //-------------------------------------------------------------------//

    if (left)  parent->left  = left;
    if (right) parent->right = right;
}

//===================================================================//

lang_status_t syntax_analysis(frontend_ctx_t* ctx)
{
    ASSERT(ctx)
    ASSERT(ctx->nodes)

    //-------------------------------------------------------------------//

    ctx->nodes[0] = get_global_statement(ctx);

    VERIFY(!ctx->nodes[0],
           return LANG_SYNTAX_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

node_t* get_global_statement(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

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
        _EXPECTED("declaration");
    }

    //-------------------------------------------------------------------//

    return declaration;
}

//===================================================================//

node_t* get_func_declaration(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* func_declaration = _CURRENT_NODE;

    _CHECK_OPERATOR(NEW_FUNC);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* func_name = _CURRENT_NODE;

    _CHECK_TYPE(IDENTIFIER);

    _CHECK_REDECLARATION(func_name);

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* func_params = get_func_params(ctx);

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(CLOSE_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* func_body = get_body(ctx);

    if (!func_body)
    {
        _SYNTAX_ERROR("empty func body");
    }

    //-------------------------------------------------------------------//

    connect_nodes(func_declaration, func_name,   nullptr);
    connect_nodes(func_name,        func_params, func_body);

    //-------------------------------------------------------------------//

    _ID(func_name).is_inited = true;
    _ID(func_name).type      = FUNC;

    //-------------------------------------------------------------------//

    return func_declaration;
}

//===================================================================//

node_t* get_func_params(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    if (_CURRENT_NODE->value.operator_code != PARAM_LINKER)
    {
        return nullptr;
    }

    //-------------------------------------------------------------------//

    node_t* param = _CURRENT_NODE;

    ctx->pos++;

    _CHECK_TYPE(OPERATOR);
    _CHECK_OPERATOR(NEW_VAR);

    node_t* var_declaration = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* var_name = _CURRENT_NODE;

    _CHECK_TYPE(IDENTIFIER);

    _CHECK_REDECLARATION(var_name);

    _ID(var_name).is_inited = true;
    _ID(var_name).type      = VAR;

    ctx->pos++;

    //-------------------------------------------------------------------//

    connect_nodes(var_declaration, var_name, nullptr);

    node_t* next_param = get_func_params(ctx);

    connect_nodes(param, var_declaration, next_param);

    //-------------------------------------------------------------------//

    return param;
}

//===================================================================//

node_t* get_var_declaration(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* var_declaration = _CURRENT_NODE;

    _CHECK_OPERATOR(NEW_VAR);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* var_name = _CURRENT_NODE;

    _CHECK_TYPE(IDENTIFIER);

    _CHECK_REDECLARATION(var_name);

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(ASSIGNMENT);

    node_t* assignment = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* var_value = get_expression(ctx);

    //-------------------------------------------------------------------//

    connect_nodes(var_declaration, var_name, var_value);

    _ID(var_name).is_inited = true;
    _ID(var_name).type      = VAR;

    //-------------------------------------------------------------------//

    return var_declaration;
}

//===================================================================//

node_t* get_body(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_START);

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(STATEMENT);

    node_t* body = _CURRENT_NODE;
    node_t* cur_statement  = body;

    ctx->pos++;

    node_t* statement_body = get_statement(ctx);

    connect_nodes(cur_statement, statement_body, nullptr);

    //-------------------------------------------------------------------//

    while(_CURRENT_NODE->value.operator_code == STATEMENT)
    {
        connect_nodes(cur_statement, nullptr, _CURRENT_NODE);

        cur_statement = _CURRENT_NODE;

        ctx->pos++;

        statement_body = get_statement(ctx);

        if (!statement_body)
        {
            _SYNTAX_ERROR("empty statement.");
        }

        connect_nodes(cur_statement, statement_body, nullptr);
    }

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_END);

    ctx->pos++;

    //-------------------------------------------------------------------//

    return body;
}

//===================================================================//

node_t* get_statement(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    switch (_CURRENT_NODE->value_type)
    {
        case OPERATOR:
        {
            return get_standart_func(ctx);
        }
        case IDENTIFIER:
        {
            switch (_ID(_CURRENT_NODE).type)
            {
                case VAR:
                {
                    return get_assignment(ctx);
                }

                case FUNC:
                {
                    return get_call(ctx);
                }

                default:
                {
                    _NOT_INIT_ERROR;

                    break;
                }
            }

            break;
        }

        default:
        {
            _EXPECTED("OPERATOR or IDENTIFIER");

            break;
        }
    }

    //-------------------------------------------------------------------//

    return nullptr;
}

//===================================================================//

node_t* get_standart_func(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    _CHECK_TYPE(OPERATOR);

    switch(_CURRENT_NODE->value.operator_code)
    {
        case IF:      return get_if(ctx);
        case WHILE:   return get_if(ctx);
        case RET:     return get_return(ctx);
        case COS:     return get_func(ctx);
        case SIN:     return get_func(ctx);
        case PRINT:   return get_func(ctx);
        case SCAN:    return get_scan(ctx);
        case NEW_VAR: return get_var_declaration(ctx);
        default:
        {
            _SYNTAX_ERROR("Expected standart func. Got "
                          _PURPLE("%s") " instead.",
                          _CURRENT_OP.name);
            break;
        }
    }

    return nullptr;
}

//===================================================================//

node_t* get_if(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* node_if = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    ctx->pos++;

    node_t* condition = get_expression(ctx);

    _CHECK_OPERATOR(CLOSE_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* body = get_body(ctx);

    if(!body)
    {
        _SYNTAX_ERROR("empty body");
    }

    //-------------------------------------------------------------------//

    connect_nodes(node_if, condition, body);

    return node_if;
}

//===================================================================//

node_t* get_return(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(RET);

    node_t* node_ret = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* ret_value = get_expression(ctx);

    //-------------------------------------------------------------------//

    connect_nodes(node_ret, ret_value, nullptr);

    //-------------------------------------------------------------------//

    return node_ret;
}

//===================================================================//

node_t* get_func(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* node_func = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    ctx->pos++;

    node_t* func_param = get_expression(ctx);

    while (_CURRENT_NODE->value.operator_code != CLOSE_BRACKET)
    {
        ctx->pos++;
    }

    _CHECK_OPERATOR(CLOSE_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    connect_nodes(node_func, func_param, nullptr);

    //-------------------------------------------------------------------//

    return node_func;
}

//===================================================================//

node_t* get_scan(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* node_scan = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_TYPE(IDENTIFIER);

    if (_ID(_CURRENT_NODE).type != VAR)
    {
        _EXPECTED("var");
    }

    connect_nodes(node_scan, _CURRENT_NODE, nullptr);

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(CLOSE_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    return node_scan;
}

//===================================================================//

node_t* get_assignment(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    _CHECK_TYPE(IDENTIFIER);

    if (_ID(_CURRENT_NODE).type != VAR)
    {
        _EXPECTED("var");
    }

    node_t* var = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(ASSIGNMENT);

    node_t* assignment = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    node_t* var_value = get_expression(ctx);

    //-------------------------------------------------------------------//

    connect_nodes(assignment, var, var_value);

    return assignment;
}

//===================================================================//

node_t* get_call(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* func_name = _CURRENT_NODE;

    ctx->pos++;

    //-------------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);

    ctx->pos++;

    node_t* func_args = get_expression(ctx);

    _CHECK_OPERATOR(CLOSE_BRACKET);

    ctx->pos++;

    //-------------------------------------------------------------------//

    connect_nodes(func_name, func_args, nullptr);

    //-------------------------------------------------------------------//

    return func_name;
}

//===================================================================//

node_t* get_expression(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* cur_expression = get_mul_div_expression(ctx);

    while (ctx->pos < ctx->n_nodes                        &&
           _CURRENT_NODE->value_type          == OPERATOR &&
          (_CURRENT_NODE->value.operator_code == ADD      ||
           _CURRENT_NODE->value.operator_code == SUB))
    {
        node_t* operation = _CURRENT_NODE;

        ctx->pos++;

        node_t* next_expression = get_mul_div_expression(ctx);

        connect_nodes(operation, cur_expression, next_expression);

        cur_expression = operation;
    }

    return cur_expression;
}

//===================================================================//

node_t* get_mul_div_expression(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* cur_mul_div_expression = get_in_parent_expression(ctx);

    while (ctx->pos < ctx->n_nodes                        &&
           _CURRENT_NODE->value_type          == OPERATOR &&
          (_CURRENT_NODE->value.operator_code == MUL      ||
           _CURRENT_NODE->value.operator_code == DIV))
    {
        node_t* operation = _CURRENT_NODE;

        ctx->pos++;

        node_t* next_mul_div_expression = get_in_parent_expression(ctx);

        connect_nodes(operation, cur_mul_div_expression, next_mul_div_expression);

        cur_mul_div_expression = operation;
    }

    return cur_mul_div_expression;
}

//===================================================================//

node_t* get_in_parent_expression(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    if (ctx->pos < ctx->n_nodes                        &&
        _CURRENT_NODE->value_type          == OPERATOR &&
        _CURRENT_NODE->value.operator_code == OPEN_BRACKET)
    {
        ctx->pos++;

        node_t* expression = get_expression(ctx);

        _CHECK_OPERATOR(CLOSE_BRACKET);

        ctx->pos++;

        return expression;
    }
    else
    {
        return get_single_expression(ctx);
    }
}

//===================================================================//

node_t* get_single_expression(frontend_ctx_t* ctx)
{
    ASSERT(ctx);
    VERIFY(ctx->pos >= ctx->n_nodes, return nullptr);

    //-------------------------------------------------------------------//

    node_t* single_expression = nullptr;

    //-------------------------------------------------------------------//

    switch(_CURRENT_NODE->value_type)
    {
        case OPERATOR:
        {
            if (ctx->pos < ctx->n_nodes                        &&
                _CURRENT_NODE->value.operator_code == COS ||
                _CURRENT_NODE->value.operator_code == SIN)
            {
                single_expression = get_func(ctx);

                break;
            }

            _EXPECTED("calculation func.");
        }

        case IDENTIFIER:
        {
            switch (_ID(_CURRENT_NODE).type)
            {
                case VAR:
                {
                    if (!_ID(_CURRENT_NODE).is_inited)
                    {
                        _NOT_INIT_ERROR;
                    }

                    single_expression = _CURRENT_NODE;

                    ctx->pos++;

                    break;
                }
                case FUNC:
                {
                    single_expression = get_call(ctx);

                    break;
                }
                default:
                {
                    _NOT_INIT_ERROR;

                    break;
                }
            }

            break;
        }

        case NUMBER:
        {
            single_expression = _CURRENT_NODE;

            ctx->pos++;

            break;
        }

        default:
        {
            fprintf(stderr, "GOUDA\n");

            return nullptr;
        }
    }

    return single_expression;
}

//———————————————————————————————————————————————————————————————————//


#undef _RED
#undef _GREEN
#undef _YELLOW
#undef _BLUE
#undef _PURPLE
#undef _TURQUOISE
#undef _ID
#undef _CURRENT_NODE
#undef _CURRENT_ID
#undef _CURRENT_OP
#undef _SYNTAX_ERROR
#undef _NOT_INIT_ERROR
#undef _EXPECTED
#undef _CHECK_TYPE
#undef _CHECK_OPERATOR
#undef _CHECK_REDECLARATION

//———————————————————————————————————————————————————————————————————//
