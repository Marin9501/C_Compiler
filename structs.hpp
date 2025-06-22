#pragma once
#include <variant>
#include <vector>
#include <string>

enum class TokenType {
    //Statements
    _exit, _type_dec, _if,
    //Expressions?
     _int_lit, _func, _ident,
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

    struct Expr {
        std::variant<IntLit*, Ident*, BinExpr*> var;
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

    struct If {
        Expr* condition;
        Scope* scope;
    };

    struct Stmnt {
        std::variant<StmntExit*, DeclareIdent*, Scope*, If*> var; 
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
