#include <stdio.h>

#include "lang.h"
#include "frontend.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

lang_status_t tree_output(frontend_ctx_t* ctx, node_t* node, size_t level)
{
    ASSERT(ctx);
    ASSERT(node);

    //-------------------------------------------------------------------//

    for (int i = 0; i < level; i++)
    {
        fputc('\t', ctx->output_file);
    }

    fprintf(ctx->output_file, "{type = %s ", type_name(node->value_type));

    switch (node->value_type)
    {
        case OPERATOR:
        {
            fprintf(ctx->output_file, "code = %d", node->value.operator_code);

            break;
        }
        case IDENTIFIER:
        {
            fprintf(ctx->output_file, "code = %ld", node->value.id_index);

            break;
        }
        case NUMBER:
        {
            fprintf(ctx->output_file, "val = %d", node->value.number);
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
        fputc('\n', ctx->output_file);
        tree_output(ctx, node->left,  level + 1);
    }
    if (node->right)
    {
        tree_output(ctx, node->right, level + 1);
    }

    if (node->left || node->right)
    {
        for (int i = 0; i < level; i++)
        {
            fputc('\t', ctx->output_file);
        }
    }

    fputs("}\n",  ctx->output_file);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//
