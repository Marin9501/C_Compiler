#pragma once
#include <variant>
#include <vector>
#include <string>

enum class TokenType {
    //Expressions?
    _exit, _int_lit, _func, _type_dec, _ident,
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

    struct Stmnt {
        std::variant<StmntExit*, DeclareIdent*, Scope*> var; 
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
