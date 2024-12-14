#include <string.h>
#include <sys/stat.h>
#include "lang.h"
#include "frontend.h"
#include "custom_assert.h"
#include "ctype.h"
#include "node_allocator.h"
#include "dsl.h"
#include "graph_dump.h"

//———————————————————————————————————————————————————————————————————//


static node_t* get_token    (frontend_ctx_t* ctx);
static node_t* get_str_node (frontend_ctx_t* ctx, const char* str);

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    ctx->nodes[ctx->n_nodes++] = _OPERATOR(STATEMENT);

    //-------------------------------------------------------------------//

    ctx->cur_line = 1;

    while (*ctx->code != '\0')
    {
        node_t* cur_token_node = get_token(ctx);

        if (cur_token_node)
        {
            ctx->nodes[ctx->n_nodes++] = cur_token_node;
        }

        else
        {
            fprintf(stderr, "PIZDEC\n");

            return LANG_GET_TOKEN_ERROR;
        }

        while (isspace(*ctx->code))
        {
            if (*ctx->code == '\n') {ctx->cur_line++;}

            ctx->code++;
        }
    }

    //-------------------------------------------------------------------//

    graph_dump(ctx, ARR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

node_t* get_token(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    if (isdigit(*ctx->code))
    {
        return _NUMBER((int) strtod(ctx->code, &ctx->code));
    }

    //-------------------------------------------------------------------//

    if (isalpha(*ctx->code))
    {
        char str[MaxStrLength] = {};

        for (int i = 0; !isspace(*ctx->code) && isalpha(*ctx->code); i++)
        {
            str[i] = *(ctx->code++);
        }

        // printf("%ld\n", strlen(str));

        return get_str_node(ctx, str);
    }

    //-------------------------------------------------------------------//

    for (int i = 1; i < nOperators; i++)
    {
        if (strncmp(OperatorsTable[i].name, ctx->code, 1) == 0)
        {
            *ctx->code++;

            return _OPERATOR(OperatorsTable[i].code);
        }
    }

    //-------------------------------------------------------------------//

    return nullptr;
}

//===================================================================//

node_t* get_str_node(frontend_ctx_t* ctx, const char* str)
{
    ASSERT(ctx);
    ASSERT(str);

    //-------------------------------------------------------------------//


    for (int i = 1; i < nOperators; i++)
    {
        if (strcmp(OperatorsTable[i].name, str) == 0)
        {
            return _OPERATOR(OperatorsTable[i].code);
        }
    }


    for (size_t id_index = 0; id_index < ctx->name_table.n_names; id_index++)
    {
    // printf("%s\n", ctx->name_table.table[0].name);
    // printf("%ld\n", ctx->name_table.n_names);

        // printf("%ld, %ld\n", id_index, ctx->name_table.n_names);
        // printf("%s - %s\n", str, ctx->name_table.table[id_index].name);

        if (strncmp(ctx->name_table.table[id_index].name, str,
            ctx->name_table.table[id_index].len) == 0)
        {
            return _IDENTIFIER(id_index);
        }
    }

    ctx->name_table.table[ctx->name_table.n_names++] =
                                    {.type = UNKNOWN,
                                     .name = strdup(str),
                                     .len = strlen(str),
                                     .is_inited = false};
    // printf("%s\n", ctx->name_table.table[0].name);
    // printf("%ld\n", ctx->name_table.n_names);

    return _IDENTIFIER(ctx->name_table.n_names - 1);
}

//———————————————————————————————————————————————————————————————————//
