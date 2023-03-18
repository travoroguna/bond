//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once
#include "lexer.h"

namespace bond{
    class Expression {
    public:
        std::shared_ptr<Span> get_span() { return m_span; }
    protected:
        std::shared_ptr<Span> m_span;
    };


    using SharedExpr = std::shared_ptr<Expression>;
    using SharedSpan = std::shared_ptr<Span>;

    class BinaryOp : public Expression{
    public:
        BinaryOp(SharedSpan &span, SharedExpr &left, Token op, SharedExpr &right);
    private:
        SharedExpr m_left;
        SharedExpr m_right;
        Token m_op;
    };


    class Unary : public Expression{
    public:
        Unary(const SharedSpan& span, Token op, const SharedExpr& expr);
    private:
        Token m_op;
        SharedExpr m_expr;
    };

    class FalseLiteral: public Expression{
    public:
        explicit FalseLiteral(const SharedSpan& span);
    };

    class TrueLiteral: public Expression{
    public:
        explicit TrueLiteral(const SharedSpan& span);
    };


    class NilLiteral: public Expression{
    public:
        explicit NilLiteral(const SharedSpan& span);
    };

    class NumberLiteral: public Expression{
    public:
        NumberLiteral(const SharedSpan& span, const std::string& lexeme);
    private:
        float m_value;
    };


    class StringLiteral: public Expression{
    public:
        StringLiteral(const SharedSpan& span, const std::string& lexeme);
    private:
        std::string m_value;
    };




};
