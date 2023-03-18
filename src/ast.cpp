//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "ast.h"
#include <memory>


namespace bond {
    BinaryOp::BinaryOp(SharedSpan &span, SharedExpr &left, Token op, SharedExpr &right) : m_op(std::move(op)) {
        m_span = span;
        m_left = left;
        m_right = right;
    }

    Unary::Unary(const SharedSpan& span, Token op, const SharedExpr& expr) : m_op(std::move(op)) {
        m_span = span;
        m_expr = expr;
    }

    FalseLiteral::FalseLiteral(const SharedSpan &span) {
        m_span = span;
    }

    TrueLiteral::TrueLiteral(const SharedSpan &span) {
        m_span = span;
    }

    NilLiteral::NilLiteral(const SharedSpan &span) {
        m_span = span;
    }


    NumberLiteral::NumberLiteral(const SharedSpan &span, const std::string &lexeme) {
        m_span = span;
        m_value = std::stof(lexeme);
    }

    StringLiteral::StringLiteral(const SharedSpan &span, const std::string &lexeme) {
        m_span = span;
        m_value = lexeme;
    }
};