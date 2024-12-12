#include "frontend.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

#define SYNTAX_ERROR(condition) \
                                \
if (cond)                       \
{                               \
    syntax_error();             \
                                \
    return LANG_SYNTAX_ERROR;   \
}                               \

//———————————————————————————————————————————————————————————————————//

lang_status_t get_global_statement(node_t** nodes, size_t* pos);
lang_status_t get_func_declaration(node_t** nodes, size_t* pos);
lang_status_t get_var_declaration(node_t** nodes, size_t* pos);

//———————————————————————————————————————————————————————————————————//

lang_status_t syntax_analyze(node_t** root,
                             node_t** nodes,
                             size_t n_nodes)
{
    ASSERT(nodes)

    //-------------------------------------------------------------------//

    size_t  pos = 0;

    node_t* cur_statement = *root;

    while (pos < n_nodes)
    {
        SYNTAX_ERROR(cur_statement->value_type != OPERATOR)
        SYNTAX_ERROR(cur_statement->value_type != STATEMENT)

        pos++;

        VERIFY(get_global_statement(nodes, &pos, cur_statement->left),
               return LANG_GET_GLOBAL_STATEMENT_ERROR);

        cur_statement->right = nodes[pos];
        cur_statement        = cur_statement->right;
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_global_statement(node_t** nodes, size_t* pos, node_t** res_node)
{
    ASSERT(nodes);
    ASSERT(pos);
    ASSERT(nodes[*pos]);

    //-------------------------------------------------------------------//

    node_t* cur_node = nodes[*pos];

    SYNTAX_ERROR(cur_node->value_type != OPERATOR)

    //-------------------------------------------------------------------//

    if (cur_node->value == NEW_VAR)
    {
        VERIFY(get_var_declaration(nodes, pos, &cur_node->left),
               return LANG_GET_VAR_DECLARATION_ERROR);
    }
    else if (cur_node->value == NEW_FUNC)
    {
        VERIFY(get_func_declaration(nodes, pos, &cur_node->left),
               return LANG_GET_FUNC_DECLARATION_ERROR);
    }

    //-------------------------------------------------------------------//

    *res_node = cur_node;

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_func_declaration(node_t** nodes, size_t* pos, node_t** res_node)
{
    ASSERT(nodes);
    ASSERT(pos);

    //-------------------------------------------------------------------//

    value_t val = nodes[*pos]->value;

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos] != '(')

    (*pos)++;

    for (int i = 0; i < NameTable[val.operator_code].n_vars; i++)
    {
        VERIFY(get_var_declaration(nodes, pos),
               return LANG_GET_FUNC_DECLARATION_ERROR);

        (*pos)++;
    }

    SYNTAX_ERROR(nodes[*pos] != ')')

    (*pos)++;

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos] != '{')

    (*pos)++;

    VERIFY(get_statement(nodes, pos),
           return LANG_GET_FUNC_DECLARATION_ERROR);

    SYNTAX_ERROR(nodes[*pos] != '}')

    (*pos)++;

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_var_declaration(node_t** nodes, size_t* pos)
{
    ASSERT(nodes);
    ASSERT(pos);

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos]->value_t != OPERATOR)
    SYNTAX_ERROR(nodes[*pos]->value   != NEW_VAR)

    (*pos)++;

    VERIFY(get_identifier(nodes, pos),
           return LANG_GET_FUNC_DECLARATION_ERROR);

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos]->value_t != OPERATOR)
    SYNTAX_ERROR(nodes[*pos]->value   != ASSIGNMENT)

    (*pos)++;

    VERIFY(get_expression(nodes, pos),
           return LANG_GET_FUNC_DECLARATION_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_expression(node_t** nodes, size_t* pos)
{
    ASSERT(nodes);
    ASSERT(pos);

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos]->value_t != OPERATOR)
    SYNTAX_ERROR(nodes[*pos]->value   != NEW_VAR)

    (*pos)++;

    VERIFY(get_identifier(nodes, pos),
           return LANG_GET_FUNC_DECLARATION_ERROR);

    //-------------------------------------------------------------------//

    SYNTAX_ERROR(nodes[*pos]->value_t != OPERATOR)
    SYNTAX_ERROR(nodes[*pos]->value   != ASSIGNMENT)

    (*pos)++;

    VERIFY(get_expression(nodes, pos),
           return LANG_GET_FUNC_DECLARATION_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}
