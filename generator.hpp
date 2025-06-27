#pragma once

#include "structs.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <unordered_map>
#include <vector>

class Generator {
    private:
        struct Var {
            std::string type;
            size_t stack_loc;
        };

        struct Scope {
            size_t begin;
            std::unordered_map<std::string, Var> variables;
        };

        const Node::Prog root_node;
        std::stringstream output;
        size_t stack_size = 0;
        std::vector<Scope> scopes;
        int label_count = 0;

        void push(const std::string& reg){
            output << "push " << reg << "\n";
            stack_size++;
        }

        void pop(const std::string& reg){
            output << "pop " << reg << "\n";
            stack_size--;
        }

        struct ExprVisitor {
            Generator& gen;

            void operator()(const Node::IntLit* int_lit){
                gen.output << "mov rax, " << int_lit->val.val << "\n";
                gen.push("rax");
            };

            void operator()(const Node::BoolLit* bool_lit){
                if (bool_lit->bool_lit.val == "true"){
                    gen.output << "mov rax, 1\n";
                    gen.push("rax");
                } else if (bool_lit->bool_lit.val == "false"){
                    gen.output << "mov rax, 0\n";
                    gen.push("rax");
                } else {
                    std::cerr << "Unknown bool value: " << bool_lit->bool_lit.val << "\n";
                    exit(-1);
                }
            }
            
            void operator()(const Node::Ident* ident){
                bool not_found = true;
                for (int i = gen.scopes.size()-1; i >= 0; i--){
                    const Scope scope = gen.scopes.at(i);
                    if (scope.variables.find(ident->ident.val) != scope.variables.end()){
                        not_found = false;
                        const Var& var = scope.variables.at(ident->ident.val);
                        std::stringstream s;
                        s << "QWORD [rsp + " << (gen.stack_size - var.stack_loc - 1)*8 << "]\n";
                        gen.push(s.str());
                        break;
                    };
                };

                if (not_found){
                    std::cerr << "Use of undeclared variable: " << ident->ident.val << "\n";
                    exit(-1);
                };
            };

            void operator()(const Node::BinExpr* bin_expr){
                gen.gen_expr(bin_expr->Left);
                gen.gen_expr(bin_expr->Right);
                if (bin_expr->operation.val == "+"){
                    gen.pop("rdi");
                    gen.pop("rax");
                    gen.output << "add rax, rdi\n";
                    gen.push("rax");
                } else if (bin_expr->operation.val == "-"){
                    gen.pop("rdi");
                    gen.pop("rax");
                    gen.output << "sub rax, rdi\n";
                    gen.push("rax");
                } else if (bin_expr->operation.val == "*"){
                    gen.pop("rdi");
                    gen.pop("rax");
                    gen.output << "mul rdi\n";
                    gen.push("rax");
                } else if (bin_expr->operation.val == "/"){
                    gen.pop("rdi");
                    gen.pop("rax");
                    gen.output << "div rdi\n";
                    gen.push("rax");
                } else {
                    std::cerr << "Unknown binary operator";
                    exit(-1);
                }
            }
        };

        struct StmntVisitor {
            Generator& gen;
            void operator()(const Node::StmntExit* exit_stmnt){
                gen.gen_expr(exit_stmnt->expr);
                gen.output << "mov rax, 60\n";
                gen.pop("rdi");
                gen.output << "syscall\n";
            };

            void operator()(const Node::DeclareIdent* ident_declare){
                if (gen.scopes.back().variables.find(ident_declare->ident->ident.val) == gen.scopes.back().variables.end()){
                    gen.scopes.back().variables.insert({ident_declare->ident->ident.val, Var {.type = ident_declare->type->type.val, .stack_loc = gen.stack_size}});
                    gen.gen_expr(ident_declare->expr);
                } else { std::cerr << "Redeclaration of variable: " << ident_declare->ident->ident.val << "\n";
                    exit(-1);
                };
            };
            
            void operator()(const Node::Scope* scope){
                gen.gen_scope(scope);
            };

            void operator()(const Node::IfElse* if_block){
                gen.gen_expr(if_block->condition);
                gen.pop("rax");
                std::string end_if = "label" + std::to_string(gen.label_count++);
                std::string end_else = "label" + std::to_string(gen.label_count++);

                gen.output << "test rax, rax\n";
                gen.output << "jz " << end_if << "\n";
                gen.gen_scope(if_block->if_block);

                if (if_block->else_block.has_value()){
                    gen.output << "jmp " << end_else << "\n";
                }

                gen.output << end_if << ":\n";

                if (if_block->else_block.has_value()){
                    gen.gen_scope(if_block->else_block.value());
                    gen.output << end_else << ":\n";
                }
            };
        };

        void gen_expr(const Node::Expr* expr){
            ExprVisitor visitor = {.gen = *this};
            std::visit(visitor, expr->var);
        };

        void gen_stmnt(const Node::Stmnt* stmnt){
            StmntVisitor visitor = {.gen = *this};
            std::visit(visitor, stmnt->var);
        };

        void gen_scope(const Node::Scope* scope){
            scopes.push_back({.begin = stack_size});
            for (const Node::Stmnt* stmnt : scope->stmnts){
                gen_stmnt(stmnt);
            };

            // while (stack_size != scopes.back().begin){
            //     pop("rax");
            // };
            output << "add rsp, " << (stack_size - scopes.back().begin)*8 << "\n"; 
            stack_size = scopes.back().begin;

            scopes.pop_back();
        };

    public:
        Generator(const Node::Prog& root) : root_node(root){
        };

        std::string generate(){
            output << "global _start\n";
            output << "_start:\n";

            for (const Node::Scope* scope : root_node.funcs){
                gen_scope(scope);
            };

            output << "mov rax, 60\n";
            output << "mov rdi, 0\n";
            output << "syscall\n";

            return output.str();
        };
};
