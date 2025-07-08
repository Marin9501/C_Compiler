#pragma once
#include <optional>
#include <variant>
#include <vector>
#include <string>

enum class TokenType {
    //Statements
    _exit, _type_dec, _if, _else, _for, _break,
    //Expressions?
     _int_lit, _ident, _bool_lit,
    //Math
     _bin_op, _eq,
    //Other
    _semi_col, _open_par, _close_par, _open_curl, _close_curl};

struct Token {
    TokenType type;
    std::string val = "";
};

namespace Node {
    struct IntLit {
        Token val;
    };

    struct BoolLit {
        Token bool_lit;
    };

    struct Type {
        Token type;
    };

    struct Ident {
        Token ident;
    };

    struct Expr;
    
    struct BinExpr {
        Expr* Left;
        Token operation;
        Expr* Right;
    };

    struct VarAssign {
        Ident* ident;
        Expr* expr;
    };

    struct Expr {
        std::variant<IntLit*, Ident*, BinExpr*, BoolLit*, VarAssign*> var;
    };

    struct DeclareIdent {
        Type* type;
        Ident* ident;
        Expr* expr;
    };

    struct StmntExit {
        Expr* expr;
    };

    struct Scope;

    struct IfElse {
        Expr* condition;
        Scope* if_block;
        std::optional<Scope*> else_block;
    };

    struct Stmnt;

    struct ForLoop {
        std::optional<Stmnt*> inicialization;
        std::optional<Expr*> condition;
        std::optional<Expr*> increment;
        Scope* scope;
    };

    struct Break {
        Token token;
    };

    struct Stmnt {
        std::variant<StmntExit*, DeclareIdent*, Scope*, IfElse*, VarAssign*, ForLoop*, Break*> var; 
    };

    struct Scope {
        std::vector<Stmnt*> stmnts;
    };

    struct Fun {
        std::vector<Stmnt> statements;
    };

    struct Prog {
        // std::vector<Fun> funcs;
        std::vector<Scope*> funcs;
    };
};
