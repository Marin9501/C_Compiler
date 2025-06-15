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
                        tokens.push_back({.type = TokenType::_type_dec, .val = buf});
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
                } else if (data.at(index) == '='){
                    tokens.push_back({.type = TokenType::_eq});
                    buf.clear();
                    index++;
                } else if (data.at(index) == '+'){
                    tokens.push_back({.type = TokenType::_bin_op, .val = "+"});
                    buf.clear();
                    index++;
                } else if (data.at(index) == '*'){
                    tokens.push_back({.type = TokenType::_bin_op, .val = "*"});
                    buf.clear();
                    index++;
                } else if (data.at(index) == '('){
                    tokens.push_back({.type = TokenType::_open_par});
                    buf.clear();
                    index++;
                } else if (data.at(index) == ')'){
                    tokens.push_back({.type = TokenType::_close_par});
                    buf.clear();
                    index++;
                } else if (data.at(index) == '{'){
                    tokens.push_back({.type = TokenType::_open_curl});
                    buf.clear();
                    index++;
                } else if (data.at(index) == '}'){
                    tokens.push_back({.type = TokenType::_close_curl});
                    buf.clear();
                    index++;
                } else if (data.at(index) == ';'){
                    tokens.push_back({.type = TokenType::_semi_col});
                    buf.clear();
                    index++;
                } else if (std::iswspace(data.at(index))){
                    index++;
                    continue;
                } else {
                    std::cerr << "Something went wrong while tokenizing\n" << buf;
                    exit(-1);
                };
            };
            index = 0;
            return tokens;
        };
};
