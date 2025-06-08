#pragma once

#include "structs.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <unordered_map>

class Generator {
    private:
        const Node::Prog root_node;
        std::stringstream output;

        struct Var {
            std::string type;
            std::string val;
        };

        std::unordered_map<std::string, Var> variables;

        struct Visitor {
            Generator* gen;
            //Expressions
            std::string operator()(const Node::IntLit* int_lit){
                return int_lit->val.val;
            };
            std::string operator()(const Node::Ident* ident){
                if (gen->variables.find(ident->ident.val) != gen->variables.end()){
                    return gen->variables[ident->ident.val].val;
                } else {
                    std::cerr << "Use of undeclared variable: " << ident->ident.val << "\n";
                    exit(-1);
                };
            };

            std::string operator()(const Node::BinExpr* bin_expr){
                return " ";
            }
            //Statements
            std::string operator()(const Node::StmntExit* exit_stmnt){
                std::stringstream exit_stream;
                exit_stream << "mov rax, 60\n";
                exit_stream  << "mov rdi, " << gen->gen_expr(exit_stmnt->expr) << "\n";
                exit_stream  << "syscall\n";
                return exit_stream.str();
            };
            std::string operator()(const Node::DeclareIdent* ident_declare){
                if (gen->variables.find(ident_declare->ident->ident.val) != gen->variables.end()){
                    std::cerr << "Redeclaration of variable: " << ident_declare->ident->ident.val << "\n";
                    exit(-1);
                } else {
                    gen->variables.insert({ident_declare->ident->ident.val, Generator::Var({.type = ident_declare->type->type.val, .val = gen->gen_expr(ident_declare->expr)})});
                };
                return "";
            };
        };

        Visitor visitor = {.gen = this};

        std::string gen_expr(const Node::Expr* expr){
            return std::visit(visitor, expr->var);
        };

        std::string gen_stmnt(const Node::Stmnt* stmnt){
            return std::visit(visitor, stmnt->var);
        };

    public:
        Generator(const Node::Prog& root) : root_node(root){
        };

        std::string generate(){
            output << "global _start\n";
            output << "_start:\n";

            for (const Node::Stmnt* statement : root_node.funcs){
                output << gen_stmnt(statement);
            };

            output << "mov rax, 60\n";
            output << "mov rdi, 0\n";
            output << "syscall\n";

            return output.str();
        };
};
