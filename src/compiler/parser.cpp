//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "parser.h"
#include <initializer_list>
#include <algorithm>
#include <optional>
#include <ranges>
#include <iostream>

namespace bond {

    std::vector<std::shared_ptr<Node>> Parser::parse() {
        std::vector<std::shared_ptr<Node>> nodes;

        if (!m_scopes->has_globals()) {
            m_scopes->new_scope();

            auto span = std::make_shared<Span>(0, 0, 0, 1);
            const char *builtins[] = {
                    "println", "print", "dump", "exit",
                    "help", "type_of", "instance_of", "input",
                    "Int", "Float", "String", "Bool", "List",
                    "debug_break", "__future__", "format", "iter", "Future"
            };

            for (auto &builtin: builtins) {
                m_scopes->declare(builtin, span, false);
            }
        }

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
                case TokenType::STRUCT:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::RETURN:
                    return;
            }

            advance();
        }
    }

    std::optional<std::shared_ptr<Node>> Parser::declaration() {
        try {
            if (match({TokenType::VAR})) return variable_declaration();
            else if (match({TokenType::STRUCT})) return struct_declaration();
            else if (match({TokenType::FUN})) return function_declaration(false, false);
            else if (match({TokenType::IMPORT})) return import_declaration();
            else if (match({TokenType::ASYNC})) return async_declaration();
            else return statement();
        }
        catch (ParserError &e) {
            if (m_report) ctx->error(e.span, e.error);
            m_diagnostics.emplace_back(e.error, e.span);
            synchronize();
            return std::nullopt;
        }
    }

    std::shared_ptr<Span>
    Parser::span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end) {
        return std::make_shared<Span>(start->module_id, start->start, end->end, end->line);
    }

    std::shared_ptr<Node> Parser::async_declaration() {
        throw ParserError("Asyncio not implemented", previous().get_span());

        auto pre = m_in_async;
        m_in_async = true;
        consume(TokenType::FUN, peek().get_span(), "Expected function after async keyword");
        auto function = function_declaration(false, false);
        m_in_async = pre;
        return std::make_shared<AsyncDef>(function->get_span(), function);
    }


    std::string split_at_last_occur(const std::string &str, char c) {
        auto pos = str.rfind(c);
        if (pos == std::string::npos) return str;
        return str.substr(pos + 1);
    }

    std::shared_ptr<Node> Parser::import_declaration() {
        auto path = consume(TokenType::STRING, peek().get_span(), "Expected string after import keyword");

        if (match({TokenType::SEMICOLON})) {
            auto alias = split_at_last_occur(split_at_last_occur(path.get_lexeme(), ':'), '/');

            m_scopes->declare(alias, path.get_span(), false);
            return std::make_shared<ImportDef>(path.get_span(), path.get_lexeme(), alias);
        }

        consume(TokenType::AS, peek().get_span(), "Expected 'as' after import path");
        auto alias = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected identifier after 'as' keyword");
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after import statement");

        m_scopes->declare(alias.get_lexeme(), alias.get_span(), false);

        return std::make_shared<ImportDef>(span_from_spans(path.get_span(), alias.get_span()), path.get_lexeme(),
                                           alias.get_lexeme());
    }

    std::shared_ptr<Node> Parser::struct_declaration() {
        auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected struct name after struct keyword");


        if (m_scopes->is_declared(id.get_lexeme())) {
            if (m_report) ctx->error(id.get_span(), fmt::format("Struct {} is already declared in this scope", id.get_lexeme()));
            auto sp = m_scopes->get(id.get_lexeme()).value()->span;
            throw ParserError(fmt::format("Note Struct {} is already declared here", id.get_lexeme()), sp);
        }

        m_scopes->declare(id.get_lexeme(), id.get_span(), false);
        m_scopes->new_scope();


        consume(TokenType::LEFT_BRACE, peek().get_span(), "Expected '{' after struct name");

        //capture instance variables
        std::vector<std::shared_ptr<Param>> instance_variables;

        while (check(TokenType::VAR)) {
            consume(TokenType::VAR, peek().get_span(), "");

            auto var = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected instance variable name");
            auto exists = std::find_if(instance_variables.begin(), instance_variables.end(), [&var](const auto &param) {
                return param->name == var.get_lexeme();
            });

            if (exists != instance_variables.end()) {
                throw ParserError(fmt::format("Instance Variable {} is already declared", var.get_lexeme()), var.get_span());
            }

            std::optional<std::shared_ptr<TypeNode>> type = std::nullopt;

            if (match({TokenType::COLON})) {
                type = parse_type();
            }

            instance_variables.push_back(std::make_shared<Param>(var.get_lexeme(), type, var.get_span()));

            consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after instance variable declaration");
        }

        //capture methods

        std::vector<std::shared_ptr<Node>> methods;
        std::unordered_map<std::string, SharedSpan> instance_methods;

        while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
            if (check(TokenType::VAR)) {
                throw ParserError("Instance variables must be declared before methods", peek().get_span());
            } else if (check(TokenType::FUN)) {
                consume(TokenType::FUN, peek().get_span(), "");
                auto method = function_declaration(true, false);
                methods.push_back(method);

                if (instance_methods.contains(dynamic_cast<FuncDef *>(method.get())->get_name())) {
                    if (m_report) ctx->error(method->get_span(), fmt::format("Method {} is already declared",
                                                               dynamic_cast<FuncDef *>(method.get())->get_name()));
                    auto sp = instance_methods[dynamic_cast<FuncDef *>(method.get())->get_name()];
                    throw ParserError(fmt::format("Note Method {} is already declared here",
                                                  dynamic_cast<FuncDef *>(method.get())->get_name()), sp);
                }

                instance_methods[dynamic_cast<FuncDef *>(method.get())->get_name()] = method->get_span();
                continue;
            }

            break;

        }

        consume(TokenType::RIGHT_BRACE, peek().get_span(), "Expected '}' after struct body");
        m_scopes->end_scope();
        return std::make_shared<StructNode>(id.get_span(), id.get_lexeme(), instance_variables, methods);

    }

    std::shared_ptr<Param> Parser::parse_parameter() {
        auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected parameter name");
        if (check(TokenType::COLON)) {
            consume(TokenType::COLON, peek().get_span(), "");
            auto type = parse_type();
            return std::make_shared<Param>(id.get_lexeme(), type, id.get_span());
        }

        return std::make_shared<Param>(id.get_lexeme(), std::nullopt, id.get_span());
    }

    std::shared_ptr<Node> Parser::function_declaration(bool is_method, bool is_expression) {
        bool pre = in_function;
        bool pre_is_err_func = is_error_func;

        in_function = true;

        std::string name;
        SharedSpan span;

        if (!is_expression) {
            auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected function name after fun keyword");

            if (is_method) {
                if (m_scopes->is_declared_in_current_scope(id.get_lexeme())) {
                    if (m_report) ctx->error(id.get_span(),
                               fmt::format("Method {} is already declared in this struct", id.get_lexeme()));
                    auto sp = m_scopes->get(id.get_lexeme()).value()->span;
                    throw ParserError(fmt::format("Note Method {} is declared here", id.get_lexeme()), sp);
                }
            } else {
                if (m_scopes->is_declared(id.get_lexeme())) {
                    if (m_report) ctx->error(id.get_span(),
                               fmt::format("Function {} is already declared in this scope", id.get_lexeme()));
                    auto sp = m_scopes->get(id.get_lexeme()).value()->span;
                    throw ParserError(fmt::format("Note {} is already declared here", id.get_lexeme()), sp);
                }

                m_scopes->declare(id.get_lexeme(), id.get_span(), false);
            }

            name = id.get_lexeme();
            span = id.get_span();
        } else {
            name = "<anonymous function>";
            span = peek().get_span();
        }

        consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after function name");

        std::vector<std::shared_ptr<Param>> params;

        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                if (params.size() >= 1024) {
                    throw ParserError("Cannot have more than 1024 parameters", peek().get_span());
                }
                auto param = parse_parameter();

                params.emplace_back(param);
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after parameters");


        std::optional<std::shared_ptr<TypeNode>> return_type = std::nullopt;

        if (match({TokenType::BANG})) {
            is_error_func = true;

            if (match({TokenType::LESS})) {
                auto ok = parse_type();
                consume(TokenType::COMMA, peek().get_span(), "Expected ',' after ok type");
                auto err = parse_type();
                consume(TokenType::GREATER, peek().get_span(), "Expected '>' after err type");
                return_type = std::make_shared<ResultType>(span_from_spans(ok->get_span(), err->get_span()), ok, err);
            }
        }
        else {
            auto next = peek();

            if (next.get_type() != TokenType::LEFT_BRACE) {
                return_type = parse_type();
            }
        }

        m_scopes->new_scope();
        for (auto &param: params) {
            m_scopes->declare(param->name, param->span, true);
        }

        std::shared_ptr<Node> body;

        if (peek().get_type() == TokenType::LEFT_BRACE) {
            consume(TokenType::LEFT_BRACE, peek().get_span(), "Expected '{' before function body");
            body = block(false);
        } else {
            body = expression();
        }
//        auto body = block(false);
        m_scopes->end_scope();

        bool tmp = is_error_func;
        is_error_func = pre_is_err_func;
        in_function = pre;

        return std::make_shared<FuncDef>(span_from_spans(span, previous().get_span()),
                                         name,
                                         params,
                                         body, tmp, return_type);
    }

    std::shared_ptr<Node> Parser::variable_declaration() {
        if (match({TokenType::LEFT_SQ})) {
            auto start = previous().get_span();
            auto exprs = expr_list(TokenType::RIGHT_SQ);
            consume(TokenType::RIGHT_SQ, previous().get_span(), "Expected ']' after expression");

            consume(TokenType::EQUAL, peek().get_span(), "Expected '=' after bindings");
            auto initializer = expression();

            for (auto &expr: exprs) {
                if (!instanceof<Identifier>(expr.get())) {
                    throw ParserError("Expected identifier in declaration", expr.get()->get_span());
                }

                auto id = std::dynamic_pointer_cast<Identifier>(expr);
                if (m_scopes->is_declared(id->get_name())) {
                    if (m_report) ctx->error(id->get_span(),
                               fmt::format("Variable {} is already declared in this scope", id->get_name()));
                    auto sp = m_scopes->get(id->get_name()).value()->span;
                    throw ParserError(fmt::format("Note Variable {} is already declared here", id->get_name()), sp);
                }

                m_scopes->declare(id->get_name(), id->get_span(), true);
            }

            consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in variable declaration");
            return std::make_shared<StructuredAssign>(start, exprs, initializer);
        }


        auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected variable name after var keyword");

        std::optional<SharedTypeNode> type = std::nullopt;

        if (match({TokenType::COLON})) {
            type = parse_type();
        }

        if (m_scopes->is_declared(id.get_lexeme())) {
            if (m_report) ctx->error(id.get_span(), fmt::format("Variable {} is already declared in this scope", id.get_lexeme()));
            auto sp = m_scopes->get(id.get_lexeme()).value()->span;
            throw ParserError(fmt::format("Note Variable {} is already declared here", id.get_lexeme()), sp);
        }

        consume(TokenType::EQUAL, peek().get_span(), "Expected '=' after variable name");
        auto initializer = expression();
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in variable declaration");

        m_scopes->declare(id.get_lexeme(), id.get_span(), true);

        return std::make_shared<NewVar>(span_from_spans(id.get_span(), previous().get_span()), id.get_lexeme(),
                                        initializer, type);
    }

    SharedTypeNode Parser::parse_type() {
        auto start = previous().get_span();
        if (match({TokenType::IDENTIFIER})) {
            auto id = previous();

            std::vector<SharedTypeNode> resolutions;
            bool is_generic = false;

            while (true) {
                if (match({TokenType::DOT})) {
                    auto next = parse_type();
                    resolutions.push_back(next);
                } else if (match({TokenType::LESS})) {
                    is_generic = true;
                    do {
                        resolutions.push_back(parse_type());
                    } while ((match({TokenType::COMMA})));

                    consume(TokenType::GREATER, peek().get_span(), "Expected '>' after generic type");
                } else {
                    break;
                }
            }

            if (resolutions.empty()) {
                return std::make_shared<TypeNode>(span_from_spans(start, id.get_span()), id.get_lexeme());
            }
            else if (is_generic) {
                return std::make_shared<GenericType>(span_from_spans(start, id.get_span()), id.get_lexeme(), resolutions);
            }
            else {
                resolutions.insert(resolutions.begin(),
                                   std::make_shared<TypeNode>(span_from_spans(start, id.get_span()), id.get_lexeme()));
                return std::make_shared<CompoundType>(span_from_spans(start, id.get_span()), resolutions);
            }

        } else if (match({TokenType::LEFT_SQ})) {
            auto ty = parse_type();
            consume(TokenType::RIGHT_SQ, previous().get_span(), "Expected ']' after expression");
            return std::make_shared<ListType>(span_from_spans(start, previous().get_span()), ty);
        }

        else if (match({TokenType::FUN})) {
            consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after function type");
            auto params = parse_type_list(TokenType::RIGHT_PAREN);
            consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after function type");

            auto return_t =  parse_return_type();
            return std::make_shared<FunctionType>(span_from_spans(start, previous().get_span()), params, return_t,
                                                  instanceof<ResultType>(return_t.get()));
        }

        else {
            throw ParserError("Expected type", peek().get_span());
        }
    }

    SharedTypeNode Parser::parse_return_type() {
        auto start = previous().get_span();

        if (match({TokenType::BANG})) {
            consume(TokenType::LESS, peek().get_span(), "Expected '<' after '!' in function type");
            auto ok = parse_type();
            consume(TokenType::COMMA, peek().get_span(), "Expected ',' after ok type in function type");
            auto err = parse_type();
            consume(TokenType::GREATER, peek().get_span(), "Expected '>' after error type in function type");

            return std::make_shared<ResultType> (span_from_spans(ok->get_span(), err->get_span()), ok, err);
        }

        return parse_type();
    }

    std::vector<SharedTypeNode> Parser::parse_type_list(TokenType end_token) {
        std::vector<SharedTypeNode> params;

        if (!check(end_token)) {
            do {
                params.push_back(parse_type());
            } while (match({TokenType::COMMA}));
        }

        return params;
    }

    std::vector<std::shared_ptr<Node>> Parser::expr_list(TokenType end_token) {
        std::vector<std::shared_ptr<Node>> exprs;

        if (!check(end_token)) {
            do {
                exprs.push_back(expression());
            } while (match({TokenType::COMMA}));
        }

        return exprs;
    }

    std::shared_ptr<Node> Parser::statement() {
        if (match({TokenType::IF})) return if_stmnt();
        else if (match({TokenType::WHILE})) return while_statement();
        else if (match({TokenType::LEFT_BRACE})) return block(true);
        else if (match({TokenType::FOR})) return for_statement();
        else if (match({TokenType::VAR})) return variable_declaration();
        else if (match({TokenType::FUN})) return closure_declaration(false);
        else if (match({TokenType::RETURN})) return return_statement();
        else if (match({TokenType::BREAK})) return break_statement();
        else if (match({TokenType::CONTINUE})) return continue_statement();
        else if (match({TokenType::OK})) return ok_statement();
        else if (match({TokenType::ERROR_t})) return err_statement();

        return expr_stmnt();
    }

    std::shared_ptr<Node> Parser::ok_statement() {
        if (!in_function) {
            throw ParserError( "Can not use Ok statement outside function", previous().get_span());
        }
        if (!is_error_func) {
            throw ParserError("Can not use Ok statement in non-error function\n  Hint: mark function as error with '!' eg 'fn foo()! {}'", previous().get_span());

        }
        auto expr = expression();
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in Ok statement");
        return std::make_shared<ResultStatement>(expr->get_span(), expr, false);
    }

    std::shared_ptr<Node> Parser::err_statement() {
        if (!in_function) {
            throw ParserError("Can not use err statement outside function", previous().get_span());
        }
        if (!is_error_func) {
            throw ParserError("Can not use ok statement in non-error function\n  Hint: mark function as error with '!' eg 'fn foo()! {}'", previous().get_span());

        }
        auto expr = expression();
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in err statement");
        return std::make_shared<ResultStatement>(expr->get_span(), expr, true);
    }

    std::shared_ptr<Node> Parser::break_statement() {
        auto keyword = previous();
        if (!m_in_loop) {
            throw ParserError( "Cannot break outside of a loop", keyword.get_span());
        }
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after break statement");
        return std::make_shared<Break>(keyword.get_span());
    }

    std::shared_ptr<Node> Parser::continue_statement() {
        auto keyword = previous();
        if (!m_in_loop) {
            throw ParserError("Cannot continue outside of a loop", keyword.get_span());
        }
        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after continue statement");
        return std::make_shared<Continue>(keyword.get_span());
    }


    std::shared_ptr<Node> Parser::await_statement() {
        if (match({TokenType::AWAIT})) {
            throw ParserError("Asyncio not implemented", previous().get_span());
            if (!m_in_async and in_function) {
                throw ParserError("Cannot await outside of an async function", previous().get_span());
            }
            auto await_expr = await_statement();
            auto span = span_from_spans(await_expr->get_span(), previous().get_span());
            return std::make_shared<Await>(span, await_expr);
        }

        return try_statement();
    }

    std::shared_ptr<Node> Parser::try_statement() {
        if (match({TokenType::TRY})) {
            if (!is_error_func and in_function) {
                throw ParserError("can not use try in non error function", previous().get_span());
            }
            auto try_expr = try_statement();
            return std::make_shared<Try>(span_from_spans(try_expr->get_span(), previous().get_span()), try_expr);

        }
        return _or();
    }

    std::shared_ptr<Node> Parser::closure_declaration(bool is_expression) {
        auto pre = is_error_func;
        is_error_func = false;
        auto function = dynamic_pointer_cast<FuncDef>(function_declaration(false, is_expression));
        is_error_func = pre;
        return std::make_shared<ClosureDef>(function->get_span(), function->get_name(),
                                            function, is_expression);

    }

    std::shared_ptr<Node> Parser::return_statement() {
        auto keyword = previous();

        if (is_error_func) {
            throw ParserError("Cannot return from error function, use ok or err instead", keyword.get_span());
        }

        if (!in_function) {
            throw ParserError("Cannot return outside of a function", keyword.get_span());
        }

        std::shared_ptr<Node> value = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            value = expression();
        }

        consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after return value");
        return std::make_shared<Return>(span_from_spans(keyword.get_span(), previous().get_span()), value);
    }

    std::shared_ptr<Node> Parser::for_statement() {
//  consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after for keyword");
//        consume(TokenType::VAR, peek().get_span(), "Expected 'var' after for keyword");
        m_in_loop = true;


        auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected variable name after for keyword");

        consume(TokenType::IN_t, peek().get_span(), "Expected 'in' after variable name");
        auto iterable = expression();

        m_scopes->new_scope();
        m_scopes->declare(id.get_lexeme(), id.get_span(), true);
//  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after for condition");
        m_scopes->end_scope();

        auto body = statement();
        m_in_loop = false;
        return std::make_shared<For>(span_from_spans(id.get_span(), previous().get_span()), id.get_lexeme(), iterable,
                                     body);
    }

    std::shared_ptr<Node> Parser::if_stmnt() {
//  consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after if keyword");
        auto condition = expression();
//  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after if condition");

        auto then_branch = statement();

        std::optional<std::shared_ptr<Node>> else_branch = std::nullopt;

        if (match({TokenType::ELSE})) {
            else_branch = statement();
        }

        return std::make_shared<If>(span_from_spans(condition->get_span(), previous().get_span()),
                                    condition,
                                    then_branch,
                                    else_branch);
    }

    std::shared_ptr<Node> Parser::block(bool create_scope = true) {

        if (create_scope) m_scopes->new_scope();
        std::vector<std::shared_ptr<Node>> statements;

        while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
            statements.push_back(statement());
        }

        consume(TokenType::RIGHT_BRACE, peek().get_span(), "Expected '}' after block");

        if (create_scope) m_scopes->end_scope();

        if (statements.empty()) return std::make_shared<Block>(peek().get_span(), statements);
        return std::make_shared<Block>(statements.front()->get_span(), statements);
    }

    std::shared_ptr<Node> Parser::while_statement() {
//  consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after while keyword");
        m_in_loop = true;
        auto condition = expression();
//  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after while condition");

        auto res = std::make_shared<While>(span_from_spans(condition->get_span(), previous().get_span()),
                                           condition,
                                           statement());
        m_in_loop = false;
        return res;
    }

    std::shared_ptr<Node> Parser::expr_stmnt() {
        auto expr = expression();
        consume(TokenType::SEMICOLON, expr->get_span(), "Expected semicolon, ';' after expression");
        return std::make_shared<ExprStmnt>(expr->get_span(), expr);
    }


    std::shared_ptr<Node> Parser::expression() {
        return assignment();
    }

    std::shared_ptr<Node> Parser::assignment() {
        auto expr = await_statement();

        if (match({TokenType::EQUAL})) {
            auto pre = previous();
            auto value = assignment();

            if (instanceof<Identifier>(expr.get())) {
                auto sp = span_from_spans(expr->get_span(), value->get_span());
                auto e = dynamic_cast<Identifier *>(expr.get());
                return std::make_shared<Assign>(sp, e->get_name(), value);
            } else if (instanceof<GetItem>(expr.get())) {
                auto sp = span_from_spans(expr->get_span(), value->get_span());
                auto e = dynamic_cast<GetItem *>(expr.get());
                return std::make_shared<SetItem>(sp, e->get_expr(), e->get_index(), value);
            } else if (instanceof<GetAttribute>(expr.get())) {
                auto sp = span_from_spans(expr->get_span(), value->get_span());
                auto e = dynamic_cast<GetAttribute *>(expr.get());
                return std::make_shared<SetAttribute>(sp, e->get_expr(), e->get_name(), value);
            } else if (instanceof<ListLiteral>(expr.get())) {
                auto e = dynamic_cast<ListLiteral *>(expr.get());
                auto list = e->get_nodes();

                std::vector<SharedNode> targets;

                for (auto &node: list) {
                    if (!instanceof<Identifier>(node.get())) {
                        throw ParserError("invalid assignment target, expected an identifier", node->get_span());
                    }
                    auto n = dynamic_cast<Identifier *>(node.get());

                    if (!m_scopes->is_declared(n->get_name())) {
                        throw ParserError(fmt::format("variable {} is not declared", n->get_name()), n->get_span());
                    }

                    targets.push_back(node);
                }

                auto sp = span_from_spans(expr->get_span(), value->get_span());
                return std::make_shared<StructuredAssign>(sp, targets, value);
            }

            throw ParserError("invalid assignment target", expr->get_span());
        }

        return expr;
    }


    std::shared_ptr<Node> Parser::_or() {
        auto expr = _and();

        while (match({TokenType::OR})) {
            auto op = previous();
            auto right = _and();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::_and() {
        auto expr = bit_or();

        while (match({TokenType::AND})) {
            auto op = previous();
            auto right = equality();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::bit_or() {
        auto expr = bit_xor();

        while (match({TokenType::BITWISE_OR})) {
            auto op = previous();
            auto right = bit_xor();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::bit_xor() {
        auto expr = bit_and();

        while (match({TokenType::BITWISE_XOR})) {
            auto op = previous();
            auto right = bit_and();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::bit_and() {
        auto expr = equality();

        while (match({TokenType::BITWISE_AND})) {
            auto op = previous();
            auto right = equality();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::equality() {
        auto expr = comparison();

        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
            auto op = previous();
            auto right = comparison();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
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
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
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
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
            expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::factor() {
        auto expr = unary();

        while (match({TokenType::SLASH, TokenType::STAR, TokenType::MOD})) {
            auto op = previous();
            auto right = unary();

            auto expr_span = expr->get_span();
            auto new_span =
                    std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end,
                                           expr_span->line);
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

        return call();
    }

//    std::shared_ptr<Node> Parser::get_item() {
//        auto expr = call();
//
//        while (true) {
//             else {
//                break;
//            }
//        }
//
//        return expr;
//    }

    std::shared_ptr<Node> Parser::call() {
        auto expr = primary();

        while (true) {
            if (match({TokenType::LEFT_SQ})) {
                auto index = expression();
                consume(TokenType::RIGHT_SQ, expr->get_span(), "Expected ']' after expression");
                expr = std::make_shared<GetItem>(expr->get_span(), expr, index);
            } else if (match({TokenType::LEFT_PAREN})) {
                expr = f_call(expr);
            } else if (match({TokenType::DOT})) {
                if (match({TokenType::IDENTIFIER})) {
                    expr = std::make_shared<GetAttribute>(span_from_spans(expr->get_span(), previous().get_span()), expr, previous().get_lexeme());
                }
                else if (match({TokenType::QUESTION})) {
                    expr = std::make_shared<Try>(span_from_spans(expr->get_span(), previous().get_span()), expr);
                }
                else {
                    throw ParserError("Expected property name or '?' after '.'.", peek().get_span());
                }
            } else {
                break;
            }
        }

        return expr;
    }

    std::shared_ptr<Node> Parser::f_call(const std::shared_ptr<Node> &callee) {
        std::vector<std::shared_ptr<Node>> arguments;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 1024) {
                    throw ParserError("Cannot have more than 1024 arguments.", peek().get_span());
                }
                arguments.push_back(expression());
            } while (match({TokenType::COMMA}));
        }

        auto paren = consume(TokenType::RIGHT_PAREN, callee->get_span(), "Expected ')' after arguments.");

        if (instanceof<GetAttribute>(callee.get())) {
            return std::make_shared<CallMethod>(span_from_spans(callee->get_span(), paren.get_span()),
                                                std::dynamic_pointer_cast<GetAttribute>(callee), arguments);
        } else {
            return std::make_shared<Call>(span_from_spans(callee->get_span(), paren.get_span()), callee, arguments);
        }
    }

    std::vector<std::pair<SharedNode, SharedNode>> Parser::key_value_pairs(TokenType end_token) {
        std::vector<std::pair<SharedNode, SharedNode>> pairs;

        if (!check(end_token)) {
            do {
                auto key = expression();
                consume(TokenType::COLON, key->get_span(), "Expected ':' after key");
                auto value = expression();
                pairs.emplace_back(key, value);
            } while (match({TokenType::COMMA}));
        }

        return pairs;
    }

    std::shared_ptr<Node> Parser::primary() {
        if (match({TokenType::FALSE_t})) return std::make_shared<FalseLiteral>(previous().get_span());
        if (match({TokenType::TRUE_t})) return std::make_shared<TrueLiteral>(previous().get_span());
        if (match({TokenType::NIL})) return std::make_shared<NilLiteral>(previous().get_span());

        if (match({TokenType::INTEGER}))
            return std::make_shared<NumberLiteral>(previous().get_span(), previous().get_lexeme(), true);
        if (match({TokenType::FLOAT}))
            return std::make_shared<NumberLiteral>(previous().get_span(), previous().get_lexeme(), false);
        if (match({TokenType::STRING}))
            return std::make_shared<StringLiteral>(previous().get_span(), previous().get_lexeme());

        if (match({TokenType::IDENTIFIER}))
            return std::make_shared<Identifier>(previous().get_span(), previous().get_lexeme());
        if (match({TokenType::LEFT_PAREN})) {
            auto expr = expression();
            consume(TokenType::RIGHT_PAREN, expr->get_span(), "Expected ')' after expression.");
            return expr;
        }
        if (match({TokenType::LEFT_SQ})) {
            auto start = previous().get_span();
            auto exprs = expr_list(TokenType::RIGHT_SQ);
            consume(TokenType::RIGHT_SQ, previous().get_span(), "Expected ']' after expression.");
            return std::make_shared<ListLiteral>(span_from_spans(start, previous().get_span()), exprs);
        }

        if (match({TokenType::LEFT_BRACE})) {
            auto start = previous().get_span();
            auto pairs = key_value_pairs(TokenType::RIGHT_BRACE);
            consume(TokenType::RIGHT_BRACE, previous().get_span(), "Expected '}' after expression.");
            return std::make_shared<DictLiteral>(span_from_spans(start, previous().get_span()), pairs);
        }

        if (match({TokenType::FUN})) {
            return closure_declaration(true);
        }

        throw ParserError("Expected expression.", peek().get_span());
    }

    Token Parser::consume(TokenType type, const SharedSpan &span, const std::string &message) {
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

    Parser::Parser(const std::vector<Token> &tokens, Context *context) : m_tokens(tokens), m_scopes(nullptr) {
        ctx = context;
        m_scopes = new Scopes(context);
    }

    void Parser::set_scopes(Scopes *scopes) {
        if (m_managed_scope) delete m_scopes;
        m_managed_scope = true;
        m_scopes = scopes;
    }

    void Scopes::new_scope() {
        m_scopes.emplace_back();
    }

    void Scopes::end_scope() {
        m_scopes.pop_back();
    }

    void Scopes::declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut) {
        if (m_scopes.empty()) return;

        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.find(name) != scope.end()) {
                throw ParserError("Variable with this name already declared in this scope.", span);
            }
        }

        m_scopes[m_scopes.size() - 1][name] = std::make_shared<Variable>(name, span, m_scopes.size() == 1, is_mut);
    }

    bool Scopes::is_declared(const std::string &name) {
        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.find(name) != scope.end()) {
                return true;
            }
        }

        return false;
    }

    std::optional<std::shared_ptr<Variable>> Scopes::get(const std::string &name) {
        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.contains(name)) {
                return scope[name];
            }
        }

        return std::nullopt;
    }

    void Scopes::declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut, bool is_global) {
        if (m_scopes.empty()) return;

        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.find(name) != scope.end()) {
                auto sp = scope[name]->span;
                throw ParserError("Variable with this name already declared in this scope.", span);
                throw ParserError("Note, Variable is declared here", sp);
            }
        }

        m_scopes[m_scopes.size() - 1][name] = std::make_shared<Variable>(name, span, is_global, is_mut);
    }

    bool Scopes::is_declared_in_current_scope(const std::string &name) {
        if (m_scopes.empty()) return false;
        return m_scopes[m_scopes.size() - 1].find(name) != m_scopes[m_scopes.size() - 1].end();
    }

    bool Scopes::is_global(const std::string &name) {
        if (m_scopes.empty()) return false;
        return m_scopes[0].contains(name);
    }

    void Scopes::print() {
        size_t i = 0;
        for (auto &scope: m_scopes) {
            std::cout << "Scope " << i++ << ":\n";
            for (auto &[name, var]: scope) {
                std::cout << "    " << name << " " << var->is_global << " " << var->is_mut << "\n";
            }
        }
    }

} // bond