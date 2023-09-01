//
// Created by hibot on 23/08/24.
//

#include "parser.h"

// structure of syntax(some syntax affects one line(h1), some syntax affects multiple lines(table))
// [* (something)]: footnote
// == (something) ==: h2(must start with line and end with newline)
// [[(something)]]: link
struct syntax {
    char *start;
    char *end;
    int type;
    bool is_line;
};


// define syntaxes
struct syntax syntaxes[] = {
        {"[* ", "]", AST_NODE_TYPE_FOOTNOTE},
        {"= ", " =\n", AST_NODE_TYPE_H1, true},
        {"== ", " ==\n", AST_NODE_TYPE_H2, true},
        {"=== ", " ===\n", AST_NODE_TYPE_H3, true},
        {"==== ", " ====\n", AST_NODE_TYPE_H4, true},
        {"===== ", " =====\n", AST_NODE_TYPE_H5, true},
        {"====== ", " ======\n", AST_NODE_TYPE_H6, true},
        {"[[","]]", AST_NODE_TYPE_LINK},
        {"'''","'''", AST_NODE_TYPE_BOLD},
        {"''","''", AST_NODE_TYPE_ITALIC},
        {"__","__", AST_NODE_TYPE_UNDERLINE},
        {"~~","~~", AST_NODE_TYPE_STRIKE},
        {"> ","", AST_NODE_TYPE_BLOCKQUOTE, true},
        {"##","\n", AST_NODE_TYPE_COMMENT},
        {"[", "]", AST_NODE_TYPE_FUNCTION},
        {" * ", "", AST_NODE_TYPE_LIST},
        {"--","--", AST_NODE_TYPE_STRIKE},
};

// get syntax by type
struct syntax get_syntax_by_type(int type){
    for(size_t i = 0; i < sizeof(syntaxes) / sizeof(struct syntax); i++){
        if(syntaxes[i].type == type){
            return syntaxes[i];
        }
    }
    abort();
}

// we just use this once(might?)
size_t get_ast_node_in_stack_match_type(stack * stack1, int type){
    ast_node ** data = (ast_node **) stack1->data;
    size_t data_size = stack1->size;
    if(data_size == 0){
        return data_size;
    }
    for(size_t i = data_size - 1; ; i--){
        if(data[i]->type == type){
            return i;
        }
        if(i == 0){
            break;
        }
    }
    return data_size;
}


bool type_exists_in_stack(stack * stack1, int type){
    ast_node ** data = (ast_node **) stack1->data;
    size_t data_size = stack1->size;
    if(data_size == 0){
        return false;
    }
    for(size_t i = data_size - 1; ; i--){
        if(data[i]->type == type){
            return true;
        }
        if(i == 0){
            break;
        }
    }
    return false;
}
bool starts_with(const char *text, size_t text_size, char *prefix) {
    size_t prefix_size = strlen(prefix);
    if(prefix_size == 0){
        return true;
    }
    if (text_size < prefix_size) {
        return false;
    }
    for (size_t i = 0; i < prefix_size; i++) {
        if (text[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

#include <stdio.h>
// parser
ast_node* parse(char* text, size_t text_size){
    // create root node
    ast_node *root = ast_node_new(AST_NODE_TYPE_ROOT, NULL, 0, 0);
    // create node_stack
    stack *node_stack = stack_new();
    // create current node
    ast_node *current_node = root;
    char* str_buf = calloc(sizeof(char), 1);
    size_t str_buf_size = 0;
    // parse text
    for (size_t i = 0; i < text_size; i++){
        // parse syntax
        bool is_break = false;
        if(text[i] == '\\' && text_size > i + 1){
            str_buf[str_buf_size] = text[i + 1];
            char * tmp = realloc(str_buf, sizeof (char *) * (str_buf_size + 2));
            if(tmp == NULL){
                abort();
            }
            str_buf = tmp;
            str_buf_size ++;
            str_buf[str_buf_size] = '\0';
            i++;
            continue;
        }
        for(size_t j = 0; j < sizeof(syntaxes) / sizeof(struct syntax); j++){
            struct syntax current_syntax = syntaxes[j];
            if(current_node->type == current_syntax.type && starts_with(text + i, text_size - i, current_syntax.end)){
                if(str_buf_size > 0){
                    // create new node
                    ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, i);
                    // add new node to current node
                    ast_node_add_child(current_node, new_node);
                    // reset str_buf
                    str_buf = calloc(sizeof(char), 1);
                    str_buf_size = 0;
                }
                // pop current node from node_stack
                current_node = stack_pop(node_stack);
                // skip syntax
                i += strlen(current_syntax.end) - 1;
                is_break = true;
                break;
            }
            // find wrong syntax
            else if(strlen(current_syntax.end) != 0 && starts_with(text + i, text_size - i, current_syntax.end)
                && type_exists_in_stack(node_stack, current_syntax.type)){
                // get index of opening syntax
                ast_node *node = node_stack->data[get_ast_node_in_stack_match_type(node_stack, current_syntax.type) + 1];

            }
            if(starts_with(text + i, text_size - i, current_syntax.start) && (!current_syntax.is_line || i == 0 || text[i - 1] == '\n')){
                if(str_buf_size > 0){
                    // create new node
                    ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, i);
                    // add new node to current node
                    ast_node_add_child(current_node, new_node);
                    // reset str_buf
                    str_buf = calloc(sizeof(char), 1);
                    str_buf_size = 0;
                }
                // create new node
                ast_node *new_node = ast_node_new(current_syntax.type, NULL, 0, i);
                // add new node to current node
                ast_node_add_child(current_node, new_node);
                // push current node to node_stack
                stack_push(node_stack, current_node);
                // set new node as current node
                current_node = new_node;
                // skip syntax
                i += strlen(current_syntax.start) - 1;
                is_break = true;
                break;
            }
        }
        if(is_break){
            continue;
        }
        // realloc str_buf
        str_buf[str_buf_size] = text[i];
        char * tmp = realloc(str_buf, sizeof (char *) * (str_buf_size + 2));
        if(tmp == NULL){
            abort();
        }
        str_buf = tmp;
        str_buf_size ++;
        str_buf[str_buf_size] = '\0';
    }
    if(str_buf_size > 0){
        // create new node
        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, text_size);
        // add new node to current node
        ast_node_add_child(root, new_node);
    }
    return root;
}