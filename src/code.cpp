//
// Created by travor on 3/18/23.
//

#include "code.h"


namespace bond {

    CodeGenerator::CodeGenerator(Context *ctx) {
        m_ctx = ctx;
        m_code = GarbageCollector::instance().make_immortal<Code>();
    }
    void Code::add_code(Opcode code, const SharedSpan& span) {
        m_code.push_back(static_cast<uint8_t>(code));
        m_spans.push_back(span);
    }

    void Code::add_code(Opcode code, uint32_t oprand, const SharedSpan& span) {
        m_code.push_back(static_cast<uint8_t>(code));
        m_code.push_back(oprand);
        m_spans.push_back(span);
        m_spans.push_back(span);

    }

    GcPtr<Code> CodeGenerator::generate_code(const std::vector<std::shared_ptr<Node>> &nodes){
        m_code = GarbageCollector::instance().make_immortal<Code>();

        for (const auto& node: nodes){
            node->accept(this);
        }

        if (m_code->get_opcodes().empty()) {
            //FIXME: this will cause a crash when an error occurs
            //       in an empty block
            m_code->add_code(Opcode::RETURN, nullptr);
        }
        else {
            m_code->add_code(Opcode::RETURN, m_code->last_span());
        }
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
        auto idx = m_code->add_constant(GarbageCollector::instance().make_immortal<Number>(expr->get_value()));
        m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());
    }

    void CodeGenerator::visit_string_lit(StringLiteral* expr) {
        auto idx = m_code->add_constant(GarbageCollector::instance().make_immortal<String>(expr->get_value()));
        m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());
    }

    void CodeGenerator::visit_nil_lit(NilLiteral* expr) {
        m_code->add_code(Opcode::PUSH_NIL, expr->get_span());
    }

    void CodeGenerator::visit_print(Print *stmnt) {
        stmnt->get_expr()->accept(this);
        m_code->add_code(Opcode::PRINT, stmnt->get_span());
    }

    void CodeGenerator::visit_expr_stmnt(ExprStmnt *stmnt) {
        stmnt->get_expr()->accept(this);
    }

    void CodeGenerator::visit_identifier(Identifier *expr) {
        if (expr->is_id_global()) {
            auto idx = m_code->add_constant(GarbageCollector::instance().make_immortal<String>(expr->get_name()));
            m_code->add_code(Opcode::LOAD_GLOBAL, idx, expr->get_span());
        }

        //TODO: code generation for local variables
    }

    void CodeGenerator::visit_new_var(NewVar *stmnt) {
        stmnt->get_expr()->accept(this);

        if (stmnt->is_var_global()) {
            auto idx = m_code->add_constant(GarbageCollector::instance().make_immortal<String>(stmnt->get_name()));
            m_code->add_code(Opcode::SET_GLOBAL, idx, stmnt->get_span());
        }

        //TODO: code generation for local variables
    }


} // bond