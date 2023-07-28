//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include "lexer.h"
#include "nodevisitor.h"
#include <optional>

namespace bond {
    enum class NodeType {
        Node, BinOp, Unary, FalseLit, TrueLit, NilLit, NumLit, StringLit
    };

    class Node;

    class NodeVisitor;

    class Node {
    public:
        Node() = default;

        virtual ~Node() = default;

        std::shared_ptr<Span> get_span() { return m_span; }

        NodeType get_type() { return m_type; }

        virtual void accept(NodeVisitor *visitor) = 0;

    protected:
        std::shared_ptr<Span> m_span;
        NodeType m_type{NodeType::Node};
    };

    using SharedNode = std::shared_ptr<Node>;
    using SharedSpan = std::shared_ptr<Span>;

    class BinaryOp : public Node {
    public:
        BinaryOp(SharedSpan &span, SharedNode &left, Token op, SharedNode &right);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_left() { return m_left; }

        SharedNode get_right() { return m_right; }

        Token get_op() { return m_op; }

    private:
        SharedNode m_left;
        SharedNode m_right;
        Token m_op;
    };

    class Unary : public Node {
    public:
        Unary(const SharedSpan &span, Token op, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        Token get_op() { return m_op; }

    private:
        Token m_op;
        SharedNode m_expr;
    };

    class FalseLiteral : public Node {
    public:
        explicit FalseLiteral(const SharedSpan &span);

        void accept(NodeVisitor *visitor) override;

    };

    class TrueLiteral : public Node {
    public:
        explicit TrueLiteral(const SharedSpan &span);

        void accept(NodeVisitor *visitor) override;

    };

    class NilLiteral : public Node {
    public:
        explicit NilLiteral(const SharedSpan &span);

        void accept(NodeVisitor *visitor) override;

    };

    class NumberLiteral : public Node {
    public:
        NumberLiteral(const SharedSpan &span, const std::string &lexeme, bool is_int);

        void accept(NodeVisitor *visitor) override;

        std::string get_value() { return m_value; }

        [[nodiscard]] bool is_int() const { return m_is_int; }

    private:
        std::string m_value;
        bool m_is_int;
    };

    class StringLiteral : public Node {
    public:
        StringLiteral(const SharedSpan &span, const std::string &lexeme);

        void accept(NodeVisitor *visitor) override;

        std::string get_value() { return m_value; }

    private:
        std::string m_value;
    };

    class Identifier : public Node {
    public:
        Identifier(const SharedSpan &span, const std::string &name);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

    private:
        std::string m_name;

    };

    class NewVar : public Node {
    public:
        NewVar(const SharedSpan &span, const std::string &name, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        SharedNode get_expr() { return m_expr; }

    private:
        std::string m_name;
        SharedNode m_expr;

    };


    class ExprStmnt : public Node {
    public:
        ExprStmnt(const SharedSpan &span, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;

    };

    class Assign : public Node {
    public:
        Assign(const SharedSpan &span, const std::string &name, const SharedNode &node);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        SharedNode get_expr() { return m_node; }

    private:
        std::string m_name;
        SharedNode m_node;

    };

    class Block : public Node {
    public:
        Block(const SharedSpan &span, const std::vector<SharedNode> &nodes);

        void accept(NodeVisitor *visitor) override;

        std::vector<SharedNode> get_nodes() { return m_nodes; }

    private:
        std::vector<SharedNode> m_nodes;
    };

    class ListLiteral : public Node {
    public:
        ListLiteral(const SharedSpan &span, const std::vector<SharedNode> &nodes);

        void accept(NodeVisitor *visitor) override;

        std::vector<SharedNode> get_nodes() { return m_nodes; }

    private:
        std::vector<SharedNode> m_nodes;
    };

    class GetItem : public Node {
    public:
        GetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        SharedNode get_index() { return m_index; }

    private:
        SharedNode m_expr;
        SharedNode m_index;
    };

    class SetItem : public Node {
    public:
        SetItem(const SharedSpan &span, const SharedNode &expr, const SharedNode &index, const SharedNode &value);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        SharedNode get_index() { return m_index; }

        SharedNode get_value() { return m_value; }

    private:
        SharedNode m_expr;
        SharedNode m_index;
        SharedNode m_value;
    };

    class If : public Node {
    public:
        If(const SharedSpan &span, const SharedNode &condition, const SharedNode &then,
           std::optional<SharedNode> else_node);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_condition() { return m_condition; }

        SharedNode get_then() { return m_then; }

        std::optional<SharedNode> get_else() { return m_else; }

    private:
        SharedNode m_condition;
        SharedNode m_then;
        std::optional<SharedNode> m_else;
    };

    class While : public Node {
    public:
        While(const SharedSpan &span, const SharedNode &condition, const SharedNode &statement);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_condition() { return m_condition; }

        SharedNode get_statement() { return m_statement; }

    private:
        SharedNode m_condition;
        SharedNode m_statement;
    };

