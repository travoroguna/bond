//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "ast.h"
#include <memory>


namespace bond {
    BinaryOp::BinaryOp(SharedSpan &span, SharedNode &left, Token op, SharedNode &right) : m_op(std::move(op)) {
        m_span = span;
        m_left = left;
        m_right = right;
        m_type = NodeType::BinOp;
    }

    void BinaryOp::accept(NodeVisitor *visitor) {
        visitor->visit_bin_op(this);
    }

    Unary::Unary(const SharedSpan& span, Token op, const SharedNode& expr) : m_op(std::move(op)) {
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

    Print::Print(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Print::accept(NodeVisitor *visitor) {
        visitor->visit_print(this);
    }

    ExprStmnt::ExprStmnt(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void ExprStmnt::accept(NodeVisitor *visitor) {
        visitor->visit_expr_stmnt(this);
    }

    NewVar::NewVar(const SharedSpan &span, const std::string &name, const SharedNode &expr) {
        m_span = span;
        m_name = name;
        m_expr = expr;
    }

    void NewVar::accept(NodeVisitor *visitor) {
        visitor->visit_new_var(this);
    }

    Identifier::Identifier(const SharedSpan &span, const std::string &name) {
        m_span = span;
        m_name = name;
    }

    void Identifier::accept(NodeVisitor *visitor) {
        visitor->visit_identifier(this);
    }

    Assign::Assign(const SharedSpan &span, const std::string &name, const SharedNode &node) {
        m_span = span;
        m_name = name;
        m_node = node;
    }

    void Assign::accept(NodeVisitor *visitor) {
        visitor->visit_assign(this);
    }

    Block::Block(const SharedSpan &span, const std::vector<SharedNode> &nodes) {
        m_span = span;
        m_nodes = nodes;
    }

    void Block::accept(NodeVisitor *visitor) {
        visitor->visit_block(this);
    }

    List::List(const SharedSpan &span, const std::vector<SharedNode> &nodes) {
        m_span = span;
        m_nodes = nodes;
    }

    void List::accept(NodeVisitor *visitor) {
        visitor->visit_list(this);
    }

    GetItem::GetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index) {
        m_span = span;
        m_expr = expr;
        m_index = index;
    }

    void GetItem::accept(NodeVisitor *visitor) {
        visitor->visit_get_item(this);
    }

    SetItem::SetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index, const SharedNode &value) {
        m_span = span;
        m_expr = expr;
        m_index = index;
        m_value = value;
    }

    void SetItem::accept(NodeVisitor *visitor) {
        visitor->visit_set_item(this);
    }
};