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


static lang_status_t tokenizer_ctx_ctor (tokenizer_ctx_t* ctx);
static lang_status_t read_code          (tokenizer_ctx_t* ctx);
static lang_status_t get_file_size      (FILE* file_ptr, size_t* size);
static node_t*       get_token          (tokenizer_ctx_t* ctx);
static node_t*       get_str_node       (tokenizer_ctx_t* ctx, const char* str);
static int           get_num            (char** str);


//———————————————————————————————————————————————————————————————————//

node_t** tokenize(FILE* input_file, size_t* n_nodes)
{
    ASSERT(input_file);

    //-------------------------------------------------------------------//

    tokenizer_ctx_t  ctx = {};

    //-------------------------------------------------------------------//

    node_allocator_t node_allocator = {};
    ctx.node_allocator = &node_allocator;

    ctx.input_file = input_file;

    VERIFY(tokenizer_ctx_ctor(&ctx),
           return nullptr);

    //-------------------------------------------------------------------//

    while (*ctx.code != '\0')
    {
        node_t* cur_token_node = get_token(&ctx);

        if (cur_token_node)
        {
            ctx.nodes[ctx.n_nodes++] = cur_token_node;
        }

        else
        {
            fprintf(stderr, "Syntax Error\n");

            return nullptr;
        }

        while (isspace(*ctx.code)) {ctx.code++;}
    }

    //-------------------------------------------------------------------//

    for (int i = 0; i < ctx.n_nodes; i++)
    {
        graph_dump(&ctx.n_dumps, ctx.nodes[i]);
    }

    //-------------------------------------------------------------------//

    *n_nodes = ctx.n_nodes;

    return ctx.nodes;
}

//===================================================================//

lang_status_t tokenizer_ctx_ctor(tokenizer_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    VERIFY(read_code(ctx),
           return LANG_READ_CODE_ERROR);

    VERIFY(node_allocator_ctor(ctx->node_allocator,
                               nAllocatedNodes),
           return LANG_NODE_ALLOCATOR_CTOR_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t read_code(tokenizer_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    size_t input_file_size = 0;

    VERIFY(get_file_size(ctx->input_file, &input_file_size),
           fclose(ctx->input_file);
           return LANG_GET_FILE_SIZE_ERROR);

    //-------------------------------------------------------------------//

    ctx->code = (char*) calloc(input_file_size + 1, sizeof(char));

    VERIFY(!ctx->code,
           fclose(ctx->input_file);
           return LANG_STD_ALLOCATE_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(fread(ctx->code,
                 sizeof(char),
                 input_file_size,
                 ctx->input_file) != input_file_size,
           fclose(ctx->input_file);
           return LANG_FREAD_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(fclose(ctx->input_file),
           return LANG_FCLOSE_ERROR);

    //-------------------------------------------------------------------//

    ctx->nodes = (node_t**) calloc(input_file_size, sizeof(node_t*));

    VERIFY(!ctx->nodes,
           return LANG_STD_ALLOCATE_ERROR);

    ctx->n_nodes = 0;

    //-------------------------------------------------------------------//

    ctx->identifiers_table = (identifier_t*) calloc(input_file_size, sizeof(identifier_t));

    VERIFY(!ctx->identifiers_table,
           return LANG_STD_ALLOCATE_ERROR);

    ctx->n_identifiers = 0;

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t get_file_size(FILE* file_ptr, size_t* size)
{
    ASSERT(file_ptr);
    ASSERT(size);

    //-------------------------------------------------------------------//

    struct stat file_status = {};

    VERIFY((fstat(fileno(file_ptr), &file_status) < 0),
                        return LANG_GET_FILE_SIZE_ERROR);

    *size = file_status.st_size;

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

node_t* get_token(tokenizer_ctx_t* ctx)
{
    ASSERT(ctx);

    //-------------------------------------------------------------------//

    if (isdigit(*ctx->code))
    {
        return _NUMBER(get_num(&ctx->code));
    }

    //-------------------------------------------------------------------//

    if (isalpha(*ctx->code))
    {
        char str[MaxStrLength] = {};

        for (int i = 0; !isspace(*ctx->code); i++)
        {
            str[i] = *(ctx->code++);
        }

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

int get_num(char** str)
{
    ASSERT(str);
    ASSERT(*str);

    int res = 0;

    while (isdigit(**str))
    {
        res = res * 10 + (**str - '0');

        (*str)++;
    }

    return res;
}

//===================================================================//

node_t* get_str_node(tokenizer_ctx_t* ctx, const char* str)
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

    for (size_t id_index = 0; id_index < ctx->n_identifiers; id_index++)
    {
        if (strncmp(ctx->identifiers_table[id_index].name, str,
            ctx->identifiers_table[id_index].len) == 0)
        {
            return _IDENTIFIER(id_index);
        }
    }

    ctx->identifiers_table[ctx->n_identifiers++] = {.type = UNKNOWN,
                                                    .name = str,
                                                    .len = sizeof(str) / sizeof(char),
                                                    .is_inited = false};

    return _IDENTIFIER(ctx->n_identifiers - 1);
}

//———————————————————————————————————————————————————————————————————//
