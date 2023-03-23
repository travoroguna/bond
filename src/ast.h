//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once
#include "lexer.h"
#include "nodevisitor.h"

namespace bond{
    enum class NodeType {
        Expr, BinOp, Unary, FalseLit, TrueLit, NilLit, NumLit, StringLit
    };


    class Expression {
    public:
        Expression() = default;
        std::shared_ptr<Span> get_span() { return m_span; }
        NodeType get_type() { return m_type; }
        virtual void accept(NodeVisitor *visitor) = 0;

    protected:
        std::shared_ptr<Span> m_span;
        NodeType m_type;
    };


    using SharedExpr = std::shared_ptr<Expression>;
    using SharedSpan = std::shared_ptr<Span>;

    class BinaryOp : public Expression{
    public:
        BinaryOp(SharedSpan &span, SharedExpr &left, Token op, SharedExpr &right);
        void accept(NodeVisitor *visitor) override;
        SharedExpr get_left() { return m_left; }
        SharedExpr get_right() { return m_right; }
        Token get_op() { return m_op; }
    private:
        SharedExpr m_left;
        SharedExpr m_right;
        Token m_op;
    };


    class Unary : public Expression{
    public:
        Unary(const SharedSpan& span, Token op, const SharedExpr& expr);
        void accept(NodeVisitor *visitor) override;
        SharedExpr get_expr() { return m_expr; }
        Token get_op() { return m_op; }

    private:
        Token m_op;
        SharedExpr m_expr;
    };

    class FalseLiteral: public Expression{
    public:
        explicit FalseLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };

    class TrueLiteral: public Expression{
    public:
        explicit TrueLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };


    class NilLiteral: public Expression{
    public:
        explicit NilLiteral(const SharedSpan& span);
        void accept(NodeVisitor *visitor) override;

    };

    class NumberLiteral: public Expression{
    public:
        NumberLiteral(const SharedSpan& span, const std::string& lexeme);
        void accept(NodeVisitor *visitor) override;
        float get_value() { return m_value; }

    private:
        float m_value;
    };


    class StringLiteral: public Expression{
    public:
        StringLiteral(const SharedSpan& span, const std::string& lexeme);
        void accept(NodeVisitor *visitor) override;
        std::string get_value() { return m_value; }

    private:
        std::string m_value;
    };




};
