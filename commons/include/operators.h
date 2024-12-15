#ifndef NODE_VALUE_H__
#define NODE_VALUE_H__

//———————————————————————————————————————————————————————————————————//

#include <stdlib.h>

//———————————————————————————————————————————————————————————————————//

enum value_type_t
{
    OPERATOR   = 1,
    NUMBER     = 2,
    IDENTIFIER = 3,
};

//———————————————————————————————————————————————————————————————————//

enum operator_code_t
{
    UNDEFINED     = 0,
    ADD           = 1,
    SUB           = 2,
    MUL           = 3,
    DIV           = 4,
    ASSIGNMENT    = 5,
    OPEN_BRACKET  = 6,
    CLOSE_BRACKET = 7,
    BODY_START    = 8,
    BODY_END      = 9,
    STATEMENT     = 10,
    PARAM_LINKER  = 11,
    IF            = 12,
    WHILE         = 13,
    NEW_VAR       = 14,
    NEW_FUNC      = 15,
    RET           = 16,
    COS           = 17,
    SIN           = 18,
    PRINT         = 19,
    SCAN          = 20,
    CALL          = 21,
    PROGRAMM_END  = 22,
};

struct operator_t
{
    operator_code_t code;
    const char*     name;
    size_t          len;
    int             n_children;
};

//———————————————————————————————————————————————————————————————————//

#define STR_AND_LEN(str) str, sizeof(str) / sizeof(char)

const operator_t OperatorsTable[] =
{ //  .code       .name and len           .n_childs
    {UNDEFINED    , nullptr, 0               , 0},
    {ADD          , STR_AND_LEN("+")         , 2},
    {SUB          , STR_AND_LEN("-")         , 2},
    {MUL          , STR_AND_LEN("*")         , 2},
    {DIV          , STR_AND_LEN("/")         , 2},
    {ASSIGNMENT   , STR_AND_LEN("=")         , 2},
    {OPEN_BRACKET , STR_AND_LEN("(")         , 1},
    {CLOSE_BRACKET, STR_AND_LEN(")")         , 1},
    {BODY_START   , STR_AND_LEN("{")         , 1},
    {BODY_END     , STR_AND_LEN("}")         , 1},
    {STATEMENT    , STR_AND_LEN("sosal?")    , 2},
    {PARAM_LINKER , STR_AND_LEN("krasivaya") , 2},
    {IF           , STR_AND_LEN("if")        , 2},
    {WHILE        , STR_AND_LEN("while")     , 2},
    {NEW_VAR      , STR_AND_LEN("krosovka")  , 2},
    {NEW_FUNC     , STR_AND_LEN("korobka")   , 2},
    {RET          , STR_AND_LEN("return")    , 1},
    {COS          , STR_AND_LEN("cosipinus") , 1},
    {SIN          , STR_AND_LEN("sipinus")   , 1},
    {PRINT        , STR_AND_LEN("print")     , 1},
    {SCAN         , STR_AND_LEN("scan")      , 1},
    {CALL         , STR_AND_LEN("please")    , 0},
    {PROGRAMM_END , STR_AND_LEN("sosal!")    , 0},
};

const int nOperators = sizeof(OperatorsTable) / sizeof(operator_t);

#undef STR_AND_LEN

//———————————————————————————————————————————————————————————————————//

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
    int               n_params;
    bool              is_inited;
};

//———————————————————————————————————————————————————————————————————//

typedef int number_t;

//———————————————————————————————————————————————————————————————————//

union value_t
{
    operator_code_t   operator_code;
    size_t            id_index;
    number_t          number;
};

//———————————————————————————————————————————————————————————————————//

#endif // NODE_VALUE_H__
