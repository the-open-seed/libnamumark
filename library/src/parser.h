//
// Created by hibot on 23/08/24.
//

#ifndef LIBNAMUMARK_PARSER_H
#ifdef __cplusplus
extern "C" {
#endif
#define LIBNAMUMARK_PARSER_H

#include "ast.h"
#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

ast_node *parse(const char *text, const size_t text_size);

#ifdef __cplusplus
}
#endif
#endif //LIBNAMUMARK_PARSER_H