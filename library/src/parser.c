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


// define syntax_defines
struct syntax syntax_defines[] = {
        {"[* ",     "]",         AST_NODE_TYPE_FOOTNOTE},
        {"= ",      " =\n",      AST_NODE_TYPE_H1,         true},
        {"== ",     " ==\n",     AST_NODE_TYPE_H2,         true},
        {"=== ",    " ===\n",    AST_NODE_TYPE_H3,         true},
        {"==== ",   " ====\n",   AST_NODE_TYPE_H4,         true},
        {"===== ",  " =====\n",  AST_NODE_TYPE_H5,         true},
        {"====== ", " ======\n", AST_NODE_TYPE_H6,         true},
        {"[[",      "]]",        AST_NODE_TYPE_LINK},
        {"'''",     "'''",       AST_NODE_TYPE_BOLD},
        {"''",      "''",        AST_NODE_TYPE_ITALIC},
        {"__",      "__",        AST_NODE_TYPE_UNDERLINE},
        {"~~",      "~~",        AST_NODE_TYPE_STRIKE},
        {"> ",      "\n",          AST_NODE_TYPE_BLOCKQUOTE, true},
        {"##",      "\n",          AST_NODE_TYPE_COMMENT},
        {"[",       "]",           AST_NODE_TYPE_FUNCTION},
        {" * ",     "",            AST_NODE_TYPE_LIST},
        {"--",      "--",          AST_NODE_TYPE_STRIKE},
        {"{{{+1 ", "}}}",          AST_NODE_TYPE_BIG_TEXT_1},
        {"{{{+2 ", "}}}",          AST_NODE_TYPE_BIG_TEXT_2},
        {"{{{+3 ", "}}}",          AST_NODE_TYPE_BIG_TEXT_3},
        {"{{{+4 ", "}}}",          AST_NODE_TYPE_BIG_TEXT_4},
        {"{{{+5 ", "}}}",          AST_NODE_TYPE_BIG_TEXT_5},
        {"{{{#!wiki ",      "}}}", AST_NODE_TYPE_WIKI},
        {"{{{#",      "}}}",          AST_NODE_TYPE_COLOR},
        {"{{{",    "}}}",          AST_NODE_TYPE_NO_WIKI},
        {"^^",      "^^",          AST_NODE_TYPE_UPPER_TEXT},
        {",,",      ",,",          AST_NODE_TYPE_LOWER_TEXT},
};

// get syntax by type
struct syntax *get_syntax_by_type(int type) {
    for (size_t i = 0; i < sizeof(syntax_defines) / sizeof(struct syntax); i++) {
        if (syntax_defines[i].type == type) {
            return &syntax_defines[i];
        }
    }
    abort();
}

// we just use this once(might?)
size_t get_ast_node_in_stack_match_type(stack *stack1, int type) {
    ast_node **data = (ast_node **) stack1->data;
    size_t data_size = stack1->size;
    if (data_size == 0) {
        return data_size;
    }
    for (size_t i = data_size - 1;; i--) {
        if (data[i]->type == type) {
            return i;
        }
        if (i == 0) {
            break;
        }
    }
    return data_size;
}


bool type_exists_in_stack(stack *stack1, int type) {
    ast_node **data = (ast_node **) stack1->data;
    size_t data_size = stack1->size;
    if (data_size == 0) {
        return false;
    }
    for (size_t i = data_size - 1;; i--) {
        if (data[i]->type == type) {
            return true;
        }
        if (i == 0) {
            break;
        }
    }
    return false;
}

