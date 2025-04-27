#include <cctype>
#include <cstdlib>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "./tokenizer.hpp"
#include "generator.hpp"
#include "parser.hpp"
#include "structs.hpp"

int main(int argc, char* argv[]){
    if (argc != 2){
        std::cerr << "Incorrect usage. \n";
    };
    std::string content;
    std::ifstream file(argv[1]);
    if (file.is_open()){
        std::stringstream buf;
        buf << file.rdbuf();
        content = buf.str();
        file.close();
    } else {
        std::cerr << "Could open file\n";
        exit(-1);
    };

    Tokenizer tokenizer(content);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    Node::Prog root = parser.parse();

    Generator generator(root);
    std::ofstream out_file("out.asm");
    if (out_file.is_open()){
        out_file << generator.generate();
        out_file.close();
    } else {
        std::cerr << "Could write to file\n";
        exit(-1);
    };

    system("cat out.asm");
    system("nasm -felf64 out.asm -o out.o");
    system("ld out.o -o out");

return 0;
};


