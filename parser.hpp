#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "./structs.hpp"
#include "./allocator.hpp"

class Parser{
    private:
        const std::vector<Token> tokens;
        const int tokens_len;
        int index = 0;
        ArenaAllocator allocator;
        enum class prec {
        };

        std::optional<Token> look(int ahead = 0){
            if (index + ahead < tokens_len){
                return tokens.at(index + ahead);
            } else {
                return {};
            };
        };
        std::optional<Token> mov_next(){
            if (index < tokens_len){
                return tokens.at(index++);
            } else {
                return {};
            };
        };

        Node::BinExpr* parse_bin_expr(){
            Node::IntLit* int_lit = allocator.alloc<Node::IntLit>();
            int_lit->val = mov_next().value();
            Node::Expr* expr = allocator.alloc<Node::Expr>();
            expr->var = int_lit;

            Node::BinExpr* bin_expr = allocator.alloc<Node::BinExpr>();
            bin_expr->Left = expr;
            bin_expr->operation = mov_next().value();
            bin_expr->Right = parse_expr();
            return bin_expr;
        };

        Node::Expr* parse_expr(){
            Node::Expr* expr = allocator.alloc<Node::Expr>();
            if (look(1).has_value() && look(1).value().type == TokenType::_bin_op){
                expr->var = parse_bin_expr();
                return expr;
            } else if (look().has_value() && look().value().type == TokenType::_int_lit){
                Node::IntLit* int_lit = allocator.alloc<Node::IntLit>();
                int_lit->val = mov_next().value();
                
                expr->var = int_lit;
                return expr;
                //return Node::Expr({.var = Node::IntLit({.val = mov_next().value()})});
            } else if (look().has_value() && look().value().type == TokenType::_ident){
                Node::Ident* ident = allocator.alloc<Node::Ident>();
                ident->ident = mov_next().value();

                expr->var = ident;
                return expr;
                //return Node::Expr({.var = Node::Ident({.ident = mov_next().value()})});
            } else {
                std::cerr << "Something went wrong when parsing expression";
                exit(-1);
            };
        };
        
        Node::Stmnt* parse_stmnt(){
            Node::Stmnt* stmnt_node = allocator.alloc<Node::Stmnt>();
            if (look().has_value() && look().value().type == TokenType::_exit){
                if (look(1).has_value() && look(1).value().type == TokenType::_open_par){
                    mov_next();
                    mov_next();
                    Node::StmntExit* stmnt_exit = allocator.alloc<Node::StmntExit>();
                    stmnt_exit->expr = parse_expr();
                    stmnt_node->var = stmnt_exit;
                    if (look().has_value() && look().value().type == TokenType::_close_par){
                        mov_next();
                    } else {
                        std::cerr << "Expected ')' when doing exit\n";
                        exit(-1);
                    }
                } else {
                    std::cerr << "Expected '(' when doing exit\n";
                    exit(-1);
                };
            } else if (look().has_value() && look().value().type == TokenType::_type_dec && 
                    look(1).has_value() && look(1).value().type == TokenType::_ident &&  
                    look(2).has_value() && look(2).value().type == TokenType::_eq){
                Node::Type* type = allocator.alloc<Node::Type>();
                type->type = mov_next().value();
                Node::Ident* ident = allocator.alloc<Node::Ident>();
                ident->ident = mov_next().value();
                mov_next(); //skip `=`
                Node::DeclareIdent* declare = allocator.alloc<Node::DeclareIdent>();
                declare->type = type;
                declare->ident = ident;
                declare->expr = parse_expr();
                stmnt_node->var = declare;
                //stmnt_node.var = Node::DeclareIdent({.type = type, .ident = ident, .expr = parse_expr()});
            } else {
                std::cerr << "Something went worng when parsing statment\n";
                exit(-1);
            };

            if (look().has_value() && look().value().type == TokenType::_semi_col){
                mov_next();
            } else {
                std::cerr << "Expected ; at the end of a line\n";
                exit(-1);
            }
            return stmnt_node;
        };

        //NO PARAMETERS OR PARENTHESIES AT FUNCTION DECLARATION
        // Node::Fun parse_func(){
        //     Node::Fun fun_node;
        //     if (tokens.at(index+1).type == TokenType::_open_curl){
        //         while (tokens.at(index).type != TokenType::_close_curl){
        //             if (index >= tokens_len){
        //                 std::cerr << "Expected } at the end of a function block\n";
        //             };
        //             fun_node.statements.push_back(parse_stmnt());
        //             index++;
        //         };
        //     } else {
        //         std::cerr << "Expected { after function declaration\n";
        //         exit(-1);
        //     };
        //     return fun_node;
        // };
    public:
        Parser(const std::vector<Token>& tokens) : tokens(tokens), tokens_len(tokens.size()), allocator(1024*1024 * 5){
        };

        Node::Prog parse(){
            Node::Prog root_node;
            while (look().has_value()){
                root_node.funcs.push_back(parse_stmnt());
            };
            return root_node;
        };
};

