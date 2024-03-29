//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "lexer.h"

#include <utility>
#include "fmt/core.h"

namespace bond {

    std::unordered_map<std::string, TokenType> keywords = {
            {"and",      TokenType::AND},
            {"struct",   TokenType::STRUCT},
            {"else",     TokenType::ELSE},
            {"false",    TokenType::FALSE_t},
            {"for",      TokenType::FOR},
            {"fn",       TokenType::FUN},
            {"if",       TokenType::IF},
            {"nil",      TokenType::NIL},
            {"or",       TokenType::OR},
            {"return",   TokenType::RETURN},
            {"import",   TokenType::IMPORT},
            {"as",       TokenType::AS},
            {"true",     TokenType::TRUE_t},
            {"var",      TokenType::VAR},
            {"while",    TokenType::WHILE},
            {"in",       TokenType::IN_t},
            {"try",      TokenType::TRY},
            {"break",    TokenType::BREAK},
            {"continue", TokenType::CONTINUE},
            {"async",    TokenType::ASYNC},
            {"await",    TokenType::AWAIT},
            {"ok", TokenType::OK},
            {"err", TokenType::ERROR_t}

    };

    Span::Span(uint32_t module_id, uint32_t start, uint32_t end, uint32_t line) {
        this->module_id = module_id;
        this->start = start;
        this->end = end;
        this->line = line;
    }

    Token::Token(std::shared_ptr<Span> span, TokenType type, std::string lexeme) {
        m_span = std::move(span);
        m_type = type;
        m_lexeme = std::move(lexeme);
    }

    std::string Token::as_string() {
        return fmt::format("[{}] ({})", static_cast<int>(m_type), m_lexeme);
    }

    Lexer::Lexer(std::string source, Context *context, uint32_t module_id) {
        m_source = std::move(source);
        m_start = 0;
        m_current = 0;
        m_line = 1;
        m_module = module_id;
        m_context = context;
    }

    std::vector<Token> &Lexer::tokenize() {
        while (!is_at_end()) {
            m_start = m_current;
            make_token();
        }

        m_start = m_current;

        m_tokens.emplace_back(
                Token(make_span(),
                      TokenType::EndOfFile,
                      m_source.substr(m_start, m_current)));
        return m_tokens;
    }

    std::shared_ptr<Span> Lexer::make_span() {
        return std::make_shared<Span>(m_module, m_start, m_current, m_line);
    }

    bool Lexer::is_at_end() {
        return m_current >= m_source.size();
    }

    char Lexer::advance() {
        return m_source[m_current++];
    }

    bool Lexer::match(char character) {
        if (is_at_end()) return false;

        if (m_source[m_current] != character) return false;
        m_current++;
        return true;
    }

    char Lexer::peek() {
        if (is_at_end()) return 0;
        return m_source[m_current];
    }

    void Lexer::new_token(TokenType type) {
        m_tokens.emplace_back(
                make_span(), type,
                m_source.substr(m_start, m_current - m_start));
    }

    void Lexer::new_token(TokenType type, std::string &lexeme) {
        m_tokens.emplace_back(make_span(), type, lexeme);
    }

    bool is_digit(char c) {
        return c >= '0' && c <= '9';
    }

    void Lexer::make_string() {
        bool isEscaped = false;

        while (peek() != '"' || isEscaped) {
            if (peek() == '\n' && !isEscaped) m_line++;

            if (peek() == '\\' && !isEscaped)
            {
                isEscaped = true;
            } else {
                isEscaped = false;
            }

            if (is_at_end()) {
                report("Unterminated string.", make_span());
                return;
            }
            advance();
        }

        // The closing '"'.
        if (isEscaped) {
            report("Unterminated escape character.", make_span());
            return;
        }
        advance();

        // Trim the surrounding quotes.
        auto value = m_source.substr(m_start + 1, m_current - m_start - 2);
        replaceEscapedSequences(value);

        new_token(TokenType::STRING, value);
    }

