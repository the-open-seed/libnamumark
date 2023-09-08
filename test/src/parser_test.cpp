#define DEBUG
#include <gtest/gtest.h>
#include "parser.h"
TEST(ParserHeadlineTest, BasicAssertions) {
    char text[] = "= Hello, world!H1 =\n== Hello, world!H2 ==\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 2);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_H1);
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    // check data
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!H1");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_H2);
    EXPECT_EQ(node->children[1]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_TEXT);
    // check data
    data = (char*)node->children[1]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!H2");
#ifdef DEBUG
    ast_node_print(node);
#endif
    // free node
    ast_node_free(node);
}

TEST(ParserTextTest, BasicAssertions){
    char text[] = "Hello, world!\n'''Hello, world!'''Hello?\nBefore''AA''After\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 5);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!\n");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[1]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->data;
    EXPECT_STREQ(data, "Hello?\nBefore");
    EXPECT_EQ(node->children[3]->type, AST_NODE_TYPE_ITALIC);
    EXPECT_EQ(node->children[3]->children_size, 1);
    EXPECT_EQ(node->children[3]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[3]->children[0]->data;
    EXPECT_STREQ(data, "AA");
    EXPECT_EQ(node->children[4]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[4]->data;
    EXPECT_STREQ(data, "After\n");
#ifdef DEBUG
    ast_node_print(node);
#endif
}
TEST(ParserComplexTextTest, BasicAssertions){
    char text[] = "= H1 =\nHello, world!\n =='''Hello, world!'''Hello?\n== H2 ==\nBefore''AA''After\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 8);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_H1);
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "H1");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->data;
    EXPECT_STREQ(data, "Hello, world!\n ==");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[2]->children_size, 1);
    EXPECT_EQ(node->children[2]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!");
    EXPECT_EQ(node->children[3]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[3]->data;
    EXPECT_STREQ(data, "Hello?\n");
    EXPECT_EQ(node->children[4]->type, AST_NODE_TYPE_H2);
    EXPECT_EQ(node->children[4]->children_size, 1);
    EXPECT_EQ(node->children[4]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[4]->children[0]->data;
    EXPECT_STREQ(data, "H2");
    EXPECT_EQ(node->children[5]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[5]->data;
    EXPECT_STREQ(data, "Before");
    EXPECT_EQ(node->children[6]->type, AST_NODE_TYPE_ITALIC);
    EXPECT_EQ(node->children[6]->children_size, 1);
    EXPECT_EQ(node->children[6]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[6]->children[0]->data;
    EXPECT_STREQ(data, "AA");
    EXPECT_EQ(node->children[7]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[7]->data;
    EXPECT_STREQ(data, "After\n");
#ifdef DEBUG
    ast_node_print(node);
#endif
}

TEST(ParserEscapeTest, BasicAssertions) {
    char text[] = "\\= H1 =\\d";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    printf("node: %p\n", node);
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    EXPECT_EQ(node->children[0]->children_size, 0);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "= H1 =d");
#ifdef DEBUG
    ast_node_print(node);
#endif
}

TEST(ParserWrongSyntaxTest, BasicAssertions){
    char text[] = "'''a\n= H1 \n wrong!'''";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "a\n= H1 \n wrong!");
#ifdef DEBUG
    ast_node_print(node);
#endif
}

TEST(ParserWrongHeaderSyntaxTest, BasicAssertions){
    char text[] = "= H1 \n =\nwrong!";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "= H1 \n =\nwrong!");
#ifdef DEBUG
    ast_node_print(node);
#endif
}

TEST(ParserQuoteTest, BasicAssertions){
    char text[] = "Hello, world!1\n> > Hello, world!2\n> Hello, world!3\nHello, world!4";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 4);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!1\n");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[1]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[1]->children[0]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!2");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[2]->children_size, 1);
    EXPECT_EQ(node->children[2]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!3");
    EXPECT_EQ(node->children[3]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[3]->data;
    EXPECT_STREQ(data, "Hello, world!4");
#ifdef DEBUG
    ast_node_print(node);
#endif
}