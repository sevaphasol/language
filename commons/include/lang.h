#ifndef LANG_H__
#define LANG_H__

//———————————————————————————————————————————————————————————————————//

#include "operators.h"

//———————————————————————————————————————————————————————————————————//

enum lang_status_t
{
    LANG_SUCCESS = 0,
    LANG_ERROR   = 1,
    LANG_READ_CODE_ERROR = 2,
    LANG_PARSE_ARGV_ERROR = 3,
    LANG_FILE_OPEN_ERROR = 4,
    LANG_GET_FILE_SIZE_ERROR = 5,
    LANG_STD_ALLOCATE_ERROR = 6,
    LANG_FREAD_ERROR = 7,
    LANG_FCLOSE_ERROR = 8,
    LANG_NODE_ALLOCATOR_CTOR_ERROR = 9,
    LANG_TOKENIZER_CTX_CTOR_ERROR = 10,
    LANG_SYNTAX_ERROR = 11,
};

//———————————————————————————————————————————————————————————————————//

struct node_t
{
    value_type_t value_type;
    value_t      value;
    node_t*      left;
    node_t*      right;
};

//———————————————————————————————————————————————————————————————————//

#endif // LANG_H__
