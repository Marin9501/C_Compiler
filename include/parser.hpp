#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include "structs.hpp"
#include "allocator.hpp"

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
            } else if (look().has_value() && look().value().type == TokenType::_bool_lit){
                Node::BoolLit* bool_lit = allocator.alloc<Node::BoolLit>();
                bool_lit->bool_lit = mov_next().value();

                expr->var = bool_lit;
                return expr;

            } else if (look().has_value() && look().value().type == TokenType::_ident){
                Node::Ident* ident = allocator.alloc<Node::Ident>();
                ident->ident = mov_next().value();

                expr->var = ident;
                return expr;
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
                std::cerr << "Something went wrong when parsing term\n";
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
            } else if (look().has_value() && look().value().type == TokenType::_ident &&
                    look(1).has_value() && look(1).value().type == TokenType::_eq){
                Node::VarAssign* var_assign= allocator.alloc<Node::VarAssign>();
                Node::Ident* ident = allocator.alloc<Node::Ident>();
                ident->ident = mov_next().value();
                var_assign->ident = ident;
                mov_next(); //Skip '='
                var_assign->expr = parse_expr();
                expr->var = var_assign;
            } else {
                expr = parse_term();
            };
                return expr;
        };

        Node::IfElse* parse_if_else(){
            Node::IfElse* if_stmnt = allocator.alloc<Node::IfElse>();
            mov_next(); //Skip 'if' token
            if (look().has_value() && look().value().type != TokenType::_open_par){
                std::cerr << "Expected '(' after 'if' statement\n";
                exit(-1);
            };
            mov_next(); //Skip '('
            if_stmnt->condition = parse_expr();
            mov_next(); //Skip ')'
            if_stmnt->if_block = parse_scope();
            if (look().has_value() && look().value().type == TokenType::_else){
                mov_next(); //Skip 'else' token
                Node::Scope* scope = allocator.alloc<Node::Scope>();
                scope->stmnts.push_back(parse_stmnt());
                if_stmnt->else_block = scope;
            }
            return if_stmnt;
        };

        void check_semi_col(bool force=true){
            if (force){
                if (look().has_value() && look().value().type == TokenType::_semi_col){
                    mov_next();
                } else {
                    std::cerr << "Expected ; at the end of a line\n";
                    exit(-1);
                }
            } else {
                if (look().has_value() && look().value().type == TokenType::_semi_col){
                    mov_next();
                }
            }
        };

        Node::DeclareIdent* parse_declaration(){
            Node::Type* type = allocator.alloc<Node::Type>();
            type->type = mov_next().value();
            Node::Ident* ident = allocator.alloc<Node::Ident>();
            ident->ident = mov_next().value();
            mov_next(); //skip `=`
            Node::DeclareIdent* declare = allocator.alloc<Node::DeclareIdent>();
            declare->type = type;
            declare->ident = ident;
            declare->expr = parse_expr();
            check_semi_col();
            return declare;
        }

        Node::VarAssign* parse_assign(){
                Node::VarAssign* var_assign= allocator.alloc<Node::VarAssign>();
                Node::Ident* ident = allocator.alloc<Node::Ident>();
                ident->ident = mov_next().value();
                var_assign->ident = ident;
                mov_next(); //Skip '='
                var_assign->expr = parse_expr();
                check_semi_col(true);
                return var_assign;
        }


        Node::ForLoop* parse_for_loop(){
            Node::ForLoop* for_loop = allocator.alloc<Node::ForLoop>();
            mov_next(); //Skip 'for' keyword
            if (look().has_value() && look().value().type == TokenType::_open_par){
                mov_next(); //Skip '('
                if (look().has_value() && look().value().type != TokenType::_semi_col){
                    for_loop->inicialization = parse_stmnt();
                } else {
                    mov_next(); //Skip ';'
                }

                if (look().has_value() && look().value().type != TokenType::_semi_col){
                    for_loop->condition = parse_expr();
                    mov_next(); //Skip ';'
                } else {
                    mov_next(); //Skip ';'
                }
 

                if (look().has_value() && look().value().type != TokenType::_close_par){
                        for_loop->increment = parse_expr();
                }

                if (look().has_value() && look().value().type == TokenType::_close_par){
                    mov_next(); //Skip ')'
                } else {
                    std::cerr << "Expected ')' when doing for loop\n";
                    exit(-1);
                }

                for_loop->scope = parse_scope();

            } else {
                std::cerr << "Expected '(' when doing for loop\n";
                exit(-1);
            }
            return for_loop;
        }
        
        Node::Stmnt* parse_stmnt(){
            Node::Stmnt* stmnt_node = allocator.alloc<Node::Stmnt>();
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
                check_semi_col();
            } else if (look().has_value() && look().value().type == TokenType::_type_dec && 
                    look(1).has_value() && look(1).value().type == TokenType::_ident &&  
                    look(2).has_value() && look(2).value().type == TokenType::_eq){
                stmnt_node->var = parse_declaration();
            } else if (look().has_value() && look().value().type == TokenType::_ident &&
                    look(1).has_value() && look(1).value().type == TokenType::_eq){
                stmnt_node->var = parse_assign();
            } else if (look().has_value() && look().value().type == TokenType::_if){
                Node::IfElse* if_block = parse_if_else();
                stmnt_node->var = if_block;
            } else if (look().has_value() && look().value().type == TokenType::_close_curl){
                check_semi_col(false);
                return stmnt_node;
            } else if (look().has_value() && look().value().type == TokenType::_open_curl){
                Node::Scope* scope = parse_scope();
                stmnt_node->var = scope;
                check_semi_col(false);
            } else if (look().has_value() && look().value().type == TokenType::_for){
                Node::ForLoop* for_loop= parse_for_loop();
                stmnt_node->var = for_loop;
            } else {
                std::cerr << "Something went worng when parsing statment\n";
                exit(-1);
            };

            return stmnt_node;
        };

        Node::Scope* parse_scope(){
            Node::Scope* scope = allocator.alloc<Node::Scope>();
            if (look().has_value()){
                if (look().value().type != TokenType::_open_curl){
                std::cerr << "Scope has to begin with '{'\n";
                exit(-1);
                }
                mov_next(); //Skip '{'
            };

            while (look().has_value() && look().value().type != TokenType::_close_curl){
                scope->stmnts.push_back(parse_stmnt());
            };
            mov_next(); //Skip '}'
            check_semi_col(false);

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

