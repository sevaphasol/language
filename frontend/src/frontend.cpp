#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "frontend.h"
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"
#include "io_interaction.h"

//———————————————————————————————————————————————————————————————————//

static lang_status_t frontend_ctx_ctor (lang_ctx_t* ctx,
                                        int             argc,
                                        const char*     argv[]);

static lang_status_t tree_output       (lang_ctx_t* ctx,
                                        node_t*         node,
                                        size_t          level);

static lang_status_t name_table_output (lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    lang_ctx_t ctx = {};

    //-------------------------------------------------------------------//

    node_allocator_t node_allocator = {};
    ctx.node_allocator = &node_allocator;

    //-------------------------------------------------------------------//

    VERIFY(lang_ctx_ctor(&ctx,
                         argc,
                         argv,
                         DefaultInput,
                         DefaultOutput),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(tokenize(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(syntax_analysis(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(name_table_output(&ctx),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(tree_output(&ctx, ctx.nodes[0], 0),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(graph_dump(&ctx, TREE),
           lang_ctx_dtor(&ctx);
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(lang_ctx_dtor(&ctx),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

lang_status_t name_table_output (lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    fprintf(ctx->output_file,
            "Number of tables = %d\n\n", 1);

    fprintf(ctx->output_file,
            "Number of indexes = %ld\n"
            "%-10s %-10s %-10s %-10s\n",
            ctx->name_table.n_names,
            "index", "name", "type", "n params");

    for (int i = 0; i < ctx->name_table.n_names; i++)
    {
        fprintf(ctx->output_file,
               "%-10d %-10s %-10d %-10d\n",
               i,
               ctx->name_table.table[i].name,
               ctx->name_table.table[i].type,
               ctx->name_table.table[i].n_params);
    }

    fputc('\n', ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t tree_output(lang_ctx_t* ctx, node_t* node, size_t level)
{
    ASSERT(ctx);
    ASSERT(node);

    //-------------------------------------------------------------------//

    // for (int i = 0; i < level; i++)
    // {
    //     fputc('\t', ctx->output_file);
    // }

    fprintf(ctx->output_file, "{%d ", node->value_type);

    switch (node->value_type)
    {
        case OPERATOR:
        {
            fprintf(ctx->output_file, "%d ", node->value.operator_code);

            break;
        }
        case IDENTIFIER:
        {
            fprintf(ctx->output_file, "%ld ", node->value.id_index);

            break;
        }
        case NUMBER:
        {
            fprintf(ctx->output_file, "%d ", node->value.number);
            break;
        }
        default:
        {
            ASSERT(true);
            break;
        }
    }

    if (node->left)
    {
        tree_output(ctx, node->left,  level + 1);
    }
    else
    {
        fputs(" _ ", ctx->output_file);
    }
    if (node->right)
    {
        tree_output(ctx, node->right, level + 1);
    }
    else
    {
        fputs(" _ ", ctx->output_file);
    }
//
//     if (node->left || node->right)
//     {
//         for (int i = 0; i < level; i++)
//         {
//             fputc('\t', ctx->output_file);
//         }
//     }

    fputs("} ",  ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
