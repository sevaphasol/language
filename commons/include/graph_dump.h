#ifndef TREE_DUMP_H__
#define TREE_DUMP_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lang.h"

//———————————————————————————————————————————————————————————————————//

#define LOGS_DIR "logs"
#define DOTS_DIR "dot_files"
#define IMGS_DIR "images"

//———————————————————————————————————————————————————————————————————//

enum tree_dump_status_t
{
    TREE_DUMP_SUCCESS               = 0,
    TREE_DUMP_STRUCT_NULL_PTR_ERROR = 1,
    TREE_DUMP_FILE_OPEN_ERROR       = 2,
    TREE_DUMP_SYSTEM_COMMAND_ERROR  = 3,
};

enum dump_mode_t
{
    TREE = 0,
    ARR  = 1,
};

//———————————————————————————————————————————————————————————————————//

const size_t FileNameBufSize   = 64;
const size_t SysCommandBufSize = 278;

//———————————————————————————————————————————————————————————————————//

const char* const BackGroundColor     = "#9c9c9c";
const char* const NodeBackGroundColor = "#494a4a";
const char* const NodeBorderColor     = "#0a0a0a";
const char* const EdgeColor           = "#000000";
const char* const NodeFontColor       = "#e6e6e6";
const char* const EdgeFontColor       = "#49006a";

//———————————————————————————————————————————————————————————————————//

tree_dump_status_t graph_dump (lang_ctx_t* ctx,
                               node_t*     node,
                               dump_mode_t mode);

//———————————————————————————————————————————————————————————————————//

#endif // TREE_DUMP_H__
