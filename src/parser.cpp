//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "parser.h"
#include <initializer_list>
#include <algorithm>
#include <exception>
#include <optional>
#include <ranges>
#include "gc.h"

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

std::vector<std::shared_ptr<Node>> Parser::parse() {
  std::vector<std::shared_ptr<Node>> nodes;
  m_scopes.new_scope();

  m_scopes.declare("println", std::make_shared<Span>(0, 0, 0, 1), false);

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
      case TokenType::RETURN:return;
    }

    advance();
  }
}

std::optional<std::shared_ptr<Node>> Parser::declaration() {
  try {
    if (match({TokenType::VAR})) return variable_declaration();
    else if (match({TokenType::FUN})) return function_declaration();
    auto st = statement();
//            m_scopes.end_scope();
    return st;
  }
  catch (ParserError &e) {
    ctx->error(e.span, e.error);
    synchronize();
    return std::nullopt;
  }
}

std::shared_ptr<Span> Parser::span_from_spans(const std::shared_ptr<Span> &start, const std::shared_ptr<Span> &end) {
  return std::make_shared<Span>(start->module_id, start->start, end->end, end->line);
}

std::shared_ptr<Node> Parser::function_declaration() {
  bool pre = in_function;
  in_function = true;
  auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected function name after fun keyword");

  if (m_scopes.is_declared(id.get_lexeme())) {
    ctx->error(id.get_span(), fmt::format("Function {} is already declared in this scope", id.get_lexeme()));
    auto sp = m_scopes.get(id.get_lexeme()).value()->span;
    throw ParserError(fmt::format("Note Function {} is already declared here", id.get_lexeme()), sp);
  }

  m_scopes.declare(id.get_lexeme(), id.get_span(), false);

  consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after function name");

  std::vector<std::pair<std::string, SharedSpan>> params;

  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (params.size() >= 1024) {
        ctx->error(peek().get_span(), "Cannot have more than 1024 parameters");
      }
      auto param = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected parameter name");
      params.emplace_back(param.get_lexeme(), param.get_span());
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after parameters");

  consume(TokenType::LEFT_BRACE, peek().get_span(), "Expected '{' before function body");

  m_scopes.new_scope();
  for (auto &param : params) {
    m_scopes.declare(param.first, param.second, true);
  }

  auto body = block(false);
  m_scopes.end_scope();
  in_function = pre;
  return std::make_shared<FuncDef>(span_from_spans(id.get_span(), previous().get_span()),
                                   id.get_lexeme(),
                                   params,
                                   body);
}

std::shared_ptr<Node> Parser::variable_declaration() {
  auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected variable name after var keyword");

  if (m_scopes.is_declared(id.get_lexeme())) {
    ctx->error(id.get_span(), fmt::format("Variable {} is already declared in this scope", id.get_lexeme()));
    auto sp = m_scopes.get(id.get_lexeme()).value()->span;
    throw ParserError(fmt::format("Note Variable {} is already declared here", id.get_lexeme()), sp);
  }

  consume(TokenType::EQUAL, peek().get_span(), "Expected '=' after variable name");
  auto initializer = expression();
  consume(TokenType::SEMICOLON, peek().get_span(), "Expected ';' after expression in variable declaration");

  m_scopes.declare(id.get_lexeme(), id.get_span(), true);

  return std::make_shared<NewVar>(span_from_spans(id.get_span(), previous().get_span()), id.get_lexeme(),
                                  initializer);
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
  else if (match({TokenType::PRINT})) return print_stmnt();
  else if (match({TokenType::WHILE})) return while_statement();
  else if (match({TokenType::LEFT_BRACE})) return block(true);
  else if (match({TokenType::FOR})) return for_statement();
  else if (match({TokenType::VAR})) return variable_declaration();
  else if (match({TokenType::FUN})) return closure_declaration();
  else if (match({TokenType::RETURN})) return return_statement();
  return expr_stmnt();
}

std::shared_ptr<Node> Parser::closure_declaration() {
  auto function = dynamic_cast<FuncDef *>(function_declaration().get());
  return std::make_shared<ClosureDef>(function->get_span(), function->get_name(), std::make_shared<FuncDef>(*function));
}

