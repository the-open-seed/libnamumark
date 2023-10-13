//
// Created by hibot on 23. 10. 13.
//
// This file contains some functions and structures for parser
// This file is not part of the library(only used in parser.c), so you don't need to read this file.
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
    size_t flags;
};

#define SYNTAX_FLAG_LINE (0x01 << 0)
#define SYNTAX_FLAG_COLLAPSE (0x01 << 1)
#define SYNTAX_FLAG_MARGE_SAME_TYPE (0x01 << 2)
#define SYNTAX_FLAG_LINE_ALLOW_MULTIPLE (0x01 << 3)
// define syntax_defines
struct syntax syntax_defines[] = {
        {"[* ",        "]",         AST_NODE_TYPE_FOOTNOTE},
        {"= ",         " =\n",      AST_NODE_TYPE_H1,         SYNTAX_FLAG_LINE},
        {"== ",        " ==\n",     AST_NODE_TYPE_H2,         SYNTAX_FLAG_LINE},
        {"=== ",       " ===\n",    AST_NODE_TYPE_H3,         SYNTAX_FLAG_LINE},
        {"==== ",      " ====\n",   AST_NODE_TYPE_H4,         SYNTAX_FLAG_LINE},
        {"===== ",     " =====\n",  AST_NODE_TYPE_H5,         SYNTAX_FLAG_LINE},
        {"====== ",    " ======\n", AST_NODE_TYPE_H6,         SYNTAX_FLAG_LINE},
        {"=# ",        " #=",       AST_NODE_TYPE_H1,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"==# ",       " #==",      AST_NODE_TYPE_H2,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"===# ",      " #===",     AST_NODE_TYPE_H3,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"====# ",     " #====",    AST_NODE_TYPE_H4,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"=====# ",    " #=====",   AST_NODE_TYPE_H5,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"======# ",   " #======",  AST_NODE_TYPE_H6,         SYNTAX_FLAG_LINE | SYNTAX_FLAG_COLLAPSE},
        {"[[",         "]]",        AST_NODE_TYPE_LINK},
        {"'''",        "'''",       AST_NODE_TYPE_BOLD},
        {"''",         "''",        AST_NODE_TYPE_ITALIC},
        {"__",         "__",        AST_NODE_TYPE_UNDERLINE},
        {"~~",         "~~",        AST_NODE_TYPE_STRIKE},
        {"> ",         "\n",        AST_NODE_TYPE_BLOCKQUOTE, SYNTAX_FLAG_LINE | SYNTAX_FLAG_MARGE_SAME_TYPE |
                                                              SYNTAX_FLAG_LINE_ALLOW_MULTIPLE},
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
// Warning: MUST Check String Length(MAX: 29) before adding new color
const char named_colors[][30] = {"aliceblue", "antiquewhite", "aqua", "aquamarine", "azure", "beige", "bisque", "black",
                                 "blanchedalmond", "blue", "blueviolet", "brown", "burlywood", "cadetblue",
                                 "chartreuse",
                                 "chocolate", "coral", "cornflowerblue", "cornsilk", "crimson", "cyan", "darkblue",
                                 "darkcyan", "darkgoldenrod", "darkgray", "darkgreen", "darkgrey", "darkkhaki",
                                 "darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred", "darksalmon",
                                 "darkseagreen", "darkslateblue", "darkslategray", "darkslategrey", "darkturquoise",
                                 "darkviolet", "deeppink", "deepskyblue", "dimgray", "dimgrey", "dodgerblue",
                                 "firebrick",
                                 "floralwhite", "forestgreen", "fuchsia", "gainsboro", "ghostwhite", "gold",
                                 "goldenrod",
                                 "gray", "green", "greenyellow", "grey", "honeydew", "hotpink", "indianred", "indigo",
                                 "ivory", "khaki", "lavender", "lavenderblush", "lawngreen", "lemonchiffon",
                                 "lightblue",
                                 "lightcoral", "lightcyan", "lightgoldenrodyellow", "lightgray", "lightgreen",
                                 "lightgrey",
                                 "lightpink", "lightsalmon", "lightseagreen", "lightskyblue", "lightslategray",
                                 "lightslategrey", "lightsteelblue", "lightyellow", "lime", "limegreen", "linen",
                                 "magenta",
                                 "maroon", "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple",
                                 "mediumseagreen", "mediumslateblue", "mediumspringgreen", "mediumturquoise",
                                 "mediumvioletred", "midnightblue", "mintcream", "mistyrose", "moccasin", "navajowhite",
                                 "navy", "oldlace", "olive", "olivedrab", "orange", "orangered", "orchid",
                                 "palegoldenrod",
                                 "palegreen", "paleturquoise", "palevioletred", "papayawhip", "peachpuff", "peru",
                                 "pink",
                                 "plum", "powderblue", "purple", "red", "rosybrown", "royalblue", "saddlebrown",
                                 "salmon",
                                 "sandybrown", "seagreen", "seashell", "sienna", "silver", "skyblue", "slateblue",
                                 "slategray", "slategrey", "snow", "springgreen", "steelblue", "tan", "teal", "thistle",
                                 "tomato", "turquoise", "violet", "wheat", "white", "whitesmoke", "yellow",
                                 "yellowgreen"};

struct syntax NULL_SYNTAX = {"", "", INT_MAX, 0};

// get syntax by type
struct syntax *get_syntax_by_type(int type) {
    for (size_t i = 0; i < sizeof(syntax_defines) / sizeof(struct syntax); i++) {
        if (syntax_defines[i].type == type) {
            return &syntax_defines[i];
        }
    }
    return &NULL_SYNTAX;
}

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
    if (text[0] != '#') {
        return false;
    }
    for (size_t i = 0; i < named_colors_size; i++) {
        const char *current = named_colors[i];
        if (starts_with(text + 1, text_size - 1, current)) {
            // check if hex ends with ' ', '\n', or '\0', if not, return false
            size_t current_size = strlen(current);
            if (text_size - 1 > current_size && text[current_size + 1] != ' ' && text[current_size + 1] != '\n' &&
                text[current_size + 1] != '\0' && text[current_size + 1] != ',') {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool check_color(const char *text, size_t text_size) {
    return check_3_digit_hex_color(text, text_size) || check_6_digit_hex_color(text, text_size) ||
           check_named_color(text, text_size);
}

// check string starts with hex color('#fff', '#ffffff', '#fff,#000' and so on)
bool starts_with_color(const char *text, const size_t text_size) {
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
        if (!is_hex(text[i]) && text[i] != '#') {
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
    if (result && check_color(text2, text_size - (text2 - text1))) {
        free(text1);
        return true;
    }
    free(text1);
    return false;
}

char* append_buffer(char* buf, size_t *buf_size, char c) {
    char* tmp = realloc(buf, sizeof(char) * (*buf_size + 2));
    if (tmp == NULL) {
        abort();
    }
    buf = tmp;
    buf[*buf_size] = c;
    buf[*buf_size + 1] = '\0';
    *buf_size += 1;
    return buf;
}

void append_text_node(ast_node *text_node, char *text, size_t text_size) {
    char *buf = realloc(text_node->data, sizeof(char) * (text_node->data_size + text_size) + 1);
    if (buf == NULL) {
        abort();
    }
    memcpy(buf + text_node->data_size, text, text_size);
    buf[text_node->data_size + text_size] = '\0';
    text_node->data = buf;
    text_node->data_size += text_size;
}

char* reset_buffer(char* buf, size_t *buf_size, bool free_buf) {
    if (free_buf) {
        free(buf);
    }
    buf = calloc(sizeof(char), 1);
    if (buf == NULL) {
        abort();
    }
    *buf_size = 0;
    return buf;
}