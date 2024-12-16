#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "lang.h"
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"
#include "io_interaction.h"
#include "backend.h"

//———————————————————————————————————————————————————————————————————//

static node_t* read_tree_from_file(lang_ctx_t* ctx);
static node_t* read_node(lang_ctx_t* ctx);

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
                         FrontendDefaultOutput,
                         BackendDefaultOutput),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    ctx.tree = read_tree_from_file(&ctx);

    //-------------------------------------------------------------------//

    graph_dump(&ctx, ctx.tree, TREE);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

node_t* read_tree_from_file(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    return read_node(ctx);
}

//===================================================================//

node_t* read_node(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    while (isspace(*ctx->code)) {ctx->code++;}

    if (*ctx->code == '_')
    {
        ctx->code++;

        return nullptr;
    }

    int nchars = 0;
    int type   = 0;
    int val    = 0;

    sscanf(ctx->code, "%*[^0-9] %d %d%n", &type, &val, &nchars);

    value_t node_value = {};

    switch(type)
    {
        case OPERATOR:   node_value.operator_code = (operator_code_t) val; break;
        case IDENTIFIER: node_value.id_index      =                   val; break;
        case NUMBER:     node_value.number        = (number_t)        val; break;
        default:         ASSERT(false);                                    break;
    }

    node_t* ret_node = node_ctor(ctx->node_allocator,
                                 (value_type_t) type,
                                 node_value,
                                 0,
                                 nullptr,
                                 nullptr);

    ctx->code += nchars;

    ret_node->left  = read_node(ctx);
    ret_node->right = read_node(ctx);

    while (isspace(*ctx->code)) {ctx->code++;}

    ctx->code++;

    //-------------------------------------------------------------------//

    return ret_node;
}

//===================================================================//

