//
// Created by hibot on 23/08/24.
//

#include "parser.h"

// structure of syntax(some syntax affects one line(h1), some syntax affects multiple lines(table))
// [* (something)]: footnote
// == (something) ==: h2(must start with line and end with newline)
// [[(something)]]: link
// so... Is there any way to make this file more readable?
struct syntax {
    char *start;
    char *end;
    int type;
    bool is_line;
};


// define syntax_defines
struct syntax syntax_defines[] = {
        {"[* ",        "]",         AST_NODE_TYPE_FOOTNOTE},
        {"= ",         " =\n",      AST_NODE_TYPE_H1,         true},
        {"== ",        " ==\n",     AST_NODE_TYPE_H2,         true},
        {"=== ",       " ===\n",    AST_NODE_TYPE_H3,         true},
        {"==== ",      " ====\n",   AST_NODE_TYPE_H4,         true},
        {"===== ",     " =====\n",  AST_NODE_TYPE_H5,         true},
        {"====== ",    " ======\n", AST_NODE_TYPE_H6,         true},
        {"[[",         "]]",        AST_NODE_TYPE_LINK},
        {"'''",        "'''",       AST_NODE_TYPE_BOLD},
        {"''",         "''",        AST_NODE_TYPE_ITALIC},
        {"__",         "__",        AST_NODE_TYPE_UNDERLINE},
        {"~~",         "~~",        AST_NODE_TYPE_STRIKE},
        {"> ",         "\n",        AST_NODE_TYPE_BLOCKQUOTE, true},
        {"##",         "\n",        AST_NODE_TYPE_COMMENT},
        {"[",          "]",         AST_NODE_TYPE_FUNCTION},
        {" * ",        "",          AST_NODE_TYPE_LIST},
        {"--",         "--",        AST_NODE_TYPE_STRIKE},
        {"{{{+1 ",     "}}}",       AST_NODE_TYPE_BIG_TEXT_1},
        {"{{{+2 ",     "}}}",       AST_NODE_TYPE_BIG_TEXT_2},
        {"{{{+3 ",     "}}}",       AST_NODE_TYPE_BIG_TEXT_3},
        {"{{{+4 ",     "}}}",       AST_NODE_TYPE_BIG_TEXT_4},
        {"{{{+5 ",     "}}}",       AST_NODE_TYPE_BIG_TEXT_5},
        {"{{{#!wiki ", "}}}",       AST_NODE_TYPE_WIKI},
        {"{{{#",       "}}}",       AST_NODE_TYPE_COLOR},
        {"{{{",        "}}}",       AST_NODE_TYPE_NO_WIKI},
        {"^^",         "^^",        AST_NODE_TYPE_UPPER_TEXT},
        {",,",         ",,",        AST_NODE_TYPE_LOWER_TEXT},
};

const size_t named_colors_size = 147;
const char named_colors[147][21] = {"aliceblue", "antiquewhite", "aqua", "aquamarine", "azure", "beige", "bisque", "black",
                                    "blanchedalmond", "blue", "blueviolet", "brown", "burlywood", "cadetblue", "chartreuse",
                                    "chocolate", "coral", "cornflowerblue", "cornsilk", "crimson", "cyan", "darkblue",
                                    "darkcyan", "darkgoldenrod", "darkgray", "darkgreen", "darkgrey", "darkkhaki",
                                    "darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred", "darksalmon",
                                    "darkseagreen", "darkslateblue", "darkslategray", "darkslategrey", "darkturquoise",
                                    "darkviolet", "deeppink", "deepskyblue", "dimgray", "dimgrey", "dodgerblue", "firebrick",
                                    "floralwhite", "forestgreen", "fuchsia", "gainsboro", "ghostwhite", "gold", "goldenrod",
                                    "gray", "green", "greenyellow", "grey", "honeydew", "hotpink", "indianred", "indigo",
                                    "ivory", "khaki", "lavender", "lavenderblush", "lawngreen", "lemonchiffon", "lightblue",
                                    "lightcoral", "lightcyan", "lightgoldenrodyellow", "lightgray", "lightgreen", "lightgrey",
                                    "lightpink", "lightsalmon", "lightseagreen", "lightskyblue", "lightslategray",
                                    "lightslategrey", "lightsteelblue", "lightyellow", "lime", "limegreen", "linen", "magenta",
                                    "maroon", "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple",
                                    "mediumseagreen", "mediumslateblue", "mediumspringgreen", "mediumturquoise",
                                    "mediumvioletred", "midnightblue", "mintcream", "mistyrose", "moccasin", "navajowhite",
                                    "navy", "oldlace", "olive", "olivedrab", "orange", "orangered", "orchid", "palegoldenrod",
                                    "palegreen", "paleturquoise", "palevioletred", "papayawhip", "peachpuff", "peru", "pink",
                                    "plum", "powderblue", "purple", "red", "rosybrown", "royalblue", "saddlebrown", "salmon",
                                    "sandybrown", "seagreen", "seashell", "sienna", "silver", "skyblue", "slateblue",
                                    "slategray", "slategrey", "snow", "springgreen", "steelblue", "tan", "teal", "thistle",
                                    "tomato", "turquoise", "violet", "wheat", "white", "whitesmoke", "yellow", "yellowgreen"};

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
// get ast_node in stack match type
// explanation: if we have a stack like this:
// [AST_NODE_TYPE_H1, AST_NODE_TYPE_H2, AST_NODE_TYPE_H3, AST_NODE_TYPE_H4, AST_NODE_TYPE_H5, AST_NODE_TYPE_H6]
// and we want to get AST_NODE_TYPE_H3, we can use this function
// if we want to get AST_NODE_TYPE_H1, we can use this function too
// but if we want to get AST_NODE_TYPE_LINK, we can't use this function
// so... we need to check if the return value is equal to stack->size
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