std::shared_ptr<Node> Parser::return_statement() {
  auto keyword = previous();

  if (!in_function) {
    ctx->error(keyword.get_span(), "Cannot return outside of a function");
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
  consume(TokenType::VAR, peek().get_span(), "Expected 'var' after for keyword");
  auto id = consume(TokenType::IDENTIFIER, peek().get_span(), "Expected variable name after var keyword");

  consume(TokenType::IN, peek().get_span(), "Expected 'in' after variable name");
  auto iterable = expression();

  m_scopes.new_scope();
  m_scopes.declare(id.get_lexeme(), id.get_span(), true);
//  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after for condition");
  m_scopes.end_scope();

  auto body = statement();

  return std::make_shared<For>(span_from_spans(id.get_span(), previous().get_span()), id.get_lexeme(), iterable, body);
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
  if (create_scope) m_scopes.new_scope();
  std::vector<std::shared_ptr<Node>> statements;

  while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
    statements.push_back(statement());
  }

  consume(TokenType::RIGHT_BRACE, peek().get_span(), "Expected '}' after block");

  if (create_scope) m_scopes.end_scope();

  if (statements.empty()) return std::make_shared<Block>(peek().get_span(), statements);
  return std::make_shared<Block>(statements.front()->get_span(), statements);
}

std::shared_ptr<Node> Parser::while_statement() {
//  consume(TokenType::LEFT_PAREN, peek().get_span(), "Expected '(' after while keyword");
  auto condition = expression();
//  consume(TokenType::RIGHT_PAREN, peek().get_span(), "Expected ')' after while condition");

  return std::make_shared<While>(span_from_spans(condition->get_span(), previous().get_span()),
                                 condition,
                                 statement());
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
  return assignment();
}

std::shared_ptr<Node> Parser::assignment() {
  auto expr = _or();

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
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
    expr = std::make_shared<BinaryOp>(new_span, expr, op, right);
  }

  return expr;
}

std::shared_ptr<Node> Parser::_and() {
  auto expr = equality();

  while (match({TokenType::AND})) {
    auto op = previous();
    auto right = equality();

    auto expr_span = expr->get_span();
    auto new_span =
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
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
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
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
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
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
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
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
    auto new_span =
        std::make_shared<Span>(expr_span->module_id, expr_span->start, right->get_span()->end, expr_span->line);
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

  return get_item();
}

std::shared_ptr<Node> Parser::get_item() {
  auto expr = call();

  while (true) {
    if (match({TokenType::LEFT_SQ})) {
      auto index = expression();
      consume(TokenType::RIGHT_SQ, expr->get_span(), "Expected ']' after expression");
      expr = std::make_shared<GetItem>(expr->get_span(), expr, index);
    } else {
      break;
    }
  }

  return expr;
}

std::shared_ptr<Node> Parser::call() {
  auto expr = primary();

  while (true) {
    if (match({TokenType::LEFT_PAREN})) {
      expr = f_call(expr);
    } else {
      break;
    }
  }

  return expr;
}

std::shared_ptr<Node> Parser::f_call(std::shared_ptr<Node> callee) {
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
  fmt::print("arguments: {}\n", arguments.size());
  return std::make_shared<Call>(span_from_spans(callee->get_span(), paren.get_span()), callee, arguments);
}

std::shared_ptr<Node> Parser::primary() {
  if (match({TokenType::FALSE})) return std::make_shared<FalseLiteral>(previous().get_span());
  if (match({TokenType::TRUE})) return std::make_shared<TrueLiteral>(previous().get_span());
  if (match({TokenType::NIL})) return std::make_shared<NilLiteral>(previous().get_span());

  if (match({TokenType::NUMBER}))
    return std::make_shared<NumberLiteral>(previous().get_span(), previous().get_lexeme());
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
    return std::make_shared<List>(span_from_spans(start, previous().get_span()), exprs);
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

void Scopes::new_scope() {
  m_scopes.emplace_back();
}

void Scopes::end_scope() {
  m_scopes.pop_back();
}

void Scopes::declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut) {
  if (m_scopes.empty()) return;

  for (auto &scope : std::ranges::reverse_view(m_scopes)) {
    if (scope.find(name) != scope.end()) {
      throw ParserError("Variable with this name already declared in this scope.", span);
    }
  }

  m_scopes[m_scopes.size() - 1][name] = std::make_shared<Variable>(name, span, m_scopes.size() == 1, is_mut);
}

bool Scopes::is_declared(const std::string &name) {
  for (auto &scope : std::ranges::reverse_view(m_scopes)) {
    if (scope.find(name) != scope.end()) {
      return true;
    }
  }

  return false;
}

std::optional<std::shared_ptr<Variable>> Scopes::get(const std::string &name) {
  for (auto &scope : std::ranges::reverse_view(m_scopes)) {
    if (scope.contains(name)) {
      return scope[name];
    }
  }

  return std::nullopt;
}

void Scopes::declare(const std::string &name, const std::shared_ptr<Span> &span, bool is_mut, bool is_global) {
  if (m_scopes.empty()) return;

  for (auto &scope : std::ranges::reverse_view(m_scopes)) {
    if (scope.find(name) != scope.end()) {
      throw ParserError("Variable with this name already declared in this scope.", span);
    }
  }

  m_scopes[m_scopes.size() - 1][name] = std::make_shared<Variable>(name, span, is_global, is_mut);
}
} // bond