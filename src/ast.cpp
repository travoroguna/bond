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
        m_type = NodeType::BinOp;
    }

    void BinaryOp::accept(NodeVisitor *visitor) {
        visitor->visit_bin_op(this);
    }

    Unary::Unary(const SharedSpan& span, Token op, const SharedExpr& expr) : m_op(std::move(op)) {
        m_span = span;
        m_expr = expr;
        m_type = NodeType::Unary;
    }

    void Unary::accept(NodeVisitor *visitor) {
        visitor->visit_unary(this);
    }

    FalseLiteral::FalseLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::FalseLit;
    }

    void FalseLiteral::accept(NodeVisitor *visitor) {
        visitor->visit_false_lit(this);
    }

    TrueLiteral::TrueLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::TrueLit;
    }

    void TrueLiteral::accept(NodeVisitor *visitor) {
        visitor->visit_true_lit(this);
    }

    NilLiteral::NilLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::NilLit;
    }

    void NilLiteral::accept(NodeVisitor *visitor) {
        visitor->visit_nil_lit(this);
    }

    NumberLiteral::NumberLiteral(const SharedSpan &span, const std::string &lexeme) {
        m_span = span;
        m_value = std::stof(lexeme);
        m_type = NodeType::NumLit;
    }

    void NumberLiteral::accept(NodeVisitor *visitor) {
        visitor->visit_num_lit(this);
    }

    StringLiteral::StringLiteral(const SharedSpan &span, const std::string &lexeme) {
        m_span = span;
        m_value = lexeme;
        m_type = NodeType::StringLit;
    }

    void StringLiteral::accept(NodeVisitor *visitor) {
        visitor->visit_string_lit(this);
    }
};