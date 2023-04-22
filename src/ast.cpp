//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "ast.h"
#include <memory>
#include <utility>

namespace bond {
    BinaryOp::BinaryOp(SharedSpan &span, SharedNode &left, Token op, SharedNode &right)
            : m_op(std::move(op)) {
        m_span = span;
        m_left = left;
        m_right = right;
        m_type = NodeType::BinOp;
    }

    void BinaryOp::accept(NodeVisitor *visitor) {
        visitor->visit_bin_op(this);
    }

    Unary::Unary(const SharedSpan &span, Token op, const SharedNode &expr)
            : m_op(std::move(op)) {
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

    NumberLiteral::NumberLiteral(const SharedSpan &span, const std::string &lexeme, bool is_int) {
        m_span = span;
        m_value = lexeme;
        m_type = NodeType::NumLit;
        m_is_int = is_int;
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

    If::If(const SharedSpan &span,
           const SharedNode &condition,
           const SharedNode &then,
           std::optional<SharedNode> else_node) {
        m_span = span;
        m_condition = condition;
        m_then = then;
        m_else = std::move(else_node);
    }

    void If::accept(NodeVisitor *visitor) {
        visitor->visit_if(this);
    }

    While::While(const SharedSpan &span, const SharedNode &condition, const SharedNode &statement) {
        m_span = span;
        m_condition = condition;
        m_statement = statement;
    }

    void While::accept(NodeVisitor *visitor) {
        visitor->visit_while(this);
    }

    Call::Call(const SharedSpan &span, const SharedNode &expr, const std::vector<SharedNode> &args) {
        m_span = span;
        m_expr = expr;
        m_args = args;
    }

    void Call::accept(NodeVisitor *visitor) {
        visitor->visit_call(this);
    }

    For::For(const SharedSpan &span, const std::string &name, const SharedNode &expr, const SharedNode &statement) {
        m_span = span;
        m_name = name;
        m_expr = expr;
        m_statement = statement;
    }

    void For::accept(NodeVisitor *visitor) {
        visitor->visit_for(this);
    }

    FuncDef::FuncDef(const SharedSpan &span,
                     const std::string &name,
                     const std::vector<std::pair<std::string, SharedSpan>> &params,
                     const SharedNode &body, bool can_error) {
        m_span = span;
        m_name = name;
        m_params = params;
        m_body = body;
        m_can_error = can_error;
    }

    void FuncDef::accept(NodeVisitor *visitor) {
        visitor->visit_func_def(this);
    }

    Return::Return(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Return::accept(NodeVisitor *visitor) {
        visitor->visit_return(this);
    }

    ClosureDef::ClosureDef(const SharedSpan &span, const std::string &name, const std::shared_ptr<FuncDef> &func_def) {
        m_span = span;
        m_name = name;
        m_func_def = func_def;
    }

    void ClosureDef::accept(NodeVisitor *visitor) {
        visitor->visit_closure_def(this);
    }

    StructNode::StructNode(const SharedSpan &span,
                           const std::string &name,
                           const std::vector<std::string> &params,
                           const std::vector<SharedNode> &methods) {
        m_span = span;
        m_name = name;
        m_params = params;
        m_methods = methods;
    }

    void StructNode::accept(NodeVisitor *visitor) {
        visitor->visit_struct(this);
    };

    GetAttribute::GetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name) {
        m_span = span;
        m_expr = expr;
        m_name = name;
    }

    void GetAttribute::accept(NodeVisitor *visitor) {
        visitor->visit_get_attribute(this);
    }

    SetAttribute::SetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name,
                               const SharedNode &value) {
        m_span = span;
        m_expr = expr;
        m_name = name;
        m_value = value;
    }

    void SetAttribute::accept(NodeVisitor *visitor) {
        visitor->visit_set_attribute(this);
    }

    ImportDef::ImportDef(const SharedSpan &span, const std::string &name, const std::string &alias) {
        m_span = span;
        m_name = name;
        m_alias = alias;
    }

    void ImportDef::accept(NodeVisitor *visitor) {
        visitor->visit_import(this);
    }

    Try::Try(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Try::accept(NodeVisitor *visitor) {
        visitor->visit_try(this);
    }


    Break::Break(const SharedSpan &span) {
        m_span = span;
    }

    void Break::accept(NodeVisitor *visitor) {
        visitor->visit_break(this);
    }

    Continue::Continue(const SharedSpan &span) {
        m_span = span;
    }

    void Continue::accept(NodeVisitor *visitor) {
        visitor->visit_continue(this);
    }
};