    class Call : public Node {
    public:
        Call(const SharedSpan &span, const SharedNode &expr, const std::vector<SharedNode> &args);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        std::vector<SharedNode> get_args() { return m_args; }

    private:
        SharedNode m_expr;
        std::vector<SharedNode> m_args;
    };



    class For : public Node {
    public:
        For(const SharedSpan &span, const std::string &name, const SharedNode &expr, const SharedNode &statement);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        SharedNode get_expr() { return m_expr; }

        SharedNode get_statement() { return m_statement; }

    private:
        std::string m_name;
        SharedNode m_expr;
        SharedNode m_statement;
    };

    class FuncDef : public Node {
    public:
        FuncDef(const SharedSpan &span,
                const std::string &name,
                const std::vector<std::pair<std::string, SharedSpan>> &params,
                const SharedNode &body, bool can_error);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        std::vector<std::pair<std::string, SharedSpan>> get_params() { return m_params; }

        SharedNode get_body() { return m_body; }

        [[nodiscard]] bool can_error() const { return m_can_error; }

    private:
        std::string m_name;
        std::vector<std::pair<std::string, SharedSpan>> m_params;
        SharedNode m_body;
        bool m_can_error;
    };

    class Return : public Node {
    public:
        Return(const SharedSpan &span, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;
    };

    class ClosureDef : public Node {
    public:
        ClosureDef(const SharedSpan &span, const std::string &name, const std::shared_ptr<FuncDef> &func_def);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        std::shared_ptr<FuncDef> get_func_def() { return m_func_def; }

    private:
        std::string m_name;
        std::shared_ptr<FuncDef> m_func_def;
    };

    class StructNode : public Node {
    public:
        StructNode(const SharedSpan &span, const std::string &name, const std::vector<std::string> &params,
                   const std::vector<SharedNode> &methods);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        std::vector<std::string> get_params() { return m_params; }

        std::vector<SharedNode> get_methods() { return m_methods; }

    private:
        std::string m_name;
        std::vector<std::string> m_params;
        std::vector<SharedNode> m_methods;
    };

    class GetAttribute : public Node {
    public:
        GetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        std::string get_name() { return m_name; }

    private:
        SharedNode m_expr;
        std::string m_name;
    };

    class SetAttribute : public Node {
    public:
        SetAttribute(const SharedSpan &span, const SharedNode &expr, const std::string &name, const SharedNode &value);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        std::string get_name() { return m_name; }

        SharedNode get_value() { return m_value; }

    private:
        SharedNode m_expr;
        std::string m_name;
        SharedNode m_value;
    };

    class ImportDef : public Node {
    public:
        ImportDef(const SharedSpan &span, const std::string &name, const std::string &alias);

        void accept(NodeVisitor *visitor) override;

        std::string get_name() { return m_name; }

        std::string get_alias() { return m_alias; }

        std::string get_actual_path() { return actual_path; }
        void set_actual_path(const std::string& path) { actual_path = path; }

    private:
        std::string m_name;
        std::string m_alias;
        std::string actual_path;
    };

    class Try : public Node {
    public:
        Try(const SharedSpan &span, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;
    };

    class Break : public Node {
    public:
        explicit Break(const SharedSpan &span);

        void accept(NodeVisitor *visitor) override;
    };


    class Continue : public Node {
    public:
        explicit Continue(const SharedSpan &span);

        void accept(NodeVisitor *visitor) override;
    };

    class AsyncDef : public Node {
    public:
        AsyncDef(const SharedSpan &span, const SharedNode &function);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_function() { return m_function; }

    private:
        SharedNode m_function;
    };


    class Await : public Node {
    public:
        Await(const SharedSpan &span, const SharedNode &expr);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;
    };

    class StructuredAssign : public Node {
    public:
        StructuredAssign(const SharedSpan &span, const std::vector<SharedNode> &targets, const SharedNode &value);

        std::vector<SharedNode> get_targets() { return m_targets; }

        SharedNode get_value() { return m_value; }

        void accept(NodeVisitor *visitor) override;

    private:
        std::vector<SharedNode> m_targets;
        SharedNode m_value;

    };

    class CallMethod : public Node {
    public:
        CallMethod(const SharedSpan &span, const std::shared_ptr<GetAttribute>& get_attr, const std::vector<SharedNode> &args);

        void accept(NodeVisitor *visitor) override;

        std::shared_ptr<GetAttribute> get_node() { return m_get_attr; }

        std::vector<SharedNode> get_args() { return m_args; }

    private:
        std::shared_ptr<GetAttribute> m_get_attr;
        std::vector<SharedNode> m_args;
    };

    class ResultStatement : public Node {
    public:
        ResultStatement(const SharedSpan &span, const SharedNode &expr, bool is_error);

        void accept(NodeVisitor *visitor) override;

        SharedNode get_expr() { return m_expr; }

        bool is_error() { return m_is_error; }

    private:
        SharedNode m_expr;
        bool m_is_error;
    };

}