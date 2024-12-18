#include "lang.h"
#include "custom_assert.h"

#define _DSL_DEFINE_
#include "dsl.h"

//===================================================================//


//===================================================================//

lang_status_t asm_node(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    switch(cur_node->value_type)
    {
        case OPERATOR:
        {
            (*OperatorsTable[cur_node->value.operator_code].asm_func)(ctx, cur_node);
            break;
        }
        case IDENTIFIER:
        {
            VERIFY(_ID(cur_node).type != VAR,
                return LANG_ASM_NODE_ERROR);

            asm_var(ctx, cur_node);
            break;
        }
        case NUMBER:
        {
            _PRINT("push %d\n", cur_node->value.number);
            break;
        }
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_var(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    identifier_t var = ctx->name_table.ids[cur_node->value.id_index];

    if (var.is_global)
    {
        _PRINT(";pushing global var %s\n"
               "push [%ld]\n", var.name, var.addr);
        return LANG_SUCCESS;
    }

    _PRINT(";pushing local var %s\n"
           "push [BP + %ld]\n", var.name, var.addr);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_call(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    _PRINT("; save BP\n"
           "push BP\n");

    _PRINT("; pushing func_params\n");
    asm_node(ctx, cur_node->left->left);

    _PRINT(";set BP\n"
           "push BP\n"
           "push %ld\n"
           "add\n", ctx->n_locals);

    _PRINT("; poping func_params to local variables\n");
    for (size_t i = 0; i < _ID(cur_node->left).n_params; i++)
    {
        _PRINT("pop [BP + %ld]\n", i);
    }

    _PRINT("; calling function\n"
           "call %s:\n"
           "pop  BP\n"
           "push AX\n", _ID(cur_node->left).name);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_binary_operation(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);
    ASSERT(cur_node->left);
    ASSERT(cur_node->right);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->left);
    asm_node(ctx, cur_node->right);
    _PRINT("%s\n", OperatorsTable[cur_node->value.operator_code].asm_name);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_unary_operation(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->left);
    _PRINT("%s\n", OperatorsTable[cur_node->value.operator_code].asm_name);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_assignment(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->right);

    if (_ID(cur_node->left->left).is_global)
    {
        _PRINT("pop %ld\n", _ID(cur_node->left->left).addr);
        return LANG_SUCCESS;
    }

    _PRINT("pop [BP + %ld]\n", _ID(cur_node->left->left).addr);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_sequential_action(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    if (cur_node->left)  asm_node(ctx, cur_node->left);
    if (cur_node->right) asm_node(ctx, cur_node->right);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_if(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    _PRINT(";pushing if params\n");
    asm_node(ctx, cur_node->left);
    _PRINT("push 0\n"
           "je else_body_%ld\n", ctx->n_labels);

    asm_node(ctx, cur_node->right);

    _PRINT("else_body_%ld:\n",
           ctx->n_labels++);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_while(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->left);
    _PRINT("start_of_while_%ld"
           "push 0\n"
           "je else end_of_while_%ld",
           ctx->n_labels,
           ctx->n_labels);

    asm_node(ctx, cur_node->right);

    _PRINT("jmp start_of_while_%ld:\n"
           "jmp end_of_while_%ld",
           ctx->n_labels,
           ctx->n_labels);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_new_var(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    if (_ID(cur_node).is_global)
    {
        _ID(cur_node).addr = ctx->n_globals++;

        return LANG_SUCCESS;
    }

    _ID(cur_node).addr = ctx->n_locals++;

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_new_func(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    _PRINT(";declaration of function %s\n"
           "%s:\n",
           _ID(cur_node->left).name,
           _ID(cur_node->left).name);
    asm_node(ctx, cur_node->left->right);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_return(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    if (cur_node->left) asm_node(ctx, cur_node->left);
    fprintf(ctx->output_file, "pop AX\n"
                              "ret\n\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_in(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    _PRINT("in\n");

    if (_ID(cur_node->left->left).is_global)
    {
        _PRINT("pop [%ld]\n", _ID(cur_node->left->left).addr);
        return LANG_SUCCESS;
    }

    _PRINT("pop [BP + %ld]\n", _ID(cur_node->left->left).addr);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_hlt(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    _PRINT("hlt\n\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//

#define _DSL_UNDEF_
#include "dsl.h"

//———————————————————————————————————————————————————————————————————//
