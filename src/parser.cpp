//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "parser.h"
#include <initializer_list>
#include <algorithm>
#include <exception>
#include <optional>


namespace bond {
    struct ParserError : public std::exception {
        ParserError(const std::string& error, const SharedSpan& span) {
            this->span = span;
            this->error = error;
        }

        [[nodiscard]] const char* what() const noexcept override {
            return error.c_str();
        }

        std::string error;
        SharedSpan span;

    };

    std::vector<std::shared_ptr<Node>> Parser::parse() {
        //why? because I can
        std::vector<std::shared_ptr<Node>> nodes;
        new_scope();

        while (!is_at_end()) {
            auto res = declaration();
            if (!res.has_value()) continue;
            nodes.push_back(res.value());
        }

        return nodes;
    }

    void Parser::synchronize() {
        advance();

        while (!is_at_end()) {
            if (previous().get_type() == TokenType::SEMICOLON) return;

            switch (peek().get_type()) {
                case TokenType::CLASS:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
            }

            advance();
        }
    }

    std::optional<std::shared_ptr<Node>> Parser::declaration() {
        try {
            if (match({TokenType::VAR})) return variable_declaration();
            return statement();
        }
        catch (ParserError& e) {
            ctx->error(e.span, e.error);
            synchronize();
            return std::nullopt;
        }
    }

    std::shared_ptr<Span> Parser::span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end) {
        return std::make_shared<Span>(start->module_id, start->start, end->end, end->line);
    }

    std::shared_ptr<Node> Parser::variable_declaration() {
        auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected variable name after var keyword");
        consume(TokenType::EQUAL, peek().get_span(), "Expected '=' after variable name");
        auto initializer = expression();
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in variable declaration");

        return std::make_shared<NewVar>(span_from_spans(id.get_span(), previous().get_span()), id.get_lexeme(), initializer, m_scopes.size() == 1);
    }

    std::shared_ptr<Node> Parser::statement() {
        if (match({TokenType::PRINT})) return print_stmnt();
        return expr_stmnt();
    }

    std::shared_ptr<Node> Parser::print_stmnt() {
        auto expr = expression();
        consume(TokenType::SEMICOLON, expr->get_span(), "Expected semicolon, ';' after expression");
        return std::make_shared<Print>(expr->get_span(), expr);
    }

    std::shared_ptr<Node> Parser::expr_stmnt() {
        auto expr = expression();
        consume(TokenType::SEMICOLON, expr->get_span(), "Expected semicolon, ';' after expression");
        return std::make_shared<ExprStmnt>(expr->get_span(), expr);
    }


    std::shared_ptr<Node> Parser::expression() {
        return equality();
    }

    std::shared_ptr<Node> Parser::equality() {
        auto expr = comparison();

        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
            auto op = previous();
            auto right = comparison();

            auto expr_span = expr->get_span();
            auto new_span = std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::comparison() {
        auto expr = term();

        while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
            auto op = previous();
            auto right = term();

            auto expr_span = expr->get_span();
            auto new_span = std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::term() {
        auto expr = factor();

        while (match({TokenType::MINUS, TokenType::PLUS})) {
            auto op = previous();
            auto right = factor();

            auto expr_span = expr->get_span();
            auto new_span = std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::factor() {
        auto expr = unary();

        while (match({TokenType::SLASH, TokenType::STAR})) {
            auto op = previous();
            auto right = unary();

            auto expr_span = expr->get_span();
            auto new_span = std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::unary() {
        if (match({TokenType::BANG, TokenType::MINUS})) {
            auto op = previous();
            auto right = unary();
            return std::make_shared<Unary>(right->get_span(), op, right);
        }

        return primary();
    }

    std::shared_ptr<Node> Parser::primary() {
        if (match({TokenType::FALSE})) return std::make_shared<FalseLiteral>(previous().get_span());
        if (match({TokenType::TRUE})) return std::make_shared<TrueLiteral>(previous().get_span());
        if (match({TokenType::NIL})) return std::make_shared<NilLiteral>(previous().get_span());

        if (match({TokenType::NUMBER})) return std::make_shared<NumberLiteral>(previous().get_span(), previous().get_lexeme());
        if (match({TokenType::STRING})) return std::make_shared<StringLiteral>(previous().get_span(), previous().get_lexeme());

        if (match({TokenType::IDENTIFIER})) return std::make_shared<Identifier>(previous().get_span(), previous().get_lexeme(), m_scopes.size() == 1);
        if (match({TokenType::LEFT_PAREN})) {
            auto expr = expression();
            consume(TokenType::RIGHT_PAREN, expr->get_span(), "Expected ')' after expression.");
            return expr;
        }

        throw ParserError("Expected expression.", peek().get_span());
    }


    Token Parser::consume(TokenType type, const SharedSpan &span, const std::string& message) {
        if (check(type)) return advance();
        throw ParserError(message, span);
    }


    bool Parser::match(std::initializer_list<TokenType> types) {
        if (std::ranges::any_of(types, [this](TokenType t) { return check(t); })) {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::check(TokenType type) {
        if (is_at_end()) return false;
        return peek().get_type() == type;
    }

    Token Parser::advance() {
        if (!is_at_end()) m_current++;
        return previous();
    }

    bool Parser::is_at_end() {
        return peek().get_type() == TokenType::EndOfFile;
    }

    Token Parser::peek() {
        return m_tokens[m_current];
    }

    Token Parser::previous() {
        return m_tokens[m_current - 1];
    }


} // bond