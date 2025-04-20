#pragma once

#include <cctype>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum class TokenType {
    _exit,
    _int_lit,
    _semi_col,
};

struct Token {
    TokenType type;
    std::string val = "";
};

class Tokenizer{
    private:
        int index = 0;
        const std::string data;
        const int data_len;

        // std::optional<char> look_next(int count = 1) const {
        //     return index + count >= data.length() ?  : data.at(index);
        // };
        //
        // char get_next(){
        //     return data.at(index++);
        // };

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
                    } else {
                        std::cerr << "Unrecognized expression: " << buf << "\n";
                        exit(-1);
                    };
                } else if (std::isdigit(data.at(index))){
                    while (index < data_len && std::isdigit(data.at(index))){
                        buf.push_back(data.at(index));
                        index++;
                    }

                    if (std::iswspace(data.at(index)) || data.at(index) == ';'){
                        tokens.push_back({.type = TokenType::_int_lit, .val = buf});
                        buf.clear();
                    } else {
                        std::cerr << "Unrecognized expression: " << buf << "\n";
                        exit(-1);
                    };
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

            // while(look_next().has_value()){
            //     if(std::isalpha(look_next().value())){
            //         while(look_next().has_value() && std::isalnum(look_next().value())){
            //             buf.push_back(get_next());
            //         };
            //
            //         if(buf == "exit"){
            //             tokens.push_back({.type = TokenType::_exit});
            //             buf.clear();
            //         } else {
            //             std::cerr << "Unrecognized instruction: " << buf << "\n";
            //             exit(-1);
            //         };
            //     } else if(std::isdigit(look_next().value())){
            //         while(std::isdigit(look_next().value())){
            //             buf.push_back(get_next());
            //         };
            //         tokens.push_back({.type = TokenType::_int_lit, .val = buf});
            //         buf.clear();
            //     } else if(look_next().value() == ';'){
            //         tokens.push_back({.type = TokenType::_semi_col});
            //         exit(-1);
            //     } else {
            //         get_next();
            //         continue;
            //     };
            // };
