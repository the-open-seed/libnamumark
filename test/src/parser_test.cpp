#define DEBUG
#include <gtest/gtest.h>
#include "parser.h"
TEST(ParserHeadlineTest, BasicAssertions) {
    char text[] = "= Hello, world!H1 =\n== Hello, world!H2 ==\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
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
    // free node
    ast_node_free(node);
}

TEST(ParserTextTest, BasicAssertions){
    char text[] = "Hello, world!\n'''Hello, world!'''Hello?\nBefore''AA''After\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
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
    ast_node_free(node);
}
TEST(ParserComplexTextTest, BasicAssertions){
    char text[] = "= H1 =\nHello, world!\n =='''Hello, world!'''Hello?\n== H2 ==\nBefore''AA''After\n";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
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
    ast_node_free(node);
}

TEST(ParserEscapeTest, BasicAssertions) {
    char text[] = "\\= H1 =\\d";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    EXPECT_EQ(node->children[0]->children_size, 0);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "= H1 =d");
    ast_node_free(node);
}

TEST(ParserWrongSyntaxTest, BasicAssertions){
    char text[] = "'''a\n= H1 \n wrong!'''";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "a\n= H1 \n wrong!");
    ast_node_free(node);
}

TEST(ParserWrongHeaderSyntaxTest, BasicAssertions){
    char text[] = "= H1 \n =\nwrong!";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "= H1 \n =\nwrong!");
    ast_node_free(node);
}

TEST(ParserDoubleQuoteTest, BasicAssertions){
    char text[] = "Hello, world!1\n> > Hello, world!2\n> Hello, world!3\nHello, world!4";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 3);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!1\n");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[1]->children_size, 2);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[1]->children[0]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!2");
    EXPECT_EQ(node->children[1]->children[1]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[1]->data;
    EXPECT_STREQ(data, "\nHello, world!3");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->data;
    EXPECT_STREQ(data, "Hello, world!4");
    ast_node_free(node);
}

