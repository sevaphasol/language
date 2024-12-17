#include "custom_assert.h"
#include "operators.h"
#include "node_allocator.h"

//———————————————————————————————————————————————————————————————————//

#define _DSL_DEFINE_
#include "dsl.h"

//———————————————————————————————————————————————————————————————————//

lang_status_t syntax_analysis (lang_ctx_t* ctx);

//-------------------------------------------------------------------//

static lang_status_t get_global_statement     (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_declaration          (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_func_declaration     (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_func_params          (lang_ctx_t* ctx, node_t** ret_node, int* n_params);
static lang_status_t get_func_use_params      (lang_ctx_t* ctx, node_t** ret_node, int* n_params);
static lang_status_t get_body                 (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_var_declaration      (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_statement            (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_standart_func        (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_if                   (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_return               (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_func                 (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_call                 (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_scan                 (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_assignment           (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_expression           (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_mul_div_expression   (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_in_parent_expression (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_single_expression    (lang_ctx_t* ctx, node_t** ret_node);
static lang_status_t get_ret                  (lang_ctx_t* ctx, node_t** ret_node);

static void          connect_nodes            (node_t* parent, node_t* left, node_t* right);
static const char*   type_name                (value_type_t    type);

//———————————————————————————————————————————————————————————————————//

lang_status_t syntax_analysis(lang_ctx_t* ctx)
{
    ASSERT(ctx)
    ASSERT(ctx->nodes)

    //---------------------------------------------------------------//

    VERIFY(get_global_statement(ctx, &ctx->nodes[0]),
           return LANG_GET_GLOBAL_STATEMENT_ERROR);

    VERIFY(!ctx->nodes[0],
           return LANG_SYNTAX_ERROR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_global_statement(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;
    _CHECK_OPERATOR(STATEMENT);
    _NEXT_POS;

    //---------------------------------------------------------------//

    VERIFY(get_declaration(ctx, &(*ret_node)->left),
           return LANG_GET_DECLARATION_ERROR);

    //---------------------------------------------------------------//

    if (_IS_OPERATOR(STATEMENT))
    {
        VERIFY(get_global_statement(ctx, &(*ret_node)->right),
               return LANG_GET_GLOBAL_STATEMENT_ERROR);
    }

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_declaration(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;

    //---------------------------------------------------------------//

    if (_IS_OPERATOR(NEW_FUNC))
    {
        VERIFY(get_func_declaration(ctx, ret_node),
               return LANG_GET_FUNC_DECLARATION_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(NEW_VAR))
    {
        VERIFY(get_var_declaration(ctx, ret_node),
               return LANG_GET_VAR_DECLARATION);
        return LANG_SUCCESS;
    }

    //-------------------------------------------------------------------//

    _EXPECTED("declaration");
}

//===================================================================//

lang_status_t get_func_declaration(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;
    _CHECK_OPERATOR(NEW_FUNC);
    _NEXT_POS

    //---------------------------------------------------------------//

    (*ret_node)->left = _CURRENT_NODE;
    _CHECK_TYPE(IDENTIFIER);
    _CHECK_REDECLARATION((*ret_node)->left);
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    int n_params = 0;

    VERIFY(get_func_params(ctx, &(*ret_node)->left->left, &n_params),
           return LANG_GET_FUNC_PARAMS_ERROR);

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(CLOSE_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_body(ctx, &(*ret_node)->left->right),
           return LANG_GET_BODY_ERROR);

    //---------------------------------------------------------------//

    _ID((*ret_node)->left).is_inited  = true;
    _ID((*ret_node)->left).type       = FUNC;
    _ID((*ret_node)->left).n_params   = n_params;

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_func_params(lang_ctx_t* ctx, node_t** ret_node, int* n_params)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (!(_IS_OPERATOR(PARAM_LINKER))) { return LANG_SUCCESS; }

    (*n_params)++;

    //--------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;

    _NEXT_POS

    //--------------------------------------------------------------//

    _CHECK_TYPE(OPERATOR);
    _CHECK_OPERATOR(NEW_VAR);

    (*ret_node)->left = _CURRENT_NODE;

    _NEXT_POS

    //--------------------------------------------------------------//

    (*ret_node)->left->left = _CURRENT_NODE;

    _CHECK_TYPE(IDENTIFIER);
    _CHECK_REDECLARATION(_CURRENT_NODE);

    _CURRENT_ID.is_inited = true;
    _CURRENT_ID.type      = VAR;

    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_func_params(ctx, &(*ret_node)->right, n_params),
           return LANG_GET_FUNC_PARAMS_ERROR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_var_declaration(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    node_t* var_declaration = _CURRENT_NODE;
    _CHECK_OPERATOR(NEW_VAR);
    _NEXT_POS

    //---------------------------------------------------------------//

    node_t* var_name = _CURRENT_NODE;
    _CHECK_TYPE(IDENTIFIER);
    _CHECK_REDECLARATION(_CURRENT_NODE);
    _NEXT_POS

    //---------------------------------------------------------------//

    node_t* assignment = _CURRENT_NODE;
    _CHECK_OPERATOR(ASSIGNMENT);
    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_expression(ctx, &assignment->right),
           return LANG_GET_EXPRESSION_ERROR);

    //---------------------------------------------------------------//

    var_declaration->left = var_name;
    assignment->left      = var_declaration;
    *ret_node = assignment;

    _ID(var_name).is_inited = true;
    _ID(var_name).type      = VAR;

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_body(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_START);
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(STATEMENT);

    *ret_node              = _CURRENT_NODE;
    node_t* cur_statement  = _CURRENT_NODE;

    _NEXT_POS

    VERIFY(get_statement(ctx, &cur_statement->left),
           return LANG_GET_STATEMENT_ERROR);

    //---------------------------------------------------------------//

    while (_IS_OPERATOR(STATEMENT))
    {
        cur_statement->right = _CURRENT_NODE;
        cur_statement        = _CURRENT_NODE;

        _NEXT_POS

        VERIFY(get_statement(ctx, &cur_statement->left),
               return LANG_GET_STATEMENT_ERROR);
    }

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(BODY_END);
    _NEXT_POS

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_statement(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (_IS_TYPE(OPERATOR))
    {
        VERIFY(get_standart_func(ctx, ret_node),
               return LANG_GET_STANDART_FUNC_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_TYPE(IDENTIFIER))
    {
        if(!(_CURRENT_ID.type == VAR)) { _NOT_INIT_ERROR; }

        VERIFY(get_assignment(ctx, ret_node),
               return LANG_GET_ASSIGNMENT_ERROR);
        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    _EXPECTED("OPERATOR or IDENTIFIER");
}

//===================================================================//

lang_status_t get_standart_func(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    _CHECK_TYPE(OPERATOR);

    if (_IS_OPERATOR(IF) ||
        _IS_OPERATOR(WHILE))
    {
        VERIFY(get_if(ctx, ret_node),
               return LANG_GET_IF_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(RET))
    {
        VERIFY(get_return(ctx, ret_node),
               return LANG_GET_RETURN_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(COS) ||
        _IS_OPERATOR(SIN) ||
        _IS_OPERATOR(OUT) )
    {
        VERIFY(get_func(ctx, ret_node),
               return LANG_GET_FUNC_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(IN))
    {
        VERIFY(get_scan(ctx, ret_node),
               return LANG_GET_SCAN_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(NEW_VAR))
    {
        VERIFY(get_var_declaration(ctx, ret_node),
               return LANG_GET_VAR_DECLARATION_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(CALL))
    {
        VERIFY(get_call(ctx, ret_node),
               return LANG_GET_CALL_ERROR);
        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    _SYNTAX_ERROR("Expected standart func. Got "
                  _PURPLE("%s") " instead.",
                  _CURRENT_OP.name);
}

//===================================================================//

lang_status_t get_if(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;

    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);
    _NEXT_POS

    VERIFY(get_expression(ctx, &(*ret_node)->left),
           return LANG_GET_EXPRESSION_ERROR);

    _CHECK_OPERATOR(CLOSE_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_body(ctx, &(*ret_node)->right),
           return LANG_GET_BODY_ERROR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_return(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(RET);
    *ret_node = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    if (!(_IS_OPERATOR(BODY_END)))
    {
        VERIFY(get_expression(ctx, &(*ret_node)->left),
               return LANG_GET_EXPRESSION_ERROR);
    }

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_func(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;

    _NEXT_POS

    _CHECK_OPERATOR(OPEN_BRACKET);
    _NEXT_POS

    _CHECK_OPERATOR(PARAM_LINKER);
    (*ret_node)->left = _CURRENT_NODE;
    _NEXT_POS

    VERIFY(get_expression(ctx, &(*ret_node)->left),
           return LANG_GET_EXPRESSION_ERROR);

    _CHECK_OPERATOR(CLOSE_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

// ===================================================================//

lang_status_t get_func_use_params(lang_ctx_t* ctx, node_t** ret_node, int* n_params)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (!(_IS_OPERATOR(PARAM_LINKER))) return LANG_SUCCESS;

    (*n_params)++;

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_expression(ctx, &(*ret_node)->left),
           return LANG_GET_EXPRESSION_ERROR);

    VERIFY(get_func_use_params(ctx, &(*ret_node)->right, n_params),
           return LANG_GET_FUNC_USE_PARAMS_ERROR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_scan(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(PARAM_LINKER);
    (*ret_node)->left = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_TYPE(IDENTIFIER);

    if (_ID(_CURRENT_NODE).type != VAR) { _EXPECTED("var"); }

    (*ret_node)->left->left = _CURRENT_NODE;

    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(CLOSE_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_assignment(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    _CHECK_TYPE(IDENTIFIER);

    if (_ID(_CURRENT_NODE).type != VAR) { _EXPECTED("var"); }

    node_t* var = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(ASSIGNMENT);
    node_t* assignment = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    VERIFY(get_expression(ctx, &assignment->right),
           return LANG_GET_EXPRESSION_ERROR);

    //---------------------------------------------------------------//

    assignment->left = var;
    (*ret_node) = assignment;

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_ret(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(RET);
    *ret_node = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_call(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    *ret_node = _CURRENT_NODE;
    _NEXT_POS

    _CHECK_TYPE(IDENTIFIER);

    if (_CURRENT_ID.type != FUNC) { _EXPECTED("func"); }

    (*ret_node)->left = _CURRENT_NODE;
    _NEXT_POS

    //---------------------------------------------------------------//

    _CHECK_OPERATOR(OPEN_BRACKET);
    _NEXT_POS

    int n_params = 0;

    VERIFY(get_func_use_params(ctx, &(*ret_node)->left->left, &n_params),
           return LANG_GET_FUNC_PARAMS_ERROR);

    _CHECK_OPERATOR(CLOSE_BRACKET);
    _NEXT_POS

    //---------------------------------------------------------------//

    if (n_params != _ID((*ret_node)->left).n_params)
    {
        _INVALID_PARAMS_MESSAGE((*ret_node));
    }

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_expression(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    node_t* cur_expression = nullptr;

    VERIFY(get_mul_div_expression(ctx, &cur_expression),
           return LANG_GET_MUL_DIV_EXPRESSION_ERROR);

    while (_IS_OPERATOR(ADD) ||
           _IS_OPERATOR(SUB))
    {
        node_t* operation = _CURRENT_NODE;
        operation->left   = cur_expression;
        _NEXT_POS

        VERIFY(get_mul_div_expression(ctx, &operation->right),
               return LANG_GET_MUL_DIV_EXPRESSION);

        cur_expression = operation;
    }

    *ret_node = cur_expression;

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_mul_div_expression(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    node_t* cur_mul_div_expression = nullptr;

    VERIFY(get_in_parent_expression(ctx, &cur_mul_div_expression),
           return LANG_GET_IN_PARENT_EXPRESSION_ERROR);

    while (_IS_OPERATOR(MUL) ||
           _IS_OPERATOR(DIV))
    {
        node_t* operation = _CURRENT_NODE;
        operation->left   = _CURRENT_NODE;
        _NEXT_POS

        VERIFY(get_in_parent_expression(ctx, &operation->right),
               return LANG_GET_IN_PARENT_EXPRESSION_ERROR);

        cur_mul_div_expression = operation;
    }

    *ret_node = cur_mul_div_expression;

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_in_parent_expression(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (_IS_OPERATOR(OPEN_BRACKET))
    {
        _NEXT_POS

        VERIFY(get_expression(ctx, ret_node),
               return LANG_GET_EXPRESSION_ERROR);

        _CHECK_OPERATOR(CLOSE_BRACKET);
        _NEXT_POS;

        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    VERIFY(get_single_expression(ctx, ret_node),
           return LANG_GET_SINGLE_EXPRESSION_ERROR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_single_expression(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (_IS_TYPE(NUMBER))
    {
        *ret_node = _CURRENT_NODE;
        _NEXT_POS

        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    if(_IS_TYPE(IDENTIFIER))
    {
        if (!_IS_ID_TYPE(VAR))
        {
            _EXPECTED("VAR");
        }

        if (!_CURRENT_ID.is_inited)
        {
            _NOT_INIT_ERROR;
        }

        *ret_node = _CURRENT_NODE;
        _NEXT_POS

        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    if (_IS_OPERATOR(SIN) ||
        _IS_OPERATOR(COS))
    {
        VERIFY(get_func(ctx, ret_node),
               return LANG_GET_FUNC_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(CALL))
    {
        VERIFY(get_call(ctx, ret_node),
               return LANG_GET_CALL_ERROR);
        return LANG_SUCCESS;
    }

    if (_IS_OPERATOR(RET))
    {
        VERIFY(get_return(ctx, ret_node),
               return LANG_GET_RETURN_ERROR);
        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    printf("%d\n", _CURRENT_NODE->value.operator_code);

    _SYNTAX_ERROR("invalid operation");
}

//===================================================================//

const char* type_name(value_type_t type)
{
    switch (type)
    {
        case OPERATOR:   return "OPERATOR";
        case NUMBER:     return "NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        default:         return "GOUDA";
    }
}

//———————————————————————————————————————————————————————————————————//

#define _DSL_UNDEF_
#include "dsl.h"

//———————————————————————————————————————————————————————————————————//
