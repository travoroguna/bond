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

    struct ParserError : public std::exception {
        ParserError(const std::string &error, const SharedSpan &span) {
            this->span = span;
            this->error = error;
        }

        [[nodiscard]] const char *what() const noexcept override {
            return error.c_str();
        }

        std::string error;
        SharedSpan span;

    };


    struct Variable {
        Variable(const std::string &name, const std::shared_ptr<Span> &span, bool is_global, bool is_mut) {
            this->name = name;
            this->span = span;
            this->is_global = is_global;
            this->is_mut = is_mut;
        }

        std::string name;
        std::shared_ptr<Span> span;
        bool is_mut = true;
        bool is_global = false;
    };

    class Scopes {
    public:
        explicit Scopes(Context *context) { m_ctx = context; }

        void new_scope();

        void end_scope();

        void declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut);

        void declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut, bool is_global);

        bool is_declared(const std::string &name);

        bool is_declared_in_current_scope(const std::string &name);

        bool is_global(const std::string &name);

        std::optional<std::shared_ptr<Variable>> get(const std::string &name);

        bool has_globals() { return !m_scopes.empty(); }

        void print();

    private:
        std::vector<std::unordered_map<std::string, std::shared_ptr<Variable>>> m_scopes;
        Context *m_ctx;
    };

    class Parser {
    public:
        Parser(const std::vector<Token> &tokens, Context *context);

        ~Parser() { if (!m_managed_scope) delete m_scopes; }

        std::vector<std::shared_ptr<Node>> parse();

        Scopes *get_scopes() { return m_scopes; }

        void set_scopes(Scopes *scopes);

        void disable_reporting() { m_report = false; }
        std::vector<std::pair<std::string, SharedSpan>> &get_diagnostics() { return m_diagnostics; }

        static std::shared_ptr<Span>
        span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end);

    private:

        Scopes *m_scopes;
        std::vector<Token> m_tokens;
        Context *ctx;
        uint32_t m_current = 0;
        bool in_function = false;
        bool is_error_func = false;
        bool m_managed_scope = false;

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

        Token consume(TokenType type, const SharedSpan &span, const std::string &message);

        void synchronize();

        std::shared_ptr<Node> statement();

        std::shared_ptr<Node> print_stmnt();

        std::shared_ptr<Node> expr_stmnt();

        std::optional<std::shared_ptr<Node>> declaration();

        std::shared_ptr<Node> variable_declaration();

        std::shared_ptr<Node> assignment();

        std::vector<std::shared_ptr<Node>> expr_list(TokenType end_token);

        std::shared_ptr<Node> get_item();

        std::shared_ptr<Node> if_stmnt();

        std::shared_ptr<Node> _or();

        std::shared_ptr<Node> _and();

        std::shared_ptr<Node> while_statement();

        std::shared_ptr<Node> call();

        std::shared_ptr<Node> f_call(const std::shared_ptr<Node> &callee);

        std::shared_ptr<Node> for_statement();

        std::shared_ptr<Node> block(bool create_scope);

        std::shared_ptr<Node> closure_declaration(bool is_expression);

        std::shared_ptr<Node> return_statement();

        std::shared_ptr<Node> struct_declaration();

        std::shared_ptr<Node> function_declaration(bool is_method, bool is_expression);

        std::shared_ptr<Node> import_declaration();

        std::shared_ptr<Node> try_statement();

        std::shared_ptr<Node> break_statement();

        std::shared_ptr<Node> continue_statement();

        std::shared_ptr<Node> async_declaration();

        std::shared_ptr<Node> await_statement();

        std::shared_ptr<Node> ok_statement() ;

        std::shared_ptr<Node> err_statement();

        bool m_in_loop = false;

        std::shared_ptr<Node> bit_or();

        std::shared_ptr<Node> bit_xor();

        std::shared_ptr<Node> bit_and();

        bool m_report = true;

        bool m_in_async = false;

        std::vector<std::pair<std::string, SharedSpan>> m_diagnostics;

        std::vector<std::pair<SharedNode, SharedNode>> key_value_pairs(TokenType end_token);

        SharedTypeNode parse_type();

        std::vector<SharedTypeNode> parse_type_list(TokenType end_token);

        std::shared_ptr<Param> parse_parameter();

        SharedTypeNode parse_return_type();
    };

    std::string split_at_last_occur(const std::string& str, char c);
} // bond
