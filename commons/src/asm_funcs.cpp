#include "lang.h"
#include "custom_assert.h"

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
            asm_identifier(ctx, cur_node);
            break;
        }
        case NUMBER:
        {
            asm_number(ctx, cur_node);
            break;
        }
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_identifier(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    identifier_t id = ctx->name_table.table[cur_node->value.id_index];

    switch (id.type)
    {
        case FUNC:
        {
            fprintf(ctx->output_file, "%s", id.name);
            break;
        }
        case VAR:
        {
            fprintf(ctx->output_file, "%s", id.name);
            break;
        }
        default:
        {
            return LANG_UNKNOWN_TYPE_ERROR;
        }
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_number(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "%d", cur_node->value.number);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_add(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);
    ASSERT(cur_node->left);
    ASSERT(cur_node->right);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);
    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->right);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "add");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_sub(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);
    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->right);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "sub");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_mul(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);
    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->right);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "mul");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_div(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->right);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "div");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_assignment(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push ");
    asm_node(ctx, cur_node->right);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "pop ");
    asm_node(ctx, cur_node->left);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_statement(lang_ctx_t* ctx, node_t* cur_node)
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

lang_status_t asm_param_linker(lang_ctx_t* ctx, node_t* cur_node)
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

    fprintf(ctx->output_file, "push 0\npush ");

    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    fprintf(ctx->output_file, "jne label:\n");

    asm_node(ctx, cur_node->right);

    fprintf(ctx->output_file, "label:\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_while(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push 0\n");

    asm_node(ctx, cur_node->left);

    fprintf(ctx->output_file, "jne label:\n");

    asm_node(ctx, cur_node->right);

    fprintf(ctx->output_file, "label:\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_new_var(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_new_func(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    asm_node(ctx, cur_node->left);
    fputs(":\n", ctx->output_file);


    for (int i = 0; i < ctx->name_table.table[cur_node->left->value.id_index].n_params; i++)
    {
        fprintf(ctx->output_file, "pop [BP + %d]\n", i);
    }

    asm_node(ctx, cur_node->left->right);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_ret(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "ret\n\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_cos(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "cos ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_sin(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "sin ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_out(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    if (cur_node->left->left->value_type != OPERATOR)
    {
        fputs("push ", ctx->output_file);
    }

    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);
    fprintf(ctx->output_file, "out\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_in(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//


    fprintf(ctx->output_file, "in\n"
                              "pop ");
    asm_node(ctx, cur_node->left);
    fputc('\n', ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_call(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "push BP\n");
    fprintf(ctx->output_file, "call ");
    asm_node(ctx, cur_node->left);
    fprintf(ctx->output_file, ":\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_hlt(lang_ctx_t* ctx, node_t* cur_node)
{
    ASSERT(ctx);
    ASSERT(cur_node);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file, "hlt\n\n");

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//
