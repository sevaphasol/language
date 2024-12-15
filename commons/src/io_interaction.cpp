#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>

//-------------------------------------------------------------------//

#include "io_interaction.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

static io_status_t parse_argv    (const char** input_file_name,
                                  const char** output_file_name,
                                  int argc,
                                  const char* argv[],
                                  const char* default_input,
                                  const char* default_output);

static io_status_t get_file_size (FILE* file_ptr, size_t* size);

//———————————————————————————————————————————————————————————————————//

io_status_t open_files(FILE**      input_file,
                       FILE**      output_file,
                       int         argc,
                       const char* argv[],
                       const char* default_input,
                       const char* default_output)
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
                      argv,
                      default_input,
                      default_output),
           return IO_PARSE_ARGV_ERROR);

    VERIFY(!input_file_name || !output_file_name,
           return IO_PARSE_ARGV_ERROR);

    //-------------------------------------------------------------------//

    *input_file  = fopen(input_file_name,  "rb");
    *output_file = fopen(output_file_name, "wb");

    VERIFY(!input_file || !output_file,
           return IO_FILE_OPEN_ERROR);

    //-------------------------------------------------------------------//

    return IO_SUCCESS;
}

//===================================================================//

io_status_t parse_argv(const char** input_file_name,
                       const char** output_file_name,
                       int argc,
                       const char* argv[],
                       const char* default_input,
                       const char* default_output)
{
    ASSERT(input_file_name);
    ASSERT(output_file_name);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    switch (argc)
    {
        case 1:
            *input_file_name  = default_input;
            *output_file_name = default_output;
        case 2:
            *input_file_name  = argv[1];
            *output_file_name = default_output;
        case 3:
            *input_file_name  = argv[1];
            *output_file_name = argv[2];
        default:
            *input_file_name  = default_input;
            *output_file_name = default_output;
    }

    //-------------------------------------------------------------------//

    return IO_SUCCESS;
}

//===================================================================//

io_status_t read_in_buf(FILE*   input_file,
                        size_t* size,
                        char**  buf)
{
    ASSERT(input_file);
    ASSERT(size);
    ASSERT(buf);

    //-------------------------------------------------------------------//

    VERIFY(get_file_size(input_file, size),
           return IO_GET_FILE_SIZE_ERROR);

    //-------------------------------------------------------------------//

    *buf = (char*) calloc(*size + 1, sizeof(char));

    VERIFY(!buf,
           return IO_STD_ALLOCATE_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(fread(*buf, sizeof(char), *size, input_file) != *size,
           return IO_FREAD_ERROR);

    //-------------------------------------------------------------------//

    return IO_SUCCESS;
}

//===================================================================//

io_status_t get_file_size(FILE* file_ptr, size_t* size)
{
    ASSERT(file_ptr);
    ASSERT(size);

    //-------------------------------------------------------------------//

    struct stat file_status = {};

    VERIFY((fstat(fileno(file_ptr), &file_status) < 0),
                        return IO_GET_FILE_SIZE_ERROR);

    *size = file_status.st_size;

    //-------------------------------------------------------------------//

    return IO_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
