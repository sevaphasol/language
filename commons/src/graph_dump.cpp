#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>

//-------------------------------------------------------------------//

#include "graph_dump.h"
#include "node_allocator.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

static tree_dump_status_t dot_file_init  (FILE*       dot_file);

static tree_dump_status_t make_edges     (node_t*     node,
                                          FILE*       file,
                                          int*        node_number);

static tree_dump_status_t make_elem      (node_t*     node,
                                          FILE*       file,
                                          int*        node_number);

static tree_dump_status_t create_png     (const char* dot_file_name,
                                          const char* png_file_name);

//———————————————————————————————————————————————————————————————————//

tree_dump_status_t graph_dump(size_t* n_dumps, node_t* root)
{
    VERIFY(!root, return TREE_DUMP_STRUCT_NULL_PTR_ERROR);

    //-------------------------------------------------------------------//

    char dot_file_name[FileNameBufSize] = {};
    snprintf(dot_file_name, FileNameBufSize,
             LOGS_DIR "/" DOTS_DIR "/" "%03ld.dot",
             *n_dumps);

    FILE* dot_file = fopen(dot_file_name, "w");
    VERIFY(!dot_file, return TREE_DUMP_FILE_OPEN_ERROR);

    dot_file_init(dot_file);

    //-------------------------------------------------------------------//

    int node_number = 1;
    make_edges(root, dot_file, &node_number);

    fputs("}\n", dot_file);
    fclose(dot_file);

    //-------------------------------------------------------------------//

    char png_file_name[FileNameBufSize] = {};
    snprintf(png_file_name, FileNameBufSize,
             LOGS_DIR "/" IMGS_DIR "/" "%03ld.png",
             *n_dumps);

    create_png(dot_file_name, png_file_name);

    //-------------------------------------------------------------------//

    (*n_dumps)++;

    //-------------------------------------------------------------------//

    return TREE_DUMP_SUCCESS;
}

//===================================================================//

tree_dump_status_t dot_file_init(FILE* dot_file)
{
    ASSERT(dot_file);

    //-------------------------------------------------------------------//

    fprintf(dot_file, "digraph G{\n"
                      "rankdir=HR;\n"
                      "node[style=filled, color=\"%s\", fillcolor=\"%s\","
                      "fontcolor=\"%s\", fontsize=14];\n"
                      "edge[color=\"%s\", fontsize=12, penwidth=1, "
                      "fontcolor = \"%s\"];\n"
                      "bgcolor=\"%s\";\n",
                      NodeBorderColor, NodeBackGroundColor, NodeFontColor,
                      EdgeColor, EdgeFontColor,
                      BackGroundColor);

    //-------------------------------------------------------------------//

    return TREE_DUMP_SUCCESS;
}

//===================================================================//

tree_dump_status_t make_elem(node_t* node, FILE* file, int* node_number)
{
    ASSERT(node);
    ASSERT(file);
    ASSERT(node_number);

    //-------------------------------------------------------------------//

    switch(node->value_type)
    {
        case NUMBER:
        {
            fprintf(file, "elem%p["
                          "shape=\"Mrecord\", "
                          "label= \"{%s | val = %d}\""
                          "];\n",
                          node,
                          "NUMBER",
                          node->value.number);
            break;
        }
        case IDENTIFIER:
        {
            fprintf(file, "elem%p["
                          "shape=\"Mrecord\", "
                          "label= \"{%s | val = %ld}\""
                          "];\n",
                          node,
                          "IDENTIFIER",
                          node->value.id_index);
            break;
        }
        case OPERATOR:
        {
            fprintf(file, "elem%p["
                          "shape=\"Mrecord\", "
                          "label= \"{%s | val = %d}\""
                          "];\n",
                          node,
                          "OPERATOR",
                          node->value.operator_code);
            break;
        }
        default:
        {
            break;
        }
    }

    //-------------------------------------------------------------------//

    return TREE_DUMP_SUCCESS;
}

//===================================================================//

tree_dump_status_t make_edges(node_t* node, FILE* file, int* node_number)
{
    ASSERT(node);
    ASSERT(file);
    ASSERT(node_number);

    //-------------------------------------------------------------------//

    make_elem(node, file, node_number);

    //-------------------------------------------------------------------//

    int head_node_number = *node_number;

    if (node->left)
    {
        int left_node_number = ++*node_number;

        fprintf(file, "elem%p->elem%p;",
                      node, node->left);

        make_edges(node->left, file, node_number);
    }

    if (node->right)
    {
        int right_node_number = ++*node_number;

        fprintf(file, "elem%p->elem%p;",
                       node, node->right);

        make_edges(node->right, file, node_number);
    }

    //-------------------------------------------------------------------//

    return TREE_DUMP_SUCCESS;
}

//===================================================================//

tree_dump_status_t create_png(const char* dot_file_name,
                              const char* png_file_name)
{
    ASSERT(dot_file_name);
    ASSERT(png_file_name);

    //-------------------------------------------------------------------//

    char command[SysCommandBufSize] = {};
    snprintf(command, SysCommandBufSize, "touch %s; dot %s -Tpng -o %s",
             png_file_name, dot_file_name, png_file_name);

    VERIFY(system(command),
           return TREE_DUMP_SYSTEM_COMMAND_ERROR);

    //-------------------------------------------------------------------//

    return TREE_DUMP_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
