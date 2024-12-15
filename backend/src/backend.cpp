#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "lang.h"
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"
#include "io_interaction.h"

//———————————————————————————————————————————————————————————————————//

static lang_status_t backend_ctx_ctor (lang_ctx_t* ctx,
                                       int argc,
                                       const char* argv[]);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    lang_ctx_t ctx = {};

    //-------------------------------------------------------------------//

    node_allocator_t node_allocator;
    ctx.node_allocator = &node_allocator;

    //-------------------------------------------------------------------//

    VERIFY(lang_ctx_ctor(&ctx, argc, argv),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    _VERIFY(read_tree_from_file(&ctx, argc, argv),
            return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    graph_dump(&ctx, TREE);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

lang_status_t read_tree_from_file(lang_ctx_t* ctx,
                                  int         argc,
                                  const char* argv)
{
    ASSERT(ctx);
    ASSERT(argv);

    //-------------------------------------------------------------------//



    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

