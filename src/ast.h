//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once
#include "lexer.h"
#include "nodevisitor.h"

namespace bond{
    enum class NodeType {
        Node, BinOp, Unary, FalseLit, TrueLit, NilLit, NumLit, StringLit
    };


    class Node {
    public:
        Node() = default;
        std::shared_ptr<Span> get_span() { return m_span; }
        NodeType get_type() { return m_type; }
        virtual void accept(NodeVisitor *visitor) = 0;

    protected:
        std::shared_ptr<Span> m_span;
        NodeType m_type {NodeType::Node};
    };


    using SharedNode = std::shared_ptr<Node>;
    using SharedSpan = std::shared_ptr<Span>;

    class BinaryOp : public Node{
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


    class Unary : public Node{
    public:
        Unary(const SharedSpan& span, Token op, const SharedNode& expr);
        void accept(NodeVisitor *visitor) override;
        SharedNode get_expr() { return m_expr; }
        Token get_op() { return m_op; }

    private:
        Token m_op;
        SharedNode m_expr;
    };

    class FalseLiteral: public Node{
    public:
        explicit FalseLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };

    class TrueLiteral: public Node{
    public:
        explicit TrueLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };


    class NilLiteral: public Node{
    public:
        explicit NilLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };

    class NumberLiteral: public Node{
    public:
        NumberLiteral(const SharedSpan& span, const std::string& lexeme);
        void accept(NodeVisitor *visitor) override;
        float get_value() { return m_value; }

    private:
        float m_value;
    };


    class StringLiteral: public Node{
    public:
        StringLiteral(const SharedSpan& span, const std::string& lexeme);
        void accept(NodeVisitor *visitor) override;
        std::string get_value() { return m_value; }

    private:
        std::string m_value;
    };


    class Identifier: public Node{
    public:
        Identifier(const SharedSpan& span, const std::string& name, bool is_global);
        void accept(NodeVisitor *visitor) override;
        std::string get_name() { return m_name; }

        void set_global() { is_global = true; }
        void set_id(uint32_t id) { m_id = id; }
        uint32_t get_id() { return m_id; }
        bool is_id_global() { return is_global; }


    private:
        std::string m_name;
        bool is_global = false;
        uint32_t m_id = 0;

    };



    class NewVar: public Node{
    public:
        NewVar(const SharedSpan& span, const std::string& name, const SharedNode &expr, bool is_global);
        void accept(NodeVisitor *visitor) override;
        std::string get_name() { return m_name; }

        void set_global() { is_global = true; }
        bool is_var_global() { return is_global; }
        SharedNode get_expr() { return m_expr; }

    private:
        std::string m_name;
        SharedNode m_expr;
        bool is_global = false;

    };

    class Print: public Node{
    public:
        Print(const SharedSpan& span, const SharedNode &expr);
        void accept(NodeVisitor *visitor) override;
        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;
    };


    class ExprStmnt: public Node{
    public:
        ExprStmnt(const SharedSpan& span, const SharedNode &expr);
        void accept(NodeVisitor *visitor) override;
        SharedNode get_expr() { return m_expr; }

    private:
        SharedNode m_expr;

    };
};
