//
// Created by hibot on 23/08/24.
//

#include "ast.h"

#include <stdlib.h>
#include "stack.h"
// AST node functions
ast_node *ast_node_new(int type, void *data, int data_size) {
    ast_node *node = malloc(sizeof(ast_node));
    // check if malloc failed
    if (node == NULL) {
        abort();
    }
    node->type = type;
    node->children = NULL;
    node->children_size = 0;
    node->data = data;
    node->data_size = data_size;
    return node;
}

void ast_node_add_child(ast_node *parent, ast_node *child) {
    struct ast_node **children = realloc(parent->children, sizeof(ast_node *) * (parent->children_size + 1));
    // check if realloc failed
    if (children == NULL) {
        abort();
    }
    children[parent->children_size] = child;
    parent->children = children;
}

void ast_node_free(ast_node *node) {
    // free children(use stack to prevent stack overflow)
    stack *stack = stack_new();
    for (int i = 0; i < node->children_size; i++) {
        stack_push(stack, node->children[i]);
    }
    while (stack->size > 0) {
        ast_node *child = stack_pop(stack);
        for (size_t i = 0; i < child->children_size; i++) {
            stack_push(stack, child->children[i]);
        }
        free(child);
    }
    // free data
    free(node->data);
    // free node
    free(node);
}

void ast_node_remove_child(ast_node *parent, size_t index) {
    // remove child
    ast_node *child = parent->children[index];
    for (size_t i = index; i < parent->children_size - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }
    parent->children_size--;
    // free child
    ast_node_free(child);
}