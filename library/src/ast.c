//
// Created by hibot on 23/08/24.
//

#include "ast.h"

// AST node functions
ast_node *ast_node_new(int type, void *data, size_t data_size, size_t index) {
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
    node->index = index;
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
    parent->children_size++;
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
    if(parent->children_size<=index){
        return;
    }
    // remove child
    ast_node *child = parent->children[index];
    for (size_t i = index; i < parent->children_size - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }
    parent->children_size--;
    // free child
    ast_node_free(child);
}
#ifdef DEBUG
void ast_node_print(ast_node *node){
    char** types = (char*[]){"AST_NODE_TYPE_ROOT", "AST_NODE_TYPE_TEXT", "AST_NODE_TYPE_BOLD", "AST_NODE_TYPE_ITALIC", "AST_NODE_TYPE_UNDERLINE", "AST_NODE_TYPE_STRIKE", "AST_NODE_TYPE_H1", "AST_NODE_TYPE_H2", "AST_NODE_TYPE_H3", "AST_NODE_TYPE_H4", "AST_NODE_TYPE_H5", "AST_NODE_TYPE_H6", "AST_NODE_TYPE_BLOCKQUOTE", "AST_NODE_TYPE_FOOTNOTE", "AST_NODE_TYPE_LINK", "AST_NODE_TYPE_COMMENT", "AST_NODE_TYPE_FUNCTION", "AST_NODE_TYPE_LIST"};
    stack *n_stack = stack_new();
    stack *depth_stack = stack_new();
    stack_push(n_stack, node);
    stack_push(depth_stack, 0);
    size_t depth;
    while (n_stack->size > 0) {
        ast_node *c_node = stack_pop(n_stack);
        depth = (size_t)stack_pop(depth_stack);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("%s\n", types[c_node->type]);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("├data: %s\n", (char *)c_node->data);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("├data_size: %zu\n", c_node->data_size);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("%schildren_size: %zu\n", c_node->children_size == 0 ? "└" : "├",c_node->children_size);
        if(c_node->children_size == 0){
            continue;
        }
        for(size_t i = 0; i < depth; i++){
            printf("│  ");
        }
        printf("├children:\n");
        for (size_t i = c_node->children_size - 1; ; i--) {
            stack_push(depth_stack, (void *) (depth + 1));
            stack_push(n_stack, c_node->children[i]);
            if(i == 0){
                break;
            }
        }
    }
}
#endif