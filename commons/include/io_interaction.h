#ifndef _IO_INTERACTION_H__
#define _IO_INTERACTION_H__

//———————————————————————————————————————————————————————————————————//

enum io_status_t
{
    IO_SUCCESS             = 0,
    IO_PARSE_ARGV_ERROR    = 1,
    IO_GET_FILE_SIZE_ERROR = 2,
    IO_STD_ALLOCATE_ERROR  = 3,
    IO_FREAD_ERROR         = 4,
    IO_FCLOSE_ERROR        = 5,
    IO_FILE_OPEN_ERROR     = 6,
};

//———————————————————————————————————————————————————————————————————//

io_status_t open_files  (FILE**      input_file,
                         FILE**      output_file,
                         int         argc,
                         const char* argv[],
                         const char* default_input,
                         const char* default_output);

io_status_t read_in_buf (FILE*   input_file,
                         size_t* size,
                         char**  buf);

//———————————————————————————————————————————————————————————————————//

#endif // _IO_INTERACTION_H__
