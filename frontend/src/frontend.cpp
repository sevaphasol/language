#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"
#include "io_interaction.h"

//———————————————————————————————————————————————————————————————————//

extern lang_status_t tokenize        (lang_ctx_t* ctx);
extern lang_status_t syntax_analysis (lang_ctx_t* ctx);

//-------------------------------------------------------------------//

static lang_status_t tree_output       (lang_ctx_t* ctx,
                                        node_t*     node);

static lang_status_t name_table_output (lang_ctx_t* ctx);

static lang_status_t print_node_value  (FILE*       fp,
                                        node_t*     node);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    lang_ctx_t ctx = {};

    node_allocator_t node_allocator = {};
    ctx.node_allocator = &node_allocator;

    //---------------------------------------------------------------//

    VERIFY(lang_ctx_ctor(&ctx,
                         argc,
                         argv,
                         FrontendDefaultInput,
                         FrontendDefaultOutput),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(tokenize(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(syntax_analysis(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(name_table_output(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(tree_output(&ctx, ctx.nodes[0]),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(graph_dump(&ctx, ctx.nodes[0], TREE),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    VERIFY(lang_ctx_dtor(&ctx),
           return EXIT_FAILURE);

    //---------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

lang_status_t name_table_output(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //---------------------------------------------------------------//

    fprintf(ctx->output_file, "%ld\n", ctx->name_table.n_names);

    for (int i = 0; i < ctx->name_table.n_names; i++)
    {
        fprintf(ctx->output_file,
               "{ %-3d %-10s %-1d %-2d }\n",
               i,
               ctx->name_table.table[i].name,
               ctx->name_table.table[i].type,
               ctx->name_table.table[i].n_params);
    }

    fputc('\n', ctx->output_file);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t tree_output(lang_ctx_t* ctx, node_t* node)
{
    ASSERT(ctx);
    ASSERT(node);

    //---------------------------------------------------------------//

    fprintf(ctx->output_file, "{%d ", node->value_type);

    VERIFY(print_node_value(ctx->output_file, node),
           return LANG_TREE_OUTPUT_ERROR);

    node->left  ? tree_output(ctx, node->left)  : fputs(" _ ", ctx->output_file);
    node->right ? tree_output(ctx, node->right) : fputs(" _ ", ctx->output_file);

    fputs("} ",  ctx->output_file);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t print_node_value(FILE* fp, node_t* node)
{
    ASSERT(fp);
    ASSERT(node);

    //---------------------------------------------------------------//

    switch (node->value_type)
    {
        case OPERATOR:
        {
            fprintf(fp, "%d ", node->value.operator_code);

            break;
        }
        case IDENTIFIER:
        {
            fprintf(fp, "%ld ", node->value.id_index);

            break;
        }
        case NUMBER:
        {
            fprintf(fp, "%d ", node->value.number);

            break;
        }
        default:
        {
            return LANG_PRINT_NODE_VALUE_ERROR;

            break;
        }
    }

    return LANG_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
