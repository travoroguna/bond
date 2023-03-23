//
// Created by travor on 3/18/23.
//

#include "code.h"


namespace bond {
    NodeVisitor::NodeVisitor() {};
    
    CodeGenerator::CodeGenerator(Context *ctx) {
        m_ctx = ctx;
    }
    void Code::add_code(Opcode code, SharedSpan span) {
        m_code.push_back(static_cast<uint8_t>(code));
        m_spans.push_back(span);
    }

    void Code::add_code(Opcode code, uint32_t oprand, SharedSpan span) {
        m_code.push_back(static_cast<uint8_t>(code));
        m_code.push_back(oprand);
        m_spans.push_back(span);
        m_spans.push_back(span);

    }

    Code* CodeGenerator::generate_code(SharedExpr expr){
        m_code = new Code();
        expr->accept(this);
        m_code->add_code(Opcode::RETURN, nullptr);
        return m_code;
    }
  
    void CodeGenerator::visit_bin_op(BinaryOp* expr){
        expr->get_left()->accept(this);
        expr->get_right()->accept(this);

        switch (expr->get_op().get_type()) {
            case TokenType::PLUS:
                m_code->add_code(Opcode::BIN_ADD, expr->get_op().get_span());
                break;
            case TokenType::MINUS:
                m_code->add_code(Opcode::BIN_SUB, expr->get_op().get_span());
                break;
            case TokenType::STAR:
                m_code->add_code(Opcode::BIN_MUL, expr->get_op().get_span());
                break;
            case TokenType::SLASH:
                m_code->add_code(Opcode::BIN_DIV, expr->get_op().get_span());
                break;
            default:
                break;
        }
    }

    void CodeGenerator::visit_unary(Unary* expr){
        expr->get_expr()->accept(this);
        switch (expr->get_op().get_type()) {
            case TokenType::MINUS:
                m_code->add_code(Opcode::BIN_SUB, expr->get_op().get_span());
                break;
            default:
                break;
        }
    }

    void CodeGenerator::visit_true_lit(TrueLiteral* expr){
        m_code->add_code(Opcode::PUSH_TRUE, expr->get_span());
    }

    void CodeGenerator::visit_false_lit(FalseLiteral* expr){
        m_code->add_code(Opcode::PUSH_FALSE, expr->get_span());
    }

    void CodeGenerator::visit_num_lit(NumberLiteral* expr){
        m_code->add_code(Opcode::LOAD_CONST, m_code->add_constant(new Number(expr->get_value())), expr->get_span());
    }

    void CodeGenerator::visit_string_lit(StringLiteral* expr) {
        // m_code->add_code(Opcode::LOAD_CONST, m_code->add_constant(new String(expr->get_value())), expr->get_span());
    }

    void CodeGenerator::visit_nil_lit(NilLiteral* expr) {
        m_code->add_code(Opcode::PUSH_NIL, expr->get_span());
    }



} // bond