// check if type exists in stack
// SIMPLE!
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

// if you can't understand this function, you're fool.(so, I'm fool)
bool starts_with(const char *text, size_t text_size, const char *prefix) {
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

bool is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// check string starts with 3 digit hex color('#fff')
bool check_3_digit_hex_color(const char *text, size_t text_size) {
    if (text_size < 4) {
        return false;
    }
    if (text[0] != '#') {
        return false;
    }
    for (size_t i = 1; i < 4; i++) {
        if (!is_hex(text[i])) {
            return false;
        }
    }
    // check if hex ends with ' ', '\n', or '\0', if not, return false
    if (text_size > 4 && text[4] != ' ' && text[4] != '\n' && text[4] != '\0' && text[4] != ',') {
        return false;
    }
    return true;
}

// check string starts with 6 digit hex color('#ffffff')
bool check_6_digit_hex_color(const char *text, size_t text_size) {
    if (text_size < 7) {
        return false;
    }
    if (text[0] != '#') {
        return false;
    }
    for (size_t i = 1; i < 7; i++) {
        if (!is_hex(text[i])) {
            return false;
        }
    }
    // check if hex ends with ' ', '\n', or '\0', if not, return false
    if (text_size > 7 && text[7] != ' ' && text[7] != '\n' && text[7] != '\0' && text[7] != ',') {
        return false;
    }
    return true;
}


// check string starts with named color('red', 'blue' and so on)
bool check_named_color(const char *text, size_t text_size) {
    if(text[0] != '#'){
        return false;
    }
    for(size_t i=0;i<named_colors_size;i++){
        const char* current = named_colors[i];
        if(starts_with(text + 1, text_size - 1, current)){
            // check if hex ends with ' ', '\n', or '\0', if not, return false
            size_t current_size = strlen(current);
            if (text_size - 1 > current_size && text[current_size + 1] != ' ' && text[current_size + 1] != '\n' && text[current_size + 1] != '\0' && text[current_size + 1] != ',') {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool check_color(const char *text, size_t text_size) {
    return check_3_digit_hex_color(text, text_size) || check_6_digit_hex_color(text, text_size) || check_named_color(text, text_size);
}

// check string starts with hex color('#fff', '#ffffff', '#fff,#000' and so on)
bool starts_with_color(const char *text, size_t text_size) {
    // first, split text by ','
    char *text1 = calloc(sizeof(char), text_size + 1);
    if (text1 == NULL) {
        abort();
    }
    memcpy(text1, text, text_size);
    char *text2 = NULL;
    // find ','
    for (size_t i = 0; i < text_size; i++) {
        if (text[i] == ',') {
            text2 = text1 + i + 1;
            break;
        }
        // check is alphabet, or number or '#'
        if (!(text[i] >= 'a' && text[i] <= 'z') && !(text[i] >= 'A' && text[i] <= 'Z') &&
            !(text[i] >= '0' && text[i] <= '9') && text[i] != '#') {
            break;
        }
    }
    bool result = check_color(text1, text_size);
    // check if text2 is NULL
    if (text2 == NULL) {
        // check if text1 is hex color
        free(text1);
        return result;
    }
    if(result && check_color(text2, text_size - (text2 - text1))){
        free(text1);
        return true;
    }
    free(text1);
    return false;
}

// Just perform the parsing process
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
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size,
                                                          AST_DATA_TYPE_STRING, i);
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
                while (current_syntax.type == AST_NODE_TYPE_BLOCKQUOTE && current_node->type == AST_NODE_TYPE_BLOCKQUOTE) {
                    current_node = stack_pop(node_stack);
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
                char *buf = realloc(str_buf, sizeof(char) * (str_buf_size + strlen(current_syntax.end)) + 1);
                if (buf == NULL) {
                    abort();
                }
                str_buf = buf;
                struct syntax *wrong_syntax = &syntax_defines[
                        get_ast_node_in_stack_match_type(node_stack,
                                                         ((ast_node *) node_stack->data[node_stack->size - 1])->type)
                ];
                str_buf_size = 0;
                for(char *p = text + current_node->index; p < text + i; p++){
                    if(*p == '\\'){
                        p++;
                    }
                    str_buf[str_buf_size] = *p;
                    str_buf_size++;
                }
                // realloc str_buf
                str_buf[str_buf_size] = '\0';
                char *tmp = realloc(str_buf, sizeof(char *) * (str_buf_size + 1));
                if (tmp == NULL) {
                    abort();
                }
                str_buf = tmp;
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
            if (current_node->type == AST_NODE_TYPE_BLOCKQUOTE && current_syntax.type == current_node->type && str_buf_size == 0
                    && starts_with(text + i, text_size - i, current_syntax.start)) {
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
                if (str_buf_size == 0 && current_node->children_size > 0 &&
                    current_node->children[current_node->children_size - 1]->type == AST_NODE_TYPE_BLOCKQUOTE) {
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
                // Oops, I want to make this code more readable(function?), but I can't. :(
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
                        ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size,
                                                          AST_DATA_TYPE_STRING, i);
                        // add new node to current node
                        ast_node_add_child(current_node, new_node);
                        // reset str_buf
                        str_buf = calloc(sizeof(char), 1);
                        str_buf_size = 0;
                    }
                }

                // process AST_NODE_TYPE_COLOR
                if(current_syntax.type == AST_NODE_TYPE_COLOR && starts_with_color(text + i + strlen(current_syntax.start) - 1, text_size - i - strlen(current_syntax.start) + 1)){
                    ast_data_color* data = calloc(sizeof(ast_data_color), 1);
                    if(data == NULL){
                        abort();
                    }
                    // get color
                    char* color = text + i + strlen(current_syntax.start) - 1;
                    size_t color_size = 0;
                    size_t max_color_size = text_size - i - strlen(current_syntax.start) + 1;
                    for(size_t k=0;k<max_color_size;k++){
                        if(color[k] == ' ' || color[k] == '\n' || color[k] == '\0'){
                            break;
                        }
                        color_size++;
                    }
                    size_t dark_color_pos = 0;
                    for(size_t k=0;k<color_size;k++){
                        if(color[k] == ','){
                            dark_color_pos = k + 1;
                            break;
                        }
                    }
                    if(dark_color_pos != 0){
                        data->dark_color = calloc(sizeof(char), dark_color_pos + 1);
                        if(data->dark_color == NULL){
                            abort();
                        }
                        // #fff,#000 => #default_color,#dark_color
                        memcpy(data->dark_color, color + dark_color_pos, color_size - dark_color_pos);
                        data->dark_color_size = color_size - dark_color_pos - 1;
                    }
                    data->color = calloc(sizeof(char), dark_color_pos == 0 ? color_size + 1 : dark_color_pos);
                    if(data->color == NULL){
                        abort();
                    }
                    memcpy(data->color, color, dark_color_pos == 0 ? color_size : dark_color_pos - 1);
                    data->color_size = dark_color_pos == 0 ? color_size : dark_color_pos - 1;
                    // create new node
                    ast_node *new_node = ast_node_new(current_syntax.type, data, 0, AST_DATA_TYPE_COLOR, i);
                    // add new node to current node
                    ast_node_add_child(current_node, new_node);
                    // push current node to node_stack
                    stack_push(node_stack, current_node);
                    // set new node as current node
                    current_node = new_node;
                    // skip syntax
                    i += color_size + strlen(current_syntax.start) - 1;
                    is_break = true;
                    break;
                } else if(current_syntax.type == AST_NODE_TYPE_COLOR){
                    continue;
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
            ast_node *new_node = ast_node_new(AST_NODE_TYPE_TEXT, str_buf, str_buf_size, AST_DATA_TYPE_STRING,
                                              text_size);
            // add new node to current node
            ast_node_add_child(root, new_node);
        }
    }
    if (node_stack->size > 0) {
        // TODO: handling improper syntax error
        abort();
    }
    return root;
}