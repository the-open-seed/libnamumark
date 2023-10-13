//
// Created by hibot on 23/08/24.
//

#include "parser.h"
#include "./parser_utils.c"

// Just perform the parsing process
// parser
ast_node *parse(const char *text, const size_t text_size) {
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
            str_buf = append_buffer(str_buf, &str_buf_size, text[i + 1]);
            i++;
            continue;
        }
        const size_t syntax_defines_size = sizeof(syntax_defines) / sizeof(struct syntax);
        const struct syntax *current_node_syntax = get_syntax_by_type(current_node->type);
        for (size_t j = 0; j < syntax_defines_size; j++) {
            struct syntax current_syntax = syntax_defines[j];
            // !!! SYNTAX END STRING !!!
            // find syntax end
            bool is_syntax_end = starts_with(text + i, text_size - i, current_syntax.end);
            if (current_node->type == current_syntax.type && is_syntax_end) {
                // !!! MAKE TEXT NODE !!!
                if (str_buf_size > 0) {
                    // check previous node type is text
                    if (current_node->children_size > 0 &&
                        current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_TEXT) {
                        append_text_node(current_node->children[current_node->children_size - 1], str_buf, str_buf_size);
                        str_buf = reset_buffer(str_buf, &str_buf_size, true);
                    } else {
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size,
                                                          AST_DATA_TYPE_STRING, i);
                        ast_node_add_child(current_node, new_node);
                        str_buf = reset_buffer(str_buf, &str_buf_size, false);
                    }
                }
                // pop current node from node_stack
                current_node = stack_pop(node_stack);
                // process end of AST_NODE_TYPE_BLOCKQUOTE in AST_NODE_TYPE_BLOCKQUOTE
                if (current_syntax.flags & SYNTAX_FLAG_LINE_ALLOW_MULTIPLE) {
                    while (current_node->type == current_syntax.type) {
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
            else if (strlen(current_syntax.end) != 0 && is_syntax_end &&
                     type_exists_in_stack(node_stack, current_syntax.type)) {
                str_buf = reset_buffer(str_buf, &str_buf_size, true);
                for (const char *p = text + current_node->index; p < text + i; p++) {
                    if (*p == '\\') {
                        p++;
                    }
                    str_buf = append_buffer(str_buf, &str_buf_size, *p);
                }
                // remove current node from node_stack
                ast_node_remove_child(node_stack->data[node_stack->size - 1],
                                      ((ast_node *) node_stack->data[node_stack->size - 1])->children_size - 1);
                current_node = stack_pop(node_stack);
                i += strlen(current_syntax.end) - 1;
                is_break = true;
                break;
            }
            // !!! SYNTAX START STRING !!!
            bool is_syntax_start = starts_with(text + i, text_size - i, current_syntax.start);
            // process AST_NODE_TYPE_BLOCKQUOTE in AST_NODE_TYPE_BLOCKQUOTE
            if (current_node_syntax->flags & SYNTAX_FLAG_LINE_ALLOW_MULTIPLE && current_syntax.type == current_node->type &&
                str_buf_size == 0 && is_syntax_start) {
                // create new node
                ast_node *new_node = ast_node_new(current_syntax.type, NULL, 0, AST_DATA_TYPE_NONE, i);
                ast_node_add_child(current_node, new_node);
                stack_push(node_stack, current_node);
                current_node = new_node;
                i += strlen(current_syntax.start) - 1;
                is_break = true;
                break;
            }
            // find syntax start
            if (is_syntax_start && (!(current_syntax.flags & SYNTAX_FLAG_LINE) || i == 0 || text[i - 1] == '\n')) {
                // check previous node flag is SYNTAX_FLAG_MARGE_SAME_TYPE
                if (current_node->children_size > 0 && current_syntax.flags & SYNTAX_FLAG_MARGE_SAME_TYPE &&
                    current_node->children[current_node->children_size - 1]->type == current_syntax.type) {
                    stack_push(node_stack, current_node);
                    // combine
                    current_node = current_node->children[current_node->children_size - 1];
                    // add \n to str_buf
                    str_buf = append_buffer(str_buf, &str_buf_size, '\n');
                    // skip syntax
                    i += strlen(current_syntax.start) - 1;
                    is_break = true;
                    break;
                }
                // Oops, I want to make this code more readable(function?), but I can't. :(
                if (str_buf_size > 0) {
                    // check previous node type is text
                    if (current_node->children_size > 0 &&
                        current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_TEXT) {
                        // append str_buf to previous node
                        ast_node *previous_node = current_node->children[current_node->children_size - 1];
                        append_text_node(previous_node, str_buf, str_buf_size);
                        str_buf = reset_buffer(str_buf, &str_buf_size, true);
                    } else {
                        // create new node
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size,
                                                          AST_DATA_TYPE_STRING, i);
                        ast_node_add_child(current_node, new_node);
                        str_buf = reset_buffer(str_buf, &str_buf_size, false);
                    }
                }

                // process AST_NODE_TYPE_COLOR
                if (current_syntax.type == AST_NODE_TYPE_COLOR) {
                    if (!starts_with_color(text + i + strlen(current_syntax.start) - 1,
                                           text_size - i - strlen(current_syntax.start) + 1)) {
                        continue;
                    }
                    ast_data_color *data = calloc(sizeof(ast_data_color), 1);
                    if (data == NULL) {
                        abort();
                    }
                    // get color
                    const char *color = text + i + strlen(current_syntax.start) - 1;
                    size_t color_size = 0;
                    size_t max_color_size = text_size - i - strlen(current_syntax.start) + 1;
                    for (size_t k = 0; k < max_color_size; k++) {
                        if (color[k] == ' ' || color[k] == '\n' || color[k] == '\0') {
                            break;
                        }
                        color_size++;
                    }
                    size_t dark_color_pos = 0;
                    for (size_t k = 0; k < color_size; k++) {
                        if (color[k] == ',') {
                            dark_color_pos = k + 1;
                            break;
                        }
                    }
                    if (dark_color_pos != 0) {
                        data->dark_color = calloc(sizeof(char), dark_color_pos + 1);
                        if (data->dark_color == NULL) {
                            abort();
                        }
                        // #fff,#000 => #default_color,#dark_color
                        memcpy(data->dark_color, color + dark_color_pos, color_size - dark_color_pos);
                        data->dark_color_size = color_size - dark_color_pos - 1;
                    }
                    data->color = calloc(sizeof(char), dark_color_pos == 0 ? color_size + 1 : dark_color_pos);
                    if (data->color == NULL) {
                        abort();
                    }
                    memcpy(data->color, color, dark_color_pos == 0 ? color_size : dark_color_pos - 1);
                    data->color_size = dark_color_pos == 0 ? color_size : dark_color_pos - 1;
                    ast_node *new_node = ast_node_new(current_syntax.type, data, 0, AST_DATA_TYPE_COLOR, i);
                    ast_node_add_child(current_node, new_node);
                    stack_push(node_stack, current_node);
                    current_node = new_node;
                    i += color_size + strlen(current_syntax.start) - 1;
                    is_break = true;
                    break;
                }
                if (current_syntax.type == AST_NODE_TYPE_LINK) {
                    // find end of link syntax
                    size_t link_size = 0;
                    size_t link_split = 0;
                    bool is_link_syntax = false;
                    char *link_buf = calloc(sizeof(char), 1);
                    // get link. we get all things needed to create link node at once
                    for (size_t k = i + strlen(current_syntax.start); k < text_size - 1; k++) {
                        if (text[k] == '\\') {
                            link_buf = append_buffer(link_buf, &link_size, text[k + 1]);
                            k++;
                            continue;
                        }
                        if (text[k] == '\n') {
                            break;
                        }
                        if (text[k] == '|' && link_split == 0) {
                            link_split = k;
                            continue;
                        }
                        if (starts_with(text + k, text_size - k, current_syntax.end)) {
                            is_link_syntax = true;
                            break;
                        }
                        link_buf = append_buffer(link_buf, &link_size, text[k]);
                    }
                    link_size++;
                    if (!is_link_syntax) {
                        free(link_buf);
                        continue;
                    }
                    // create new data struct
                    ast_data_link *data = calloc(sizeof(ast_data_link), 1);
                    if (data == NULL) {
                        abort();
                    }
                    if (link_split == 0) {
                        data->link = calloc(sizeof(char), link_size + 1);
                        if (data->link == NULL) {
                            abort();
                        }
                        memcpy(data->link, link_buf, link_size);
                        data->link_size = link_size - 1;
                    } else {
                        data->link = calloc(sizeof(char), link_split + 1);
                        if (data->link == NULL) {
                            abort();
                        }
                        memcpy(data->link, text + i + 2, link_split - i - 2);
                        data->link[link_split - 1 - i] = '\0';
                        data->link_size = link_split - i - 2;
                    }
                    // create new node
                    ast_node *new_node = ast_node_new(current_syntax.type, data, 1, AST_DATA_TYPE_LINK, i);
                    // add new node to current node
                    ast_node_add_child(current_node, new_node);
                    // push current node to node_stack
                    stack_push(node_stack, current_node);
                    // set new node as current node
                    current_node = new_node;
                    // skip syntax
                    i += strlen(current_syntax.start) - 1;
                    if (link_split != 0) {
                        i = link_split;
                    }
                    is_break = true;
                    break;
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
        // append text[i] to str_buf
        // realloc str_buf
        str_buf = append_buffer(str_buf, &str_buf_size, text[i]);

        // if current node is LINE syntax, and text[i] is '\n', this is not correct syntax
        if (current_node_syntax->flags & SYNTAX_FLAG_LINE && text[i] == '\n') {
            // resize buf
            char *buf = realloc(str_buf, sizeof(char) * (str_buf_size + strlen(current_node_syntax->start)) + 1);
            if (buf == NULL) {
                abort();
            }
            str_buf = buf;
            // move buf content(must use memmove, because str_buf and str_buf+strlen(current_syntax->start) overlaps)
            memmove(str_buf + strlen(current_node_syntax->start), str_buf, str_buf_size);
            // copy start syntax to buf
            memcpy(str_buf, current_node_syntax->start, strlen(current_node_syntax->start));
            str_buf_size += strlen(current_node_syntax->start);
            ast_node *parent = node_stack->data[node_stack->size - 1];
            // remove current node from node_stack
            ast_node_remove_child(parent, parent->children_size - 1);
            // change current node
            current_node = stack_pop(node_stack);
        }
    }
    // when loop ends, check str_buf
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
            free(str_buf);
        } else {
            // create new node
            ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, AST_DATA_TYPE_STRING,
                                              text_size);
            // add new node to current node
            ast_node_add_child(root, new_node);
        }
    }
    if (node_stack->size > 0) {
        // TODO: handling improper syntax error
        while (node_stack->size > 0) {
            ast_node *current = stack_pop(node_stack);
            // make text node

            // move children to root
            for (size_t i = 0; i < current->children_size; i++) {
                ast_node_add_child(root, current->children[i]);
            }
            if (node_stack->size > 0) {
                ast_node_remove_child(node_stack->data[node_stack->size - 1],
                                      ((ast_node *) node_stack->data[node_stack->size - 1])->children_size - 1);
            } else {
                ast_node_remove_child(root, root->children_size - 1);
            }
        }
    }
    return root;
}