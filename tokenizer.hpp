#pragma once

#include <cctype>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#include <string>
#include "./structs.hpp"

class Tokenizer{
    private:
        int index = 0;
        const std::string data;
        const int data_len;
        int open_curl = 0;
        int close_curl = 0;
        int open_par = 0;
        int close_par = 0;

    public:
        Tokenizer(const std::string &content) : data(content), data_len(data.length()) {
        };

        std::vector<Token> tokenize(){
            std::string buf;
            std::vector<Token> tokens;

            while (index < data_len){
                if (std::isalpha(data.at(index)) || data.at(index) == '_'){
                    while (index < data_len && std::isalnum(data.at(index)) || data.at(index) == '_'){
                        buf.push_back(data.at(index));
                        index++;
                    }

                    if (buf == "exit"){
                        tokens.push_back({.type = TokenType::_exit});
                        buf.clear();
                    } else if (buf == "int"){
                        tokens.push_back({.type = TokenType::_type_dec, .val = "int"});
                        buf.clear();
                    } else if (buf == "bool"){
                        tokens.push_back({.type = TokenType::_type_dec, .val = "bool"});
                        buf.clear();
                    } else if (buf == "true"){
                        tokens.push_back({.type = TokenType::_bool_lit, .val = "true"});
                        buf.clear();
                    } else if (buf == "false"){
                        tokens.push_back({.type = TokenType::_bool_lit, .val = "false"});
                        buf.clear();
                    } else if (buf == "if"){
                        tokens.push_back({.type = TokenType::_if});
                        buf.clear();
                    } else if (buf == "else"){
                        tokens.push_back({.type = TokenType::_else});
                        buf.clear();
                    } else {
                        tokens.push_back({.type = TokenType::_ident, .val = buf});
                        buf.clear();
                    };
                } else if (std::isdigit(data.at(index))){
                    while (index < data_len && std::isdigit(data.at(index))){
                        buf.push_back(data.at(index));
                        index++;
                    }

                    tokens.push_back({.type = TokenType::_int_lit, .val = buf});
                    buf.clear();
                } else if (!std::iswspace(data.at(index))){
                    //Single line comment
                    if (data.at(index) == '/' && data.at(index+1) == '/'){
                        while (data.at(index) != '\n'){
                            index++;
                        };
                        continue;
                    };

                    //Multi line comment
                    if (data.at(index) == '/' && data.at(index+1) == '*'){
                        while (!(data.at(index) == '*' && data.at(index+1) == '/')){
                            if (index == data_len-1){
                                std::cerr << "Unclosed comment\n";
                                exit(-1);
                            }
                            index++;
                        }
                        //Skip '*'
                        index++;
                        //Skip '/'
                        index++;
                        continue;
                    }

                    switch (data.at(index)){
                        break;
                        case '=':
                            tokens.push_back({.type = TokenType::_eq});
                            buf.clear();
                            index++;
                            break;
                        case '+':
                            tokens.push_back({.type = TokenType::_bin_op, .val = "+"});
                            buf.clear();
                            index++;
                            break;
                        case '-':
                            tokens.push_back({.type = TokenType::_bin_op, .val = "-"});
                            buf.clear();
                            index++;
                            break;
                        case '*':
                            tokens.push_back({.type = TokenType::_bin_op, .val = "*"});
                            buf.clear();
                            index++;
                            break;
                        case '/':
                            tokens.push_back({.type = TokenType::_bin_op, .val = "/"});
                            buf.clear();
                            index++;
                            break;
                        case '(':
                            tokens.push_back({.type = TokenType::_open_par});
                            buf.clear();
                            index++;
                            open_par++;
                            break;
                        case ')':
                            tokens.push_back({.type = TokenType::_close_par});
                            buf.clear();
                            index++;
                            close_par++;
                            break;
                        case '{':
                            tokens.push_back({.type = TokenType::_open_curl});
                            buf.clear();
                            index++;
                            open_curl++;
                            break;
                        case '}':
                            tokens.push_back({.type = TokenType::_close_curl});
                            buf.clear();
                            index++;
                            close_curl++;
                            break;
                        case ';':
                            tokens.push_back({.type = TokenType::_semi_col});
                            buf.clear();
                            index++;
                            break;
                        default:
                            std::cerr << "Something went wrong while tokenizing\n" << buf;
                            exit(-1);
                    }

                } else {
                    index++;
                    continue;
                };
            };
            index = 0;
            if (open_curl != close_curl || open_par != close_par){
                std::cerr << "Unmatched parethesies\n";
                exit(-1);
            }
            return tokens;
        };
};