TEST(ParserSequenceQuoteTest, BasicAssertions){
    char text[] = "Hello, world!1\n> '''Hello, world!2'''\n> '''Hello, world!3'''\nHello, world!4";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 3);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!1\n");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_BLOCKQUOTE);
    EXPECT_EQ(node->children[1]->children_size, 3);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[1]->children[0]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!2");
    EXPECT_EQ(node->children[1]->children[1]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[1]->data;
    EXPECT_STREQ(data, "\n");
    EXPECT_EQ(node->children[1]->children[2]->type, AST_NODE_TYPE_BOLD);
    EXPECT_EQ(node->children[1]->children[2]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[2]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[2]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!3");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->data;
    EXPECT_STREQ(data, "Hello, world!4");
    ast_node_free(node);
}

TEST(ParserColorTextTest, BasicAssertions){
    char text[] = "Hello, world!1\n{{{#ff0000 Hello, world!2}}}{{{#red Hello, world!3}}}\n{{{#aaa Hello, world!4}}}{{{#ggg Hello, world!5}}}";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 6);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!1\n");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_COLOR);
    EXPECT_EQ(node->children[1]->data_type, AST_DATA_TYPE_COLOR);
    EXPECT_EQ(((ast_data_color *)node->children[1]->data)->color_size, 7);
    EXPECT_STRCASEEQ(((ast_data_color *)node->children[1]->data)->color, "#ff0000");
    EXPECT_EQ(((ast_data_color *)node->children[1]->data)->dark_color_size, 0);
    EXPECT_EQ(node->children[1]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!2");
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_COLOR);
    EXPECT_EQ(node->children[2]->data_type, AST_DATA_TYPE_COLOR);
    EXPECT_EQ(((ast_data_color *)node->children[2]->data)->color_size, 4);
    EXPECT_STRCASEEQ(((ast_data_color *)node->children[2]->data)->color, "#red");
    EXPECT_EQ(((ast_data_color *)node->children[2]->data)->dark_color_size, 0);
    EXPECT_EQ(node->children[2]->children_size, 1);
    EXPECT_EQ(node->children[2]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!3");
    EXPECT_EQ(node->children[3]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[3]->data;
    EXPECT_STREQ(data, "\n");
    EXPECT_EQ(node->children[4]->type, AST_NODE_TYPE_COLOR);
    EXPECT_EQ(node->children[4]->data_type, AST_DATA_TYPE_COLOR);
    EXPECT_EQ(((ast_data_color *)node->children[4]->data)->color_size, 4);
    EXPECT_STRCASEEQ(((ast_data_color *)node->children[4]->data)->color, "#aaa");
    EXPECT_EQ(((ast_data_color *)node->children[4]->data)->dark_color_size, 0);
    EXPECT_EQ(node->children[4]->children_size, 1);
    EXPECT_EQ(node->children[4]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[4]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!4");
    EXPECT_EQ(node->children[5]->type, AST_NODE_TYPE_NO_WIKI);
    EXPECT_EQ(node->children[5]->children_size, 1);
    EXPECT_EQ(node->children[5]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[5]->children[0]->data;
    EXPECT_STREQ(data, "#ggg Hello, world!5");
    ast_node_free(node);
}

TEST(ParserDarkColorTextTest, BasicAssertions){
    char text[] = "{{{#ff0000,#blue Hello, world!2}}}";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 1);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_COLOR);
    EXPECT_EQ(node->children[0]->data_type, AST_DATA_TYPE_COLOR);
    EXPECT_EQ(((ast_data_color *)node->children[0]->data)->color_size, 7);
    EXPECT_STRCASEEQ(((ast_data_color *)node->children[0]->data)->color, "#ff0000");
    EXPECT_EQ(((ast_data_color *)node->children[0]->data)->dark_color_size, 4);
    EXPECT_STRCASEEQ(((ast_data_color *)node->children[0]->data)->dark_color, "#blue");
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "Hello, world!2");
    ast_node_free(node);
}
TEST(ParserLinkTest, BasicAssertions){
    char text[] = "[[example]][[나무마크]][[나무마크|여기가 내용]][[나무마크|{{{+3 여기안에서도 문법이 되겠죠?}}}]]";
    printf("text: %s\n", text);
    ast_node *node = parse(text, strlen(text));
#ifdef DEBUG
    ast_node_print(node);
#endif
    EXPECT_EQ(node->type, AST_NODE_TYPE_ROOT);
    EXPECT_EQ(node->children_size, 4);
    EXPECT_EQ(node->children[0]->type, AST_NODE_TYPE_LINK);
    EXPECT_EQ(node->children[0]->data_type, AST_DATA_TYPE_LINK);
    EXPECT_EQ(((ast_data_link *)node->children[0]->data)->link_size, 7);
    EXPECT_STRCASEEQ(((ast_data_link *)node->children[0]->data)->link, "example");
    EXPECT_EQ(node->children[0]->children_size, 1);
    EXPECT_EQ(node->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    char* data = (char*)node->children[0]->children[0]->data;
    EXPECT_STREQ(data, "example");
    EXPECT_EQ(node->children[1]->type, AST_NODE_TYPE_LINK);
    EXPECT_EQ(node->children[1]->data_type, AST_DATA_TYPE_LINK);
    EXPECT_EQ(((ast_data_link *)node->children[1]->data)->link_size, 12);
    EXPECT_STRCASEEQ(((ast_data_link *)node->children[1]->data)->link, "나무마크");
    EXPECT_EQ(node->children[1]->children_size, 1);
    EXPECT_EQ(node->children[1]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[1]->children[0]->data;
    EXPECT_STREQ(data, "나무마크");
    // [[link|text]]: link: 나무마크, text: 여기가 내용
    EXPECT_EQ(node->children[2]->type, AST_NODE_TYPE_LINK);
    EXPECT_EQ(node->children[2]->data_type, AST_DATA_TYPE_LINK);
    EXPECT_EQ(((ast_data_link *)node->children[2]->data)->link_size, 12);
    EXPECT_STRCASEEQ(((ast_data_link *)node->children[2]->data)->link, "나무마크");
    EXPECT_EQ(node->children[2]->children_size, 1);
    EXPECT_EQ(node->children[2]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[2]->children[0]->data;
    EXPECT_STREQ(data, "여기가 내용");
    // [[나무마크|{{{+3 여기안에서도 문법이 되겠죠?}}}]]
    EXPECT_EQ(node->children[3]->type, AST_NODE_TYPE_LINK);
    EXPECT_EQ(node->children[3]->data_type, AST_DATA_TYPE_LINK);
    EXPECT_EQ(((ast_data_link *)node->children[3]->data)->link_size, 12);
    EXPECT_STRCASEEQ(((ast_data_link *)node->children[3]->data)->link, "나무마크");
    EXPECT_EQ(node->children[3]->children_size, 1);
    EXPECT_EQ(node->children[3]->children[0]->type, AST_NODE_TYPE_BIG_TEXT_3);
    EXPECT_EQ(node->children[3]->children[0]->children_size, 1);
    EXPECT_EQ(node->children[3]->children[0]->children[0]->type, AST_NODE_TYPE_TEXT);
    data = (char*)node->children[3]->children[0]->children[0]->data;
    EXPECT_STREQ(data, "여기안에서도 문법이 되겠죠?");
    ast_node_free(node);
}