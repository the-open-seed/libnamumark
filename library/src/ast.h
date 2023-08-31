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
    // index in text
    size_t index;
} ast_node;
// AST node functions
ast_node *ast_node_new(int type, void *data, size_t data_size, size_t index);
void ast_node_add_child(ast_node *parent, ast_node *child);
void ast_node_free(ast_node *node);
void ast_node_remove_child(ast_node *parent, size_t index);
void ast_node_print(ast_node *node);
#ifdef __cplusplus
}
#endif
#endif //LIBNAMUMARK_AST_H
