#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "include/structs.hpp"
#include "include/tokenizer.hpp"

std::vector<Token> tokenize_helper(std::string input){
    Tokenizer tokenizer = Tokenizer(input);
    return tokenizer.tokenize();
};

TEST(Tokenizer, EmptyInput){
    std::vector<Token> t = tokenize_helper("");
    EXPECT_EQ(t.size(), 0);
    t = tokenize_helper("     ");
    EXPECT_EQ(t.size(), 0);
    t = tokenize_helper("\n\n\n\n");
    EXPECT_EQ(t.size(), 0);
    t = tokenize_helper("  \n  \n  \n\n");
    EXPECT_EQ(t.size(), 0);
}

TEST(Tokenizer, KeyWords){
    std::vector<Token> t = tokenize_helper("exit\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_exit);
    EXPECT_EQ(t.at(0).val, "");

    t = tokenize_helper("int\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_type_dec);
    EXPECT_EQ(t.at(0).val, "int");

    t = tokenize_helper("bool\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_type_dec);
    EXPECT_EQ(t.at(0).val, "bool");

    t = tokenize_helper("if\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_if);
    EXPECT_EQ(t.at(0).val, "");

    t = tokenize_helper("else\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_else);
    EXPECT_EQ(t.at(0).val, "");

    t = tokenize_helper("for\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_for);
    EXPECT_EQ(t.at(0).val, "");
}

TEST(Tokenizer, MultipleKeywords){
    std::vector<Token> t = tokenize_helper("exit\nint bool \n true false if \nelse\n");
    EXPECT_EQ(t.size(), 7);

    EXPECT_EQ(t.at(0).type, TokenType::_exit);
    EXPECT_EQ(t.at(0).val, "");

    EXPECT_EQ(t.at(1).type, TokenType::_type_dec);
    EXPECT_EQ(t.at(1).val, "int");

    EXPECT_EQ(t.at(2).type, TokenType::_type_dec);
    EXPECT_EQ(t.at(2).val, "bool");

    EXPECT_EQ(t.at(3).type, TokenType::_bool_lit);
    EXPECT_EQ(t.at(3).val, "true");

    EXPECT_EQ(t.at(4).type, TokenType::_bool_lit);
    EXPECT_EQ(t.at(4).val, "false");

    EXPECT_EQ(t.at(5).type, TokenType::_if);
    EXPECT_EQ(t.at(5).val, "");

    EXPECT_EQ(t.at(6).type, TokenType::_else);
    EXPECT_EQ(t.at(6).val, "");
};

TEST (Tokenizer, ValidVariableNames){
    std::vector<Token> t = tokenize_helper("_exit_\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_ident);
    EXPECT_EQ(t.at(0).val, "_exit_");

    t = tokenize_helper("a\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_ident);
    EXPECT_EQ(t.at(0).val, "a");

    t = tokenize_helper("a1\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_ident);
    EXPECT_EQ(t.at(0).val, "a1");

    t = tokenize_helper("aB\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_ident);
    EXPECT_EQ(t.at(0).val, "aB");
};

TEST (Tokenizer, InvalidVariableNames){
    std::vector<Token> t;
    EXPECT_DEATH(t = tokenize_helper("1a\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("a!\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("a?\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("a#\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("_123\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("_Var\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("__var\n"), ".*");
    EXPECT_DEATH(t = tokenize_helper("_A1\n"), ".*");
};

TEST (Tokenizer, LiteralValues){
    std::vector<Token> t = tokenize_helper("1\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_int_lit);
    EXPECT_EQ(t.at(0).val, "1");

    t = tokenize_helper("124090\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_int_lit);
    EXPECT_EQ(t.at(0).val, "124090");

    t = tokenize_helper("true\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_bool_lit);
    EXPECT_EQ(t.at(0).val, "true");

    t = tokenize_helper("false\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_bool_lit);
    EXPECT_EQ(t.at(0).val, "false");

    EXPECT_DEATH(tokenize_helper("12_4090\n"), ".*");
    EXPECT_DEATH(tokenize_helper("124090_\n"), ".*");
};

TEST (Tokenizer, Comments){
    std::vector<Token> t = tokenize_helper("//exit\n var\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_ident);
    EXPECT_EQ(t.at(0).val, "var");

    t = tokenize_helper("//\n");
    EXPECT_EQ(t.size(), 0);

    t = tokenize_helper("////\n");
    EXPECT_EQ(t.size(), 0);

    t = tokenize_helper("//bbbbb int \n//aaaa\n");
    EXPECT_EQ(t.size(), 0);

    t = tokenize_helper("/* */");
    EXPECT_EQ(t.size(), 0);

    t = tokenize_helper("/* true\n bool \n var */\n");
    EXPECT_EQ(t.size(), 0);

    t = tokenize_helper("/* true\n bool \n var */ exit\n");
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.at(0).type, TokenType::_exit);
    EXPECT_EQ(t.at(0).val, "");
}

TEST (Tokenizer, SpecialChars){
    std::vector<Token> t = tokenize_helper("=+-*/(){};");

    EXPECT_EQ(t.at(0).type, TokenType::_eq);
    EXPECT_EQ(t.at(0).val, "");

    EXPECT_EQ(t.at(1).type, TokenType::_bin_op);
    EXPECT_EQ(t.at(1).val, "+");

    EXPECT_EQ(t.at(2).type, TokenType::_bin_op);
    EXPECT_EQ(t.at(2).val, "-");

    EXPECT_EQ(t.at(3).type, TokenType::_bin_op);
    EXPECT_EQ(t.at(3).val, "*");

    EXPECT_EQ(t.at(4).type, TokenType::_bin_op);
    EXPECT_EQ(t.at(4).val, "/");

    EXPECT_EQ(t.at(5).type, TokenType::_open_par);
    EXPECT_EQ(t.at(5).val, "");

    EXPECT_EQ(t.at(6).type, TokenType::_close_par);
    EXPECT_EQ(t.at(6).val, "");

    EXPECT_EQ(t.at(7).type, TokenType::_open_curl);
    EXPECT_EQ(t.at(7).val, "");

    EXPECT_EQ(t.at(8).type, TokenType::_close_curl);
    EXPECT_EQ(t.at(8).val, "");

    EXPECT_EQ(t.at(9).type, TokenType::_semi_col);
    EXPECT_EQ(t.at(9).val, "");
}

TEST (Tokenizer, ParenCount){
    std::vector<Token> t = tokenize_helper("()");
    EXPECT_EQ(t.size(), 2);

    t = tokenize_helper("({}){(})");// Fix this
    EXPECT_EQ(t.size(), 8);

    EXPECT_DEATH(tokenize_helper("("), ".*");
    EXPECT_DEATH(tokenize_helper("({"), ".*");
    EXPECT_DEATH(tokenize_helper("({}"), ".*");
    EXPECT_DEATH(tokenize_helper("({)"), ".*");
};

// TEST (Parser, ){
// }
