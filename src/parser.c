//
// Created by hibot on 23/08/24.
//

#include "parser.h"

struct preprocessor_result {
    bool *is_normal_char;
    char *text;
    size_t text_size;
};

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

// structure of syntax pre-parsing stack(using in preprocessor)
struct syntax_info {
    struct syntax *syntax;
    size_t index;
};

// define syntaxes
struct syntax syntaxes[] = {
        {"[* ", "]", AST_NODE_TYPE_FOOTNOTE},
        {"= ", " =", AST_NODE_TYPE_H1, true},
        {"== ", " ==", AST_NODE_TYPE_H2, true},
        {"=== ", " ===", AST_NODE_TYPE_H3, true},
        {"==== ", " ====", AST_NODE_TYPE_H4, true},
        {"===== ", " =====", AST_NODE_TYPE_H5, true},
        {"====== ", " ======", AST_NODE_TYPE_H6, true},
        {"[[","]]", AST_NODE_TYPE_LINK},
        {"''","''", AST_NODE_TYPE_ITALIC},
        {"'''","'''", AST_NODE_TYPE_BOLD},
        {"__","__", AST_NODE_TYPE_UNDERLINE},
        {"~~","~~", AST_NODE_TYPE_STRIKE},
        {"> ","", AST_NODE_TYPE_BLOCKQUOTE, true},
        {"##","", AST_NODE_TYPE_COMMENT},
};

bool starts_with(const char *text, size_t text_size, char *prefix) {
    size_t prefix_size = strlen(prefix);
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

// preprocessor
// check syntax, if not valid, mark as normal text(bool[])
struct preprocessor_result * preprocessor(const char* text, size_t text_size){
    struct preprocessor_result *result = malloc(sizeof(struct preprocessor_result));
    bool *is_normal_char = calloc(text_size, sizeof(bool));
    if (is_normal_char == NULL) {
        abort();
    }
    char * copied_text = malloc(text_size);
    if (copied_text == NULL) {
        abort();
    }
    size_t copied_text_size = text_size;
    memcpy(copied_text, text, text_size);
    stack *stack = stack_new();
    for(size_t i = 0; i < text_size; i++){
        for(size_t j = 0; j < sizeof(syntaxes) / sizeof(struct syntax); j++){
            struct syntax current_syntax = syntaxes[j];
            if(starts_with(text + i, text_size - i, current_syntax.start)){
                struct syntax_info *syntax_info = malloc(sizeof(struct syntax_info));
                if (syntax_info == NULL) {
                    abort();
                }
                syntax_info->syntax = &current_syntax;
                syntax_info->index = i;
                stack_push(stack, syntax_info);
            }
            if(starts_with(text + i, text_size - i, current_syntax.end)){
                struct syntax_info *syntax = stack_pop(stack);
                if(syntax == NULL){
                    for(size_t k = i; k < i + strlen(syntax->syntax->end); k++){
                        is_normal_char[k] = true;
                    }
                    for(size_t k = syntax->index; k < syntax->index + strlen(syntax->syntax->start); k++){
                        is_normal_char[k] = true;
                    }
                }
                else{
                    free(syntax);
                }
            }
        }
    }
    if(stack->size > 0){
        for(size_t i = 0; i < stack->size; i++){
            struct syntax_info *syntax = stack_pop(stack);
            for(size_t j = syntax->index; j < syntax->index + strlen(syntax->syntax->start); j++){
                is_normal_char[j] = true;
            }
            free(syntax);
        }
    }
    free(stack);
    for(size_t i = 0; i < copied_text_size; i++){
        if(copied_text[i] == '\\'){
            memcpy(copied_text + i, copied_text + i + 1, copied_text_size - i - 1);
            copied_text_size--;
            i--;
        }
    }
    // set result
    result->is_normal_char = is_normal_char;
    result->text = copied_text;
    result->text_size = copied_text_size;
    return result;
}