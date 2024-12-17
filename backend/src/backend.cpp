#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lang.h"
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"
#include "io_interaction.h"
#include "backend.h"

//———————————————————————————————————————————————————————————————————//

static lang_status_t read_tree_from_file(lang_ctx_t* ctx, node_t** node);
static lang_status_t read_tree(lang_ctx_t* ctx, node_t** node);
static lang_status_t put_node_value(int type, int val, value_t* node_value);
static lang_status_t read_name_table(lang_ctx_t* ctx);
static lang_status_t asm_code(lang_ctx_t* ctx);
static lang_status_t asm_globals(lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    lang_ctx_t ctx = {};

    //-------------------------------------------------------------------//

    node_allocator_t node_allocator;
    ctx.node_allocator = &node_allocator;

    //-------------------------------------------------------------------//

    VERIFY(lang_ctx_ctor(&ctx,
                         argc,
                         argv,
                         BackendDefaultInput,
                         BackendDefaultOutput),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(read_name_table(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(read_tree(&ctx, &ctx.tree),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(graph_dump(&ctx, ctx.tree, TREE),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    VERIFY(asm_code(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

lang_status_t asm_code(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    VERIFY(asm_globals(ctx),
        return LANG_ASM_NODE_ERROR);

    fprintf(ctx->output_file, "push %ld\n"
                              "pop BX\n"
                              "call main:\n"
                              "hlt\n\n",
                              ctx->n_globals);

    VERIFY(asm_node(ctx, ctx->tree),
           return LANG_ASM_NODE_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t asm_globals(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    node_t* cur_node = ctx->tree;

    while (cur_node                                         &&
           cur_node->value_type                == OPERATOR  &&
           cur_node->value.operator_code       == STATEMENT &&
           cur_node->left->value_type          == OPERATOR  &&
           cur_node->left->value.operator_code == ASSIGNMENT)
    {
        VERIFY(asm_assignment(ctx, cur_node->left),
               return LANG_ASM_NODE_ERROR);

        cur_node = cur_node->right;
        ctx->n_globals++;
    }

    ctx->tree = cur_node;

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t read_name_table(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    int n_chars = 0;
    size_t n_name_tables = 0;

    sscanf(ctx->code, "%ld%n", &n_name_tables, &n_chars);
    ctx->code += n_chars;

    //-------------------------------------------------------------------//

    size_t n_ids = 0;
    sscanf(ctx->code, "%ld%n", &n_ids, &n_chars);
    ctx->code += n_chars;

    ctx->name_table.table = (identifier_t*) calloc(n_ids, sizeof(identifier_t));
    VERIFY(!ctx->name_table.table,
           return LANG_STD_ALLOCATE_ERROR);

    int  id_index = 0;
    char buf[MaxStrLength] = {};
    int type = 0;
    int n_params = 0;

    for (int i = 0; i < n_ids; i++)
    {
        int nchars = 0;

        sscanf(ctx->code, " { %d %s %d %d } %n",
                          &id_index,
                          buf,
                          &type,
                          &n_params,
                          &nchars);

        ctx->code += nchars;

        ctx->name_table.table[id_index].type     = (identifier_type_t) type;
        ctx->name_table.table[id_index].n_params = n_params;
        ctx->name_table.table[id_index].name     = strdup(buf);
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t read_tree(lang_ctx_t* ctx, node_t** node)
{
    ASSERT(ctx);
    ASSERT(node);

    //-------------------------------------------------------------------//

    while (isspace(*ctx->code)) {ctx->code++;}

    //-------------------------------------------------------------------//

    if (*ctx->code == '_')
    {
        ctx->code++;

        *node = nullptr;

        return LANG_SUCCESS;
    }

    //-------------------------------------------------------------------//

    int nchars = 0;
    int type   = 0;
    int val    = 0;

    sscanf(ctx->code, "%*[^0-9] %d %d%n", &type, &val, &nchars);

    //-------------------------------------------------------------------//

    value_t node_value = {};

    VERIFY(put_node_value(type, val ,&node_value),
           return LANG_PUT_NODE_VALUE_ERROR);

    //-------------------------------------------------------------------//

    *node = node_ctor(ctx->node_allocator,
                      (value_type_t) type,
                      node_value,
                      0,
                      nullptr,
                      nullptr);

    //-------------------------------------------------------------------//

    ctx->code += nchars;

    VERIFY(read_tree(ctx, &(*node)->left),
           return LANG_READ_LEFT_NODE_ERROR);

    VERIFY(read_tree(ctx, &(*node)->right),
           return LANG_READ_RIGHT_NODE_ERROR);

    //-------------------------------------------------------------------//

    while (isspace(*ctx->code)) {ctx->code++;}

    VERIFY(*(ctx)->code != '}',
           return LANG_INCORRECT_INPUT_SYNTAX_ERROR);

    ctx->code++;

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t put_node_value(int type, int val, value_t* node_value)
{
    ASSERT(node_value);

    //-------------------------------------------------------------------//

    switch(type)
    {
        case OPERATOR:
        {
            node_value->operator_code = (operator_code_t) val;
            break;
        }
        case IDENTIFIER:
        {
            node_value->id_index = val;
            break;
        }
        case NUMBER:
        {
            node_value->number = (number_t) val;
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
