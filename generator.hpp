#pragma once

#include "structs.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <unordered_map>

class Generator {
    private:
        struct Var {
            std::string type;
            size_t stack_loc;
        };

        const Node::Prog root_node;
        std::stringstream output;
        size_t stack_size = 0;
        std::unordered_map<std::string, Var> variables;

        void push(const std::string& reg){
            output << "push " << reg << "\n";
            stack_size++;
        }

        void pop(const std::string& reg){
            output << "pop " << reg << "\n";
            stack_size--;
        }

        struct ExprVisitor {
            Generator* gen;
            //Expressions
            void operator()(const Node::IntLit* int_lit){
                gen->output << "mov rax, " << int_lit->val.val << "\n";
                gen->push("rax");
            };
            void operator()(const Node::Ident* ident){
                if (gen->variables.find(ident->ident.val) != gen->variables.end()){
                    // gen->output << "mov rax, " << gen->variables[ident->ident.val].val << "\n";
                    const Var& var = gen->variables[ident->ident.val];
                    std::stringstream s;
                    s << "QWORD [rsp + " << (gen->stack_size - var.stack_loc - 1)*8 << "]\n";
                    gen->push(s.str());
                } else {
                    std::cerr << "Use of undeclared variable: " << ident->ident.val << "\n";
                    exit(-1);
                };
            };

            //TODO!!!
            void operator()(const Node::BinExpr* bin_expr){
                gen->gen_expr(bin_expr->Left);
                gen->gen_expr(bin_expr->Right);
                if (bin_expr->operation.val == "+"){
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->output << "add rax, rdi\n";
                    gen->push("rax");
                } else if (bin_expr->operation.val == "*"){
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->output << "mul rdi\n";
                    gen->push("rax");

                } else {
                    std::cerr << "Unknown binary operator";
                    exit(-1);
                }
            }
        };
        struct StmntVisitor {
            Generator* gen;
            //Statements
            void operator()(const Node::StmntExit* exit_stmnt){
                gen->gen_expr(exit_stmnt->expr);
                gen->output << "mov rax, 60\n";
                gen->pop("rdi");
                gen->output << "syscall\n";
            };
            void operator()(const Node::DeclareIdent* ident_declare){
                if (gen->variables.find(ident_declare->ident->ident.val) == gen->variables.end()){
                    // gen->variables.insert({ident_declare->ident->ident.val, Generator::Var({.type = ident_declare->type->type.val, .val = gen->gen_expr(ident_declare->expr)})});
                    gen->variables.insert({ident_declare->ident->ident.val, Var {.type = ident_declare->type->type.val, .stack_loc = gen->stack_size}});
                    gen->gen_expr(ident_declare->expr);
                } else {
                    std::cerr << "Redeclaration of variable: " << ident_declare->ident->ident.val << "\n";
                    exit(-1);
                };
            };
        };

        void gen_expr(const Node::Expr* expr){
            ExprVisitor visitor = {.gen = this};
            std::visit(visitor, expr->var);
        };

        void gen_stmnt(const Node::Stmnt* stmnt){
            StmntVisitor visitor = {.gen = this};
            std::visit(visitor, stmnt->var);
        };

    public:
        Generator(const Node::Prog& root) : root_node(root){
        };

        std::string generate(){
            output << "global _start\n";
            output << "_start:\n";

            for (const Node::Stmnt* statement : root_node.funcs){
                gen_stmnt(statement);
            };

            output << "mov rax, 60\n";
            output << "mov rdi, 0\n";
            output << "syscall\n";

            return output.str();
        };
};