bool starts_with(const char *text, size_t text_size, char *prefix) {
    size_t prefix_size = strlen(prefix);
    if (prefix_size == 0) {
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

// parser
ast_node *parse(char *text, size_t text_size) {
    // create root node
    ast_node *root = ast_node_new(AST_NODE_TYPE_ROOT, NULL, 0, AST_DATA_TYPE_NONE, 0);
    // create node_stack
    stack *node_stack = stack_new();
    // create current node
    ast_node *current_node = root;
    char *str_buf = calloc(sizeof(char), 1);
    size_t str_buf_size = 0;
    // parse text
    for (size_t i = 0; i < text_size; i++) {
        // parse syntax
        bool is_break = false;
        if (text[i] == '\\' && text_size > i + 1) {
            str_buf[str_buf_size] = text[i + 1];
            char *tmp = realloc(str_buf, sizeof(char *) * (str_buf_size + 2));
            if (tmp == NULL) {
                abort();
            }
            str_buf = tmp;
            str_buf_size++;
            str_buf[str_buf_size] = '\0';
            i++;
            continue;
        }
        for (size_t j = 0; j < sizeof(syntax_defines) / sizeof(struct syntax); j++) {
            struct syntax current_syntax = syntax_defines[j];
            // !!! SYNTAX END STRING !!!
            // find syntax end
            if (current_node->type == current_syntax.type && starts_with(text + i, text_size - i, current_syntax.end)) {
                // !!! MAKE TEXT NODE !!!
                if (str_buf_size > 0) {
                    // check previous node type is text
                    if (current_node->children_size > 0 &&
                        current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_TEXT) {
                        // append str_buf to previous node
                        ast_node *previous_node = current_node->children[current_node->children_size - 1];
                        char *buf = realloc(previous_node->data,
                                            sizeof(char) * (previous_node->data_size + str_buf_size) + 1);
                        if (buf == NULL) {
                            abort();
                        }
                        previous_node->data = buf;
                        // copy str_buf to buf
                        memcpy(previous_node->data + previous_node->data_size, str_buf, str_buf_size);
                        previous_node->data_size += str_buf_size;
                        // reset str_buf(realloc)
                        str_buf = realloc(str_buf, sizeof(char)); // NOLINT(*-suspicious-realloc-usage)
                        if (str_buf == NULL) {
                            abort();
                        }
                        str_buf_size = 0;
                        str_buf[0] = '\0';
                    } else {
                        // create new node
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, AST_DATA_TYPE_STRING, i);
                        // add new node to current node
                        ast_node_add_child(current_node, new_node);
                        // reset str_buf
                        str_buf = calloc(sizeof(char), 1);
                        str_buf_size = 0;
                    }
                }
                // pop current node from node_stack
                current_node = stack_pop(node_stack);
                // process end of AST_NODE_TYPE_BLOCKQUOTE in AST_NODE_TYPE_BLOCKQUOTE
                // previous node is AST_NODE_TYPE_BLOCKQUOTE and current node is AST_NODE_TYPE_BLOCKQUOTE
                if(current_syntax.type==AST_NODE_TYPE_BLOCKQUOTE){
                    // fix bug when AST_NODE_TYPE_BLOCKQUOTE appears more than two times
                    while (current_node->type==AST_NODE_TYPE_BLOCKQUOTE) {
                        current_node = stack_pop(node_stack);
                    }
                }
                // skip syntax
                i += strlen(current_syntax.end) - 1;
                is_break = true;
                break;
            }
            // !!! WRONG SYNTAX END STRING !!!
                // find wrong syntax
            else if (strlen(current_syntax.end) != 0 && starts_with(text + i, text_size - i, current_syntax.end)
                     && type_exists_in_stack(node_stack, current_syntax.type)) {
                // resize buf
                char *buf = realloc(str_buf, sizeof(char) * (str_buf_size + strlen(current_syntax.start)) + 1);
                if (buf == NULL) {
                    abort();
                }
                str_buf = buf;
                struct syntax *wrong_syntax = &syntax_defines[
                        get_ast_node_in_stack_match_type(node_stack,
                                                         ((ast_node *) node_stack->data[node_stack->size - 1])->type)
                ];
                // move buf content
                memmove(str_buf + strlen(wrong_syntax->start), str_buf, str_buf_size);
                // copy start syntax to buf
                memcpy(str_buf, wrong_syntax->start, strlen(wrong_syntax->start));
                str_buf_size += strlen(wrong_syntax->start);
                // remove current node from node_stack
                ast_node_remove_child(node_stack->data[node_stack->size - 1],
                                      ((ast_node *) node_stack->data[node_stack->size - 1])->children_size - 1);
                // change current node
                current_node = stack_pop(node_stack);
                i += strlen(current_syntax.end) - 1;
                is_break = true;
                break;
            }
            // process AST_NODE_TYPE_BLOCKQUOTE in AST_NODE_TYPE_BLOCKQUOTE
            if(current_node->type==AST_NODE_TYPE_BLOCKQUOTE && current_syntax.type==AST_NODE_TYPE_BLOCKQUOTE &&
                    str_buf_size == 0 && starts_with(text + i, text_size - i, current_syntax.start)){
                // create new node
                ast_node *new_node = ast_node_new(current_syntax.type, NULL, 0, AST_DATA_TYPE_NONE, i);
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
            // find syntax start
            if (starts_with(text + i, text_size - i, current_syntax.start) &&
                (!current_syntax.is_line || i == 0 || text[i - 1] == '\n')) {
                // check previous node type is quote
                if(str_buf_size == 0 && current_node->children_size > 0 && current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_BLOCKQUOTE){
                    stack_push(node_stack, current_node);
                    // combine quote
                    current_node = current_node->children[current_node->children_size - 1];
                    // add \n to str_buf
                    str_buf[str_buf_size] = '\n';
                    char *tmp = realloc(str_buf, sizeof(char *) * (str_buf_size + 2));
                    if (tmp == NULL) {
                        abort();
                    }
                    str_buf = tmp;
                    str_buf_size++;
                    str_buf[str_buf_size] = '\0';
                    // skip syntax
                    i += strlen(current_syntax.start) - 1;
                    is_break = true;
                    break;
                }
                if (str_buf_size > 0) {
                    // check previous node type is text
                    if (current_node->children_size > 0 &&
                        current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_TEXT) {
                        // append str_buf to previous node
                        ast_node *previous_node = current_node->children[current_node->children_size - 1];
                        char *buf = realloc(previous_node->data,
                                            sizeof(char) * (previous_node->data_size + str_buf_size) + 1);
                        if (buf == NULL) {
                            abort();
                        }
                        previous_node->data = buf;
                        // copy str_buf to buf
                        memcpy(previous_node->data + previous_node->data_size, str_buf, str_buf_size);
                        previous_node->data_size += str_buf_size;
                        str_buf = realloc(str_buf, sizeof(char)); // NOLINT(*-suspicious-realloc-usage)
                        if (str_buf == NULL) {
                            abort();
                        }
                        str_buf_size = 0;
                    } else {
                        // create new node
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, AST_DATA_TYPE_STRING, i);
                        // add new node to current node
                        ast_node_add_child(current_node, new_node);
                        // reset str_buf
                        str_buf = calloc(sizeof(char), 1);
                        str_buf_size = 0;
                    }
                }
                // create new node
                ast_node *new_node = ast_node_new(current_syntax.type, NULL, 0, AST_DATA_TYPE_NONE, i);
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
        if (is_break) {
            continue;
        }
        // realloc str_buf
        str_buf[str_buf_size] = text[i];
        char *tmp = realloc(str_buf, sizeof(char *) * (str_buf_size + 2));
        if (tmp == NULL) {
            abort();
        }
        str_buf = tmp;
        str_buf_size++;
        str_buf[str_buf_size] = '\0';
        if (current_node->type >= AST_NODE_TYPE_H1 && current_node->type <= AST_NODE_TYPE_H6 && text[i] == '\n') {
            // get syntax
            struct syntax *current_syntax = get_syntax_by_type(current_node->type);
            // resize buf
            char *buf = realloc(str_buf, sizeof(char) * (str_buf_size + strlen(current_syntax->start)) + 1);
            if (buf == NULL) {
                abort();
            }
            str_buf = buf;
            // move buf content(must use memmove, because str_buf and str_buf+strlen(current_syntax->start) overlaps)
            memmove(str_buf + strlen(current_syntax->start), str_buf, str_buf_size);
            // copy start syntax to buf
            memcpy(str_buf, current_syntax->start, strlen(current_syntax->start));
            str_buf_size += strlen(current_syntax->start);
            // remove current node from node_stack
            ast_node_remove_child(node_stack->data[node_stack->size - 1],
                                  ((ast_node *) node_stack->data[node_stack->size - 1])->children_size - 1);
            // change current node
            current_node = stack_pop(node_stack);
        }
    }
    if (str_buf_size > 0) {
        // check previous node type is text
        if (current_node->children_size > 0 &&
            current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_TEXT) {
            // append str_buf to previous node
            ast_node *previous_node = current_node->children[current_node->children_size - 1];
            char *buf = realloc(previous_node->data, sizeof(char) * (previous_node->data_size + str_buf_size) + 1);
            if (buf == NULL) {
                abort();
            }
            previous_node->data = buf;
            // copy str_buf to buf
            memcpy(previous_node->data + previous_node->data_size, str_buf, str_buf_size);
            previous_node->data_size += str_buf_size;
        } else {
            // create new node
            ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, AST_DATA_TYPE_STRING, text_size);
            // add new node to current node
            ast_node_add_child(root, new_node);
        }
    }
    return root;
}