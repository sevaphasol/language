#include <string.h>
#include <sys/stat.h>
#include "lang.h"
#include "custom_assert.h"
#include "ctype.h"
#include "node_allocator.h"
#include "graph_dump.h"

//———————————————————————————————————————————————————————————————————//

#define _DSL_DEFINE_
#include "dsl.h"

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize (lang_ctx_t* ctx);

//-------------------------------------------------------------------//

static lang_status_t get_token         (lang_ctx_t* ctx,
                                        node_t**    ret_node);

static lang_status_t get_str_node      (lang_ctx_t* ctx,
                                        const char* str,
                                        node_t**    ret_node);

static bool          acceptable_symbol (char        symbol);

static void          skip_spaces       (lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    //---------------------------------------------------------------//

    ctx->cur_line = 1;

    while (*ctx->code != '\0')
    {
        node_t* cur_token_node = nullptr;
        VERIFY(get_token(ctx, &cur_token_node),
               return LANG_GET_TOKEN_ERROR);

        ctx->nodes[ctx->n_nodes++] = cur_token_node;

        skip_spaces(ctx);
    }

    ctx->nodes[ctx->n_nodes] = _POISON;

    //---------------------------------------------------------------//

    // graph_dump(ctx, ctx->nodes[0], ARR);

    //---------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_token(lang_ctx_t* ctx, node_t** ret_node)
{
    ASSERT(ctx);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    if (isdigit(*ctx->code))
    {
        *ret_node = _NUMBER((int) strtod(ctx->code, &ctx->code));

        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    if (isalpha(*ctx->code))
    {
        char buf[MaxStrLength] = {};

        for (int i = 0; acceptable_symbol(*ctx->code); i++)
        {
            buf[i] = *(ctx->code++);
        }

        VERIFY(get_str_node(ctx, buf, ret_node),
               return LANG_GET_STR_NODE_ERROR);

        return LANG_SUCCESS;
    }

    //---------------------------------------------------------------//

    for (int i = 1; i < nOperators; i++)
    {
        if (OperatorsTable[i].name[0] == ctx->code[0])
        {
            *ctx->code++;

            *ret_node = _OPERATOR(OperatorsTable[i].code);

            return LANG_SUCCESS;
        }
    }

    //---------------------------------------------------------------//

    fprintf(stderr, _RED("Syntax Error")". Line"
                    _PURPLE(" %ld") ". "
                    "Unknown operator "
                    _TURQUOISE("%c\n"),
                    ctx->cur_line, ctx->code[0]);

    //---------------------------------------------------------------//

    return LANG_UNKNOWN_OPERATOR_ERROR;
}

//===================================================================//

lang_status_t get_str_node(lang_ctx_t* ctx,
                           const char* str,
                           node_t**    ret_node)
{
    ASSERT(ctx);
    ASSERT(str);
    ASSERT(ret_node);

    //---------------------------------------------------------------//

    for (int i = 1; i < nOperators; i++)
    {
        if (strncmp(OperatorsTable[i].name, str,
                    OperatorsTable[i].len) == 0)
        {
            *ret_node = _OPERATOR(OperatorsTable[i].code);

            return LANG_SUCCESS;
        }
    }

    size_t len = strlen(str);

    for (size_t id_ind = 0; id_ind < ctx->name_table.n_names; id_ind++)
    {
        if (strncmp(ctx->name_table.names[id_ind].name, str, len) == 0)
        {
            *ret_node = _IDENTIFIER(id_ind);

            return LANG_SUCCESS;
        }
    }

    ctx->name_table.names[ctx->name_table.n_names++] = {
                                     .name      = strdup(str),
                                     .len       = len};

    //---------------------------------------------------------------//

    *ret_node = _IDENTIFIER(ctx->name_table.n_names - 1);

    return LANG_SUCCESS;
}

//===================================================================//

void skip_spaces(lang_ctx_t* ctx)
{
    ASSERT(ctx);

    while (isspace(*ctx->code))
    {
        if (*ctx->code == '\n') {ctx->cur_line++;}

        ctx->code++;
    }
}

//===================================================================//

bool acceptable_symbol(char symbol)
{
    return (isalpha(symbol) || symbol == '_' || symbol == '?');
}

//———————————————————————————————————————————————————————————————————//

#define _DSL_UNDEF_
#include "dsl.h"

//———————————————————————————————————————————————————————————————————//
