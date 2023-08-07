//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include "ast.h"
#include <memory>
#include <utility>

namespace bond {
    class NodeVisitor;

    BinaryOp::BinaryOp(SharedSpan &span, SharedNode &left, Token op, SharedNode &right)
            : m_op(std::move(op)) {
        m_span = span;
        m_left = left;
        m_right = right;
        m_type = NodeType::BinOp;
    }

    void BinaryOp::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Unary::Unary(const SharedSpan &span, Token op, const SharedNode &expr)
            : m_op(std::move(op)) {
        m_span = span;
        m_expr = expr;
        m_type = NodeType::Unary;
    }

    void Unary::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    FalseLiteral::FalseLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::FalseLit;
    }

    void FalseLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    TrueLiteral::TrueLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::TrueLit;
    }

    void TrueLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    NilLiteral::NilLiteral(const SharedSpan &span) {
        m_span = span;
        m_type = NodeType::NilLit;
    }

    void NilLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    NumberLiteral::NumberLiteral(const SharedSpan &span, const std::string &lexeme, bool is_int) {
        m_span = span;
        m_value = lexeme;
        m_type = NodeType::NumLit;
        m_is_int = is_int;
    }

    void NumberLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    StringLiteral::StringLiteral(const SharedSpan &span, const std::string &lexeme) {
        m_span = span;
        m_value = lexeme;
        m_type = NodeType::StringLit;
    }

    void StringLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }


    ExprStmnt::ExprStmnt(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void ExprStmnt::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    NewVar::NewVar(const SharedSpan &span, const std::string &name, const SharedNode &expr, std::optional<SharedTypeNode> type){
        m_span = span;
        m_name = name;
        m_expr = expr;
        m_type = type;
    }

    void NewVar::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Identifier::Identifier(const SharedSpan &span, const std::string &name) {
        m_span = span;
        m_name = name;
    }

    void Identifier::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Assign::Assign(const SharedSpan &span, const std::string &name, const SharedNode &node) {
        m_span = span;
        m_name = name;
        m_node = node;
    }

    void Assign::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Block::Block(const SharedSpan &span, const std::vector<SharedNode> &nodes) {
        m_span = span;
        m_nodes = nodes;
    }

    void Block::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    ListLiteral::ListLiteral(const SharedSpan &span, const std::vector<SharedNode> &nodes) {
        m_span = span;
        m_nodes = nodes;
    }

    void ListLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    GetItem::GetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index) {
        m_span = span;
        m_expr = expr;
        m_index = index;
    }

    void GetItem::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    SetItem::SetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index, const SharedNode &value) {
        m_span = span;
        m_expr = expr;
        m_index = index;
        m_value = value;
    }

    void SetItem::accept(NodeVisitor *visitor) {
        visitor->visit(this);
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
        visitor->visit(this);
    }

    While::While(const SharedSpan &span, const SharedNode &condition, const SharedNode &statement) {
        m_span = span;
        m_condition = condition;
        m_statement = statement;
    }

    void While::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Call::Call(const SharedSpan &span, const SharedNode &expr, const std::vector<SharedNode> &args) {
        m_span = span;
        m_expr = expr;
        m_args = args;
    }

    void Call::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    CallMethod::CallMethod(const SharedSpan &span, const std::shared_ptr<GetAttribute>& get_attr, const std::vector<SharedNode> &args) {
        m_span = span;
        m_get_attr = get_attr;
        m_args = args;
    }

    void CallMethod::accept(NodeVisitor *visitor)  {
        visitor->visit(this);
    }

    For::For(const SharedSpan &span, const std::string &name, const SharedNode &expr, const SharedNode &statement) {
        m_span = span;
        m_name = name;
        m_expr = expr;
        m_statement = statement;
    }

    void For::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    FuncDef::FuncDef(const SharedSpan &span,
                     const std::string &name,
                     const std::vector<std::shared_ptr<Param>> &params,
                     const SharedNode &body, bool can_error,
                     const std::optional<SharedTypeNode>& return_type) {
        m_span = span;
        m_name = name;
        m_params = params;
        m_body = body;
        m_can_error = can_error;
        m_return_type = return_type;
    }

    void FuncDef::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Return::Return(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Return::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    ClosureDef::ClosureDef(const SharedSpan &span, const std::string &name, const std::shared_ptr<FuncDef> &func_def, bool is_expression) {
        m_span = span;
        m_name = name;
        m_func_def = func_def;
        m_is_expression = is_expression;
    }

    void ClosureDef::accept(NodeVisitor *visitor) {
        visitor->visit(this);
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
        visitor->visit(this);
    }

    GetAttribute::GetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name) {
        m_span = span;
        m_expr = expr;
        m_name = name;
    }

    void GetAttribute::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    SetAttribute::SetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name,
                               const SharedNode &value) {
        m_span = span;
        m_expr = expr;
        m_name = name;
        m_value = value;
    }

    void SetAttribute::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    ImportDef::ImportDef(const SharedSpan &span, const std::string &name, const std::string &alias) {
        m_span = span;
        m_name = name;
        m_alias = alias;
    }

    void ImportDef::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Try::Try(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Try::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }


    Break::Break(const SharedSpan &span) {
        m_span = span;
    }

    void Break::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Continue::Continue(const SharedSpan &span) {
        m_span = span;
    }

    void Continue::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    AsyncDef::AsyncDef(const SharedSpan &span, const SharedNode &function) {
        m_span = span;
        m_function = function;
    }

    void AsyncDef::accept(bond::NodeVisitor *visitor) {
        visitor->visit(this);
    }

    Await::Await(const SharedSpan &span, const SharedNode &expr) {
        m_span = span;
        m_expr = expr;
    }

    void Await::accept(bond::NodeVisitor *visitor) {
        visitor->visit(this);
    }

    StructuredAssign::StructuredAssign(const SharedSpan &span, const std::vector<SharedNode> &targets,
                                       const SharedNode &value) {
        m_span = span;
        m_targets = targets;
        m_value = value;
    }

    void StructuredAssign::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    ResultStatement::ResultStatement(const SharedSpan &span, const SharedNode &expr, bool is_error) {
        m_span = span;
        m_expr = expr;
        m_is_error = is_error;
    }

    void ResultStatement::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }

    DictLiteral::DictLiteral(const SharedSpan &span, const std::vector<std::pair<SharedNode, SharedNode>> &pairs) {
        m_span = span;
        m_pairs = pairs;
    }

    void DictLiteral::accept(NodeVisitor *visitor) {
        visitor->visit(this);
    }
}