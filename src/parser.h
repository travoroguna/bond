//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include "lexer.h"
#include "ast.h"
#include <unordered_set>
#include <unordered_map>
#include <optional>

namespace bond {

    struct Variable{
        Variable() = default;
        bool is_global {false};
        bool is_used {false};
        bool is_initialized {false};
    };

    class Parser {
    public:
        Parser(std::vector<Token> &tokens, Context *context) : m_tokens(tokens) { ctx = context; }
        std::vector<std::shared_ptr<Node>> parse();

    private:
        std::vector<Token> &m_tokens;
        Context *ctx;
        uint32_t m_current = 0;
        size_t scope_level = 0;

        std::vector<std::unordered_map<std::string, Variable>> m_scopes;

        void new_scope() { m_scopes.emplace_back(std::unordered_map<std::string, Variable>()); }

        std::shared_ptr<Node> expression();

        std::shared_ptr<Node> equality();

        bool match(std::initializer_list<TokenType> types);

        bool check(TokenType type);

        Token advance();

        bool is_at_end();

        Token peek();

        Token previous();

        std::shared_ptr<Node> comparison();

        std::shared_ptr<Node> term();

        std::shared_ptr<Node> factor();

        std::shared_ptr<Node> unary();

        std::shared_ptr<Node> primary();

        Token consume(TokenType type, const SharedSpan &span, const std::string& message);

        void synchronize();

        std::shared_ptr<Node> statement();

        std::shared_ptr<Node> print_stmnt();

        std::shared_ptr<Node> expr_stmnt();

        std::optional<std::shared_ptr<Node>> declaration();

        std::shared_ptr<Node> variable_declaration();

        static std::shared_ptr<Span> span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end);
    };

} // bond
