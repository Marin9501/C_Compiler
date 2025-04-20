#include <cctype>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "./tokenizer.hpp"
std::string toStr(TokenType t){
    switch(t){
        case TokenType::_exit: 
            return "exit";
        case TokenType::_int_lit:
            return "int_lit";
        case TokenType::_semi_col:
            return "semi col";
    };
    return "";
};

int main(int argc, char* argv[]){
    if (argc != 2){
        std::cerr << "Incorrect usage. \n";
    };

    std::ifstream file(argv[1]);
    std::stringstream buf;
    buf << file.rdbuf();
    std::string content = buf.str();

    Tokenizer tokenizer(content);
    std::vector<Token> tokens = tokenizer.tokenize();
    // for(auto t : tokens){
    //     std::cout << toStr(t.type) << " " << t.val << "\n";
    // };

    return 0;
};


