//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include "lexer.h"
#include "ast.h"


namespace bond {

    class Parser {
    public:
        Parser(std::vector<Token> &tokens, Context *context) : m_tokens(tokens) { ctx = context; }
        std::shared_ptr<Expression> parse();

    private:
        std::vector<Token> &m_tokens;
        Context *ctx;
        uint32_t m_current = 0;

        std::shared_ptr<Expression> expression();

        std::shared_ptr<Expression> equality();

        bool match(std::initializer_list<TokenType> types);

        bool check(TokenType type);

        Token advance();

        bool is_at_end();

        Token peek();

        Token previous();


        std::shared_ptr<Expression> comparison();

        std::shared_ptr<Expression> term();

        std::shared_ptr<Expression> factor();

        std::shared_ptr<Expression> unary();

        std::shared_ptr<Expression> primary();

        Token consume(TokenType type, const SharedSpan &span, const std::string& message);

        void synchronize();

    };

} // bond
