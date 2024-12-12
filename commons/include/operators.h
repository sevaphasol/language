#ifndef NODE_VALUE_H__
#define NODE_VALUE_H__

//———————————————————————————————————————————————————————————————————//

#include "stdlib.h"

//———————————————————————————————————————————————————————————————————//

enum value_type_t
{
    OPERATOR   = 1,
    NUMBER     = 2,
    IDENTIFIER = 3,
};

//-------------------------------------------------------------------//

enum operator_code_t
{
    UNDEFINED     = 0,
    ADD           = 1,
    SUB           = 2,
    MUL           = 3,
    DIV           = 4,
    COS           = 5,
    SIN           = 6,
    ASSIGNMENT    = 7,
    OPEN_BRACKET  = 8,
    CLOSE_BRACKET = 9,
    BODY_START    = 10,
    BODY_END      = 11,
    STATEMENT     = 12,
    IF            = 13,
    WHILE         = 14,
    RETURN        = 15,
    PARAM_LINKER  = 16,
    NEW_VAR       = 17,
    NEW_FUNC      = 18,
    PROGRAMM_END  = 19,
};

struct operator_t
{
    operator_code_t code;
    const char*     name;
    size_t          len;
};

#define STR_AND_LEN(str) str, sizeof(str) / sizeof(char)

const operator_t OperatorsTable[] =
{ //  .code       .name and len
    {UNDEFINED    , nullptr, 0               },
    {ADD          , STR_AND_LEN("+")         },
    {SUB          , STR_AND_LEN("-")         },
    {MUL          , STR_AND_LEN("*")         },
    {DIV          , STR_AND_LEN("/")         },
    {COS          , STR_AND_LEN("cosipinus") },
    {SIN          , STR_AND_LEN("sipinus")   },
    {ASSIGNMENT   , STR_AND_LEN("OG")        },
    {OPEN_BRACKET , STR_AND_LEN("(")         },
    {CLOSE_BRACKET, STR_AND_LEN(")")         },
    {BODY_START   , STR_AND_LEN("{")         },
    {BODY_END     , STR_AND_LEN("}")         },
    {STATEMENT    , STR_AND_LEN(";")         },
    {IF           , STR_AND_LEN("if")        },
    {WHILE        , STR_AND_LEN("while")     },
    {RETURN       , STR_AND_LEN("return")    },
    {PARAM_LINKER , STR_AND_LEN("huy")       },
    {NEW_VAR      , STR_AND_LEN("int")       },
    {NEW_FUNC     , STR_AND_LEN("func")      },
    {PROGRAMM_END , STR_AND_LEN("pizdec")    }
};

const int nOperators = sizeof(OperatorsTable) / sizeof(operator_t);

//-------------------------------------------------------------------//

enum identifier_type_t
{
    UNKNOWN = 0,
    VAR     = 1,
    FUNC    = 2,
};

struct identifier_t
{
    identifier_type_t type;
    const char*       name;
    size_t            len;
    bool              is_inited;
};

//-------------------------------------------------------------------//

typedef int number_t;

//-------------------------------------------------------------------//

union value_t
{
    operator_code_t   operator_code;
    size_t            id_index;
    number_t          number;
};

//———————————————————————————————————————————————————————————————————//

#endif // NODE_VALUE_H__
