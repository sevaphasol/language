#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "frontend.h"
#include "custom_assert.h"
#include "graph_dump.h"

//———————————————————————————————————————————————————————————————————//

static lang_status_t parse_argv (const char** input_file_name,
                                 const char** output_file_name,
                                 int argc,
                                 const char* argv[]);

static lang_status_t open_files (FILE**      input_file,
                                 FILE**      output_file,
                                 int         argc,
                                 const char* argv[]);

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    FILE* input_file  = nullptr;
    FILE* output_file = nullptr;

    VERIFY(open_files(&input_file,
                      &output_file,
                      argc,
                      argv),
           return EXIT_FAILURE); //TODO lang_ctx, close files when error

    //-------------------------------------------------------------------//

    size_t n_nodes = 0;

    node_t** tokenized_nodes = tokenize(input_file, &n_nodes);

    VERIFY(!tokenized_nodes, return EXIT_FAILURE);

    //-------------------------------------------------------------------//

    node_t* code_tree = syntax_analyze(tokenized_nodes, n_nodes);

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}

//===================================================================//

void syntax_error()
{
    fprintf(stderr, "Syntax Error\n");
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