    std::unordered_map<char, char> replacement = {
            {'n', '\n'},
            {'t', '\t'},
            {'r', '\r'},
            {'0', '\0'},
            {'\\', '\\'},
            {'\'', '\''},
            {'"', '"'}
    };
    void Lexer::replaceEscapedSequences(std::string& str) {
        size_t position = 0;

        for (auto [c, rep]: replacement) {
            while ((position = str.find(fmt::format("\\{}", c), position)) != std::string::npos) {
                str.replace(position, 2, fmt::format("{}", rep));
                position += 1;
            }
            position = 0;
        }
    }

    bool is_alpha(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    bool is_alpha_numeric(char c) {
        return is_alpha(c) || is_digit(c);
    }

    char Lexer::peek_next() {
        if (m_current + 1 >= m_source.length()) return '\0';
        return m_source[m_current + 1];
    }

    void Lexer::make_number() {
        while (is_digit(peek())) advance();

        if (peek() == '.' && is_digit(peek_next())) {
            advance();

            while (is_digit(peek())) advance();
        }

        auto l = m_source.substr(m_start, m_current - m_start);

        if (l.find('.') != std::string::npos) return new_token(TokenType::FLOAT, l);

        new_token(TokenType::INTEGER, l);
    }

    void Lexer::make_identifier() {
        while (is_alpha_numeric(peek())) advance();

        auto id = m_source.substr(m_start, m_current - m_start);
        if (keywords.contains(id)) {
            new_token(keywords[id]);
        } else {
            new_token(TokenType::IDENTIFIER);
        }
    }

    void Lexer::make_token() {
        auto c = advance();

#define ADD_TOKEN(X, T) case X: new_token((T)); break
#define ADD_IF(X0, X1, T0, T1) case X0: new_token(match(X1) ? T1 : T0); break
        switch (c) {
            ADD_TOKEN('(', TokenType::LEFT_PAREN);
            ADD_TOKEN(')', TokenType::RIGHT_PAREN);
            ADD_TOKEN('{', TokenType::LEFT_BRACE);
            ADD_TOKEN('}', TokenType::RIGHT_BRACE);
            ADD_TOKEN(',', TokenType::COMMA);
            ADD_TOKEN('.', TokenType::DOT);
            ADD_TOKEN('-', TokenType::MINUS);
            ADD_TOKEN('+', TokenType::PLUS);
            ADD_TOKEN('%', TokenType::MOD);
            ADD_TOKEN(';', TokenType::SEMICOLON);
            ADD_TOKEN('*', TokenType::STAR);
            ADD_TOKEN('[', TokenType::LEFT_SQ);
            ADD_TOKEN(']', TokenType::RIGHT_SQ);
            ADD_TOKEN('|', TokenType::BITWISE_OR);
            ADD_TOKEN('&', TokenType::BITWISE_AND);
            ADD_TOKEN('^', TokenType::BITWISE_XOR);
            ADD_TOKEN(':', TokenType::COLON);
            ADD_TOKEN('?', TokenType::QUESTION);
            ADD_IF('!', '=', TokenType::BANG, TokenType::BANG_EQUAL);
            ADD_IF('=', '=', TokenType::EQUAL, TokenType::EQUAL_EQUAL);
            ADD_IF('<', '=', TokenType::LESS, TokenType::LESS_EQUAL);
            ADD_IF('>', '=', TokenType::GREATER, TokenType::GREATER_EQUAL);

            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !is_at_end()) advance();
                } else {
                    new_token(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                m_line++;
                break;

            case '"':
                make_string();
                break;

            default:
                if (is_digit(c)) {
                    make_number();
                } else if (is_alpha(c)) {
                    make_identifier();
                } else {
                    auto span = make_span();
                    report("unexpected character", span);
                }
        }
#undef ADD_TOKEN
    }

    void Lexer::report(const std::string &message, const std::shared_ptr<Span> &span) {
        if (m_report) {
            m_context->error(span, message);
        }
        else {
            m_error_spans.emplace_back(message, span);
        }
    }
} // bond