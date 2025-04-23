#pragma once
#include <variant>
#include <vector>
#include <string>

enum class TokenType {_exit, _int_lit, _semi_col, _open_par, _close_par, _func, _open_curl, _close_curl, _type_dec, _ident, _eq};

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
        Token val;
    };
    
    struct Expr {
        std::variant<IntLit, Ident> var;
    };

    struct DeclareIdent {
        Type type;
        Ident ident;
        Expr expr;
    };

    struct StmntExit {
        Expr expr;
    };

    struct Stmnt {
        std::variant<StmntExit, DeclareIdent> var; 
    };

    struct Fun {
        std::vector<Stmnt> statements;
    };

    struct Prog {
        // std::vector<Fun> funcs;
        std::vector<Stmnt> funcs;
    };
};
