//
// Created by hibot on 23/08/24.
//

#include "ast.h"

// AST node functions
ast_node *ast_node_new(int type, void *data, size_t data_size, int data_type, size_t index) {
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
    node->data_type = data_type;
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

// WARNING: This function will free all children
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
        free(child->children);
        free(child->data);
        free(child);
    }
    // free data
    free(node->data);
    // free node
    free(node);
}

void ast_node_remove_child(ast_node *parent, size_t index) {
    if (parent->children_size <= index) {
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

void ast_node_print(ast_node *node) {
    char **types = (char *[]) {"AST_NODE_TYPE_ROOT", "AST_NODE_TYPE_TEXT", "AST_NODE_TYPE_BOLD", "AST_NODE_TYPE_ITALIC",
                               "AST_NODE_TYPE_UNDERLINE", "AST_NODE_TYPE_STRIKE", "AST_NODE_TYPE_H1",
                               "AST_NODE_TYPE_H2", "AST_NODE_TYPE_H3", "AST_NODE_TYPE_H4", "AST_NODE_TYPE_H5",
                               "AST_NODE_TYPE_H6", "AST_NODE_TYPE_BLOCKQUOTE", "AST_NODE_TYPE_FOOTNOTE",
                               "AST_NODE_TYPE_LINK", "AST_NODE_TYPE_COMMENT", "AST_NODE_TYPE_FUNCTION",
                               "AST_NODE_TYPE_LIST", "AST_NODE_TYPE_BIG_TEXT_1", "AST_NODE_TYPE_BIG_TEXT_2",
                               "AST_NODE_TYPE_BIG_TEXT_3", "AST_NODE_TYPE_BIG_TEXT_4", "AST_NODE_TYPE_BIG_TEXT_5",
                               "AST_NODE_TYPE_UPPER_TEXT", "AST_NODE_TYPE_LOWER_TEXT", "AST_NODE_TYPE_NO_WIKI",
                               "AST_NODE_TYPE_WIKI", "AST_NODE_TYPE_COLOR"};
    stack *n_stack = stack_new();
    stack *depth_stack = stack_new();
    stack_push(n_stack, node);
    stack_push(depth_stack, 0);
    size_t depth;
    while (n_stack->size > 0) {
        ast_node *c_node = stack_pop(n_stack);
        depth = (size_t) stack_pop(depth_stack);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("%s\n", types[c_node->type]);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("├data_type: %d\n", c_node->data_type);
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        if (c_node->data_type == AST_DATA_TYPE_STRING) {
            printf("├data: %s(%zu)\n", (char *) c_node->data, c_node->data_size);
        } else if (c_node->data_type == AST_DATA_TYPE_NONE) {
            printf("├data: NONE\n");
        } else if (c_node->data_type == AST_DATA_TYPE_INT) {
            printf("├data: %d\n", *(int *) c_node->data);
        } else if (c_node->data_type == AST_DATA_TYPE_BOOL) {
            printf("├data: %s\n", *(bool *) c_node->data ? "true" : "false");
        } else if (c_node->data_type == AST_DATA_TYPE_FLOAT) {
            printf("├data: %f\n", *(float *) c_node->data);
        } else if (c_node->data_type == AST_DATA_TYPE_TABLE) {
            printf("├data: TABLE\n");
        } else if (c_node->data_type == AST_DATA_TYPE_STYLE) {
            printf("├data: STYLE\n");
        } else if (c_node->data_type == AST_DATA_TYPE_ARGUMENT) {
            printf("├data: ARGUMENT\n");
        } else if (c_node->data_type == AST_DATA_TYPE_LINK) {
            printf("├data: LINK\n");
            for (size_t i = 0; i <= depth; i++) {
                printf("│  ");
            }
            printf("├link: %s(%zu)\n", ((ast_data_link *) c_node->data)->link,
                   ((ast_data_link *) c_node->data)->link_size);
        } else if (c_node->data_type == AST_DATA_TYPE_COLOR) {
            printf("├data: COLOR\n");
            for (size_t i = 0; i <= depth; i++) {
                printf("│  ");
            }
            printf("├color: %s(%zu)\n", ((ast_data_color *) c_node->data)->color,
                   ((ast_data_color *) c_node->data)->color_size);
            for (size_t i = 0; i <= depth; i++) {
                printf("│  ");
            }
            printf("├dark_color: %s(%zu)\n", ((ast_data_color *) c_node->data)->dark_color,
                   ((ast_data_color *) c_node->data)->dark_color_size);
        }
        for (size_t i = 0; i < depth; i++) {
            printf("│  ");
        }
        printf("├children: %s(%zu)\n", c_node->children_size == 0 ? "NONE" : "", c_node->children_size);
        if (c_node->children_size == 0) {
            continue;
        }
        for (size_t i = c_node->children_size - 1;; i--) {
            stack_push(depth_stack, (void *) (depth + 1));
            stack_push(n_stack, c_node->children[i]);
            if (i == 0) {
                break;
            }
        }
    }
}