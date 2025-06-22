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

        int get_prec(Token token){
            if (token.val == "+" || token.val == "-"){
                return 0;
            } else if (token.val == "*" || token.val == "/"){
                return 1;
            } else {
                std::cerr << "Unknown binary operator " << token.val << "\n";
                exit(-1);
            }
        };

        Node::Expr* parse_term(){
            Node::Expr* expr = allocator.alloc<Node::Expr>();
            if (look().has_value() && look().value().type == TokenType::_int_lit){
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
            } else if (look().has_value() && look().value().type == TokenType::_open_par){
                mov_next(); //Skip '('
                expr = parse_expr();
                if (look().has_value() && look().value().type != TokenType::_close_par){
                    std::cerr << "Expected ')'\n";
                    exit(-1);
                };
                mov_next(); //Skip ')'
                return expr;
            } else {
                std::cerr << "Something went wrong when parsing expression\n";
                exit(-1);
            };
        };

        Node::Expr* parse_bin_expr(int min_prec=0){
            Node::Expr* lhs = parse_term();
            while (look().has_value() && look().value().type == TokenType::_bin_op && get_prec(look().value()) >= min_prec){
                Node::BinExpr* bin_expr = allocator.alloc<Node::BinExpr>();
                int next_prec = get_prec(look().value()) + 1;
                bin_expr->operation = mov_next().value();
                Node::Expr* rhs = parse_bin_expr(next_prec);
                bin_expr->Left = lhs;
                bin_expr->Right = rhs;

                Node::Expr* parent = allocator.alloc<Node::Expr>();
                parent->var = bin_expr;
                lhs = parent;    
            };
            return lhs;
        };

        Node::Expr* parse_expr(){
            Node::Expr* expr = allocator.alloc<Node::Expr>();
            if (look(1).has_value() && look(1).value().type == TokenType::_bin_op){
                expr = parse_bin_expr();
            } else {
                expr = parse_term();
            };
                return expr;
        };

        Node::If* parse_if(){
            Node::If* if_block = allocator.alloc<Node::If>();
            mov_next(); //Skip 'if' token
            if (look().has_value() && look().value().type != TokenType::_open_par){
                std::cerr << "Expected '(' after 'if' statement\n";
                exit(-1);
            };
            mov_next(); //Skip '('
            if_block->condition = parse_expr();
            mov_next(); //Skip ')'
            if_block->scope = parse_scope();
            return if_block;
        };
        
        Node::Stmnt* parse_stmnt(){
            Node::Stmnt* stmnt_node = allocator.alloc<Node::Stmnt>();
            if (look().has_value() && look().value().type == TokenType::_if){
                Node::If* if_block = parse_if();
                stmnt_node->var = if_block;
            } else if (look().has_value() && look().value().type == TokenType::_open_curl){
                Node::Scope* scope = parse_scope();
                stmnt_node->var = scope;
            } else if (look().has_value() && look().value().type == TokenType::_close_curl){
                return stmnt_node;
            } else {
                if (look().has_value() && look().value().type == TokenType::_exit){
                    if (look(1).has_value() && look(1).value().type == TokenType::_open_par){
                        mov_next(); //Skip 'exit' token
                        mov_next(); //Skip '('
                        Node::StmntExit* stmnt_exit = allocator.alloc<Node::StmntExit>();
                        stmnt_exit->expr = parse_expr();
                        stmnt_node->var = stmnt_exit;
                        if (look().has_value() && look().value().type == TokenType::_close_par){
                            mov_next(); //Skip ')'
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
            };

            if (look().has_value() && look().value().type == TokenType::_semi_col){
                mov_next();
            }
            return stmnt_node;
        };

        Node::Scope* parse_scope(){
            Node::Scope* scope = allocator.alloc<Node::Scope>();
            if (look().has_value()){
                if (look().value().type != TokenType::_open_curl){
                std::cerr << "Scope has to begin with '{'\n";
                exit(-1);
                } else {
                mov_next(); //Skip '{'
                };
            };

            while (look().has_value() && look().value().type != TokenType::_close_curl){
                scope->stmnts.push_back(parse_stmnt());
            };
            mov_next(); //Skip '}'

            return scope;
        };

    public:
        Parser(const std::vector<Token>& tokens) : tokens(tokens), tokens_len(tokens.size()), allocator(1024*1024 * 5){
        };

        Node::Prog parse(){
            Node::Prog root_node;
            while (look().has_value()){
                root_node.funcs.push_back(parse_scope());
            };
            return root_node;
        };
};

