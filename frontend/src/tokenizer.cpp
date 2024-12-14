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

#define _RED(str)       "\033[31m" str "\033[0m"
#define _GREEN(str)     "\033[32m" str "\033[0m"
#define _YELLOW(str)    "\033[33m" str "\033[0m"
#define _BLUE(str)      "\033[34m" str "\033[0m"
#define _PURPLE(str)    "\033[35m" str "\033[0m"
#define _TURQUOISE(str) "\033[36m" str "\033[0m"

//———————————————————————————————————————————————————————————————————//

static node_t* get_token    (frontend_ctx_t* ctx);
static node_t* get_str_node (frontend_ctx_t* ctx, const char* str);

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize(frontend_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    // ctx->nodes[ctx->n_nodes++] = _OPERATOR(STATEMENT);

    //-------------------------------------------------------------------//

    ctx->cur_line = 1;

    while (*ctx->code != '\0')
    {
        node_t* cur_token_node = get_token(ctx);

        VERIFY(!cur_token_node,
               return LANG_GET_TOKEN_ERROR);

        ctx->nodes[ctx->n_nodes++] = cur_token_node;

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

bool acceptable_symbol(char symbol)
{
    return (isalpha(symbol) || symbol == '_' || symbol == '?');
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

        for (int i = 0; acceptable_symbol(*ctx->code); i++)
        {
            str[i] = *(ctx->code++);
        }

        // printf("%s\n", str);

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

    char buf[MaxStrLength];

    fprintf(stderr, _RED("Syntax Error")". Line"
                    _PURPLE(" %ld") ". "
                    "Unknown operator "
                    _TURQUOISE("%c\n"),
                    ctx->cur_line, ctx->code[0]);

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
        if (strncmp(OperatorsTable[i].name, str,
            OperatorsTable[i].len) == 0)
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
            strlen(str)) == 0)
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

#undef _RED
#undef _GREEN
#undef _YELLOW
#undef _BLUE
#undef _PURPLE
#undef _TURQUOIS

//———————————————————————————————————————————————————————————————————//
