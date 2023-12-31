//
// Created by hibot on 23/08/24.
//

#ifndef LIBNAMUMARK_AST_H
#ifdef __cplusplus
extern "C" {
#endif
#define LIBNAMUMARK_AST_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "stack.h"
// AST node types
#define AST_NODE_TYPE_ROOT 0
#define AST_NODE_TYPE_TEXT 1
#define AST_NODE_TYPE_BOLD 2
#define AST_NODE_TYPE_ITALIC 3
#define AST_NODE_TYPE_UNDERLINE 4
#define AST_NODE_TYPE_STRIKE 5
#define AST_NODE_TYPE_H1 6
#define AST_NODE_TYPE_H2 7
#define AST_NODE_TYPE_H3 8
#define AST_NODE_TYPE_H4 9
#define AST_NODE_TYPE_H5 10
#define AST_NODE_TYPE_H6 11
#define AST_NODE_TYPE_BLOCKQUOTE 12
#define AST_NODE_TYPE_FOOTNOTE 13
#define AST_NODE_TYPE_LINK 14
#define AST_NODE_TYPE_COMMENT 15
#define AST_NODE_TYPE_FUNCTION 16
#define AST_NODE_TYPE_LIST 17
#define AST_NODE_TYPE_BIG_TEXT_1 18
#define AST_NODE_TYPE_BIG_TEXT_2 19
#define AST_NODE_TYPE_BIG_TEXT_3 20
#define AST_NODE_TYPE_BIG_TEXT_4 21
#define AST_NODE_TYPE_BIG_TEXT_5 22
#define AST_NODE_TYPE_UPPER_TEXT 23
#define AST_NODE_TYPE_LOWER_TEXT 24
#define AST_NODE_TYPE_NO_WIKI 25
#define AST_NODE_TYPE_WIKI 26
#define AST_NODE_TYPE_COLOR 27

// AST data types
#define AST_DATA_TYPE_NONE 0
#define AST_DATA_TYPE_STRING 1
#define AST_DATA_TYPE_INT 2
#define AST_DATA_TYPE_BOOL 3
#define AST_DATA_TYPE_FLOAT 4
#define AST_DATA_TYPE_TABLE 5
#define AST_DATA_TYPE_STYLE 6
#define AST_DATA_TYPE_ARGUMENT 7
#define AST_DATA_TYPE_LINK 8
#define AST_DATA_TYPE_COLOR 9
typedef struct ast_data_link {
    char *link;
    size_t link_size;
} ast_data_link;
typedef struct ast_data_color {
    char *color;
    size_t color_size;
    char *dark_color;
    size_t dark_color_size;
} ast_data_color;

// AST node structure
typedef struct ast_node {
    int type;
    // children
    struct ast_node **children;
    // children size
    size_t children_size;
    // data
    void *data;
    // data size
    size_t data_size;
    // data type
    int data_type;
    // index in text
    size_t index;
} ast_node;

// AST node functions
ast_node *ast_node_new(int type, void *data, size_t data_size, int data_type, size_t index);

void ast_node_add_child(ast_node *parent, ast_node *child);

void ast_node_free(ast_node *node);

void ast_node_remove_child(ast_node *parent, size_t index);

#ifdef DEBUG
void ast_node_print(ast_node *node);
#endif
#ifdef __cplusplus
}
#endif
#endif //LIBNAMUMARK_AST_H
