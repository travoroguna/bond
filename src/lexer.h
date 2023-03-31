//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include "context.h"
#include "span.h"


namespace bond {


    enum class TokenType : int {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
        LEFT_SQ, RIGHT_SQ,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

        EndOfFile
    };

//    std::ostream &operator<<(std::ostream& os, TokenType t) {
//        return os << static_cast<int>(t);
//    }

    class Token {
    public:
        Token(std::shared_ptr<Span> span, TokenType type, std::string lexeme);
        std::string as_string();
        TokenType get_type() { return m_type; }
        std::shared_ptr<Span> get_span() { return m_span; }
        std::string get_lexeme() { return m_lexeme; };


    private:
        std::shared_ptr<Span> m_span;
        TokenType m_type;
        std::string m_lexeme;

    };

    class Lexer {
    private:
        std::shared_ptr<Span> make_span();
        bool is_at_end();
        void make_token();


        std::string m_source;
        std::vector<Token> m_tokens = {};
        uint32_t m_start, m_current, m_module, m_line;
        Context *m_context;

    public:
        Lexer(std::string source, Context *context, uint32_t module_id);
        std::vector<Token>& tokenize();

        char advance();

        void new_token(TokenType type);

        bool match(char c);

        char peek();

        void make_string();

        void new_token(TokenType type, std::string &lexeme);

        void make_number();

        char peek_next();

        void make_identifier();
    };

} // bond
