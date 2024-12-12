#ifndef TREE_DUMP_H__
#define TREE_DUMP_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include <stdlib.h>

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

//———————————————————————————————————————————————————————————————————//

const size_t FileNameBufSize   = 64;
const size_t SysCommandBufSize = 278;

//———————————————————————————————————————————————————————————————————//

const char* const BackGroundColor     = "#bcbddc";
const char* const NodeBackGroundColor = "#a1d99b";
const char* const NodeBorderColor     = "#756bb1";
const char* const EdgeColor           = "#006d2c";
const char* const NodeFontColor       = "#ba3d3d";
const char* const EdgeFontColor       = "#49006a";

//———————————————————————————————————————————————————————————————————//

tree_dump_status_t graph_dump (size_t* n_dumps, node_t* node);

//———————————————————————————————————————————————————————————————————//

#endif // TREE_DUMP_H__
