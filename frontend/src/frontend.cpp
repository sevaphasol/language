#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "frontend.h"
#include "custom_assert.h"
#include "graph_dump.h"
#include "node_allocator.h"

//———————————————————————————————————————————————————————————————————//

static lang_status_t parse_argv        (const char** input_file_name,
                                        const char** output_file_name,
                                        int argc,
                                        const char* argv[]);

static lang_status_t open_files        (FILE**      input_file,
                                        FILE**      output_file,
                                        int         argc,
                                        const char* argv[]);

static lang_status_t frontend_ctx_ctor (frontend_ctx_t* ctx,
                                        int argc,
                                        const char* argv[]);

static lang_status_t read_code         (frontend_ctx_t* ctx);

static lang_status_t get_file_size     (FILE* file_ptr, size_t* size);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    frontend_ctx_t ctx = {};

    //-------------------------------------------------------------------//

    node_allocator_t node_allocator = {};
    ctx.node_allocator = &node_allocator;

    //-------------------------------------------------------------------//

    VERIFY(frontend_ctx_ctor(&ctx, argc, argv),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    VERIFY(tokenize(&ctx),
           return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    node_t* code_tree = syntax_analyze(&ctx);

    //-------------------------------------------------------------------//

    graph_dump(&ctx, TREE);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

lang_status_t frontend_ctx_ctor(frontend_ctx_t* ctx,
                                int argc,
                                const char* argv[])
{
    ASSERT(ctx);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    VERIFY(open_files(&ctx->input_file,
                      &ctx->output_file,
                      argc,
                      argv),
           return LANG_OPEN_FILES_ERROR);

    VERIFY(read_code(ctx),
           return LANG_READ_CODE_ERROR);

    VERIFY(node_allocator_ctor(ctx->node_allocator,
                               nAllocatedNodes),
           return LANG_NODE_ALLOCATOR_CTOR_ERROR);

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t open_files(FILE**      input_file,
                         FILE**      output_file,
                         int         argc,
                         const char* argv[])
{
    ASSERT(input_file);
    ASSERT(output_file);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    const char* input_file_name  = nullptr;
    const char* output_file_name = nullptr;

    VERIFY(parse_argv(&input_file_name,
                      &output_file_name,
                      argc,
                      argv),
           return LANG_PARSE_ARGV_ERROR);

    VERIFY(!input_file_name || !output_file_name,
           return LANG_PARSE_ARGV_ERROR);

    //-------------------------------------------------------------------//

    *input_file  = fopen(input_file_name,  "rb");
    *output_file = fopen(output_file_name, "wb");

    VERIFY(!input_file || !output_file,
           return LANG_FILE_OPEN_ERROR);

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t parse_argv(const char** input_file_name,
                         const char** output_file_name,
                         int argc,
                         const char* argv[])
{
    ASSERT(input_file_name);
    ASSERT(output_file_name);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    switch (argc)
    {
        case 1:
            *input_file_name  = DefaultInput;
            *output_file_name = DefaultOutput;
        case 2:
            *input_file_name  = argv[1];
            *output_file_name = DefaultOutput;
        case 3:
            *input_file_name  = argv[1];
            *output_file_name = argv[2];
        default:
            *input_file_name  = DefaultInput;
            *output_file_name = DefaultOutput;
    }

    //-------------------------------------------------------------------//

    return LANG_SUCCESS;
}

//===================================================================//

lang_status_t read_code(frontend_ctx_t* ctx)
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

    ctx->name_table.table = (identifier_t*) calloc(input_file_size,
                                                   sizeof(identifier_t));

    VERIFY(!ctx->name_table.table,
           return LANG_STD_ALLOCATE_ERROR);

    ctx->name_table.n_names = 0;

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

//———————————————————————————————————————————————————————————————————//
