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

    struct Variable {
        Variable(const std::shared_ptr<Span> &span, size_t scope_level) {
            m_span = span;
            m_scope_level = scope_level;
        }

        bool is_global{false};
        bool is_used{false};
        std::shared_ptr<Span> m_span;
        size_t m_scope_level;

    };

    class Symbols {
    public:
        explicit Symbols(Context *ctx) { m_ctx = ctx; }

        void new_scope() { m_scopes.emplace_back(std::unordered_map<std::string, std::shared_ptr<Variable>>()); }

        std::unordered_map<std::string, std::shared_ptr<Variable>> pop_scope() {
            auto last = m_scopes[m_scopes.size() - 1];
            m_scopes.pop_back();
            return last;
        }

        std::unordered_map<std::string, std::shared_ptr<Variable>> &current_scope() {
            return m_scopes[m_scopes.size() - 1];
        }

        std::optional<std::shared_ptr<Variable>> get_variable(const std::string &name);

        std::optional<std::shared_ptr<Variable>> declare(const std::string &name, const std::shared_ptr<Span> &span);

        void use(const std::string &name);

        std::unordered_map<std::string, std::shared_ptr<Variable>> symbols;

    private:
        std::vector<std::unordered_map<std::string, std::shared_ptr<Variable>>> m_scopes;
        Context *m_ctx;
    };

    class Parser {
    public:
        Parser(std::vector<Token> &tokens, Context *context) : m_tokens(tokens), symbols(context) { ctx = context; }

        std::vector<std::shared_ptr<Node>> parse();

        Symbols *get_symbols() { return &symbols; }

    private:
        std::vector<Token> &m_tokens;
        Context *ctx;
        uint32_t m_current = 0;
        Symbols symbols;

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

        static std::shared_ptr<Span>
        span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end);

        std::shared_ptr<Node> assignment();
    };

} // bond
