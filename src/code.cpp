//
// Created by travor on 3/18/23.
//

#include "code.h"
#include "object.h"

namespace bond {

CodeGenerator::CodeGenerator(Context *ctx, Scopes *scopes) : m_ctx(ctx), m_scopes(scopes) {
    m_code = GarbageCollector::instance().make_immortal<Code>();
}

void Code::add_code(Opcode code, const SharedSpan &span) {
    m_code.push_back(static_cast<uint8_t>(code));
    m_spans.push_back(span);
}

void Code::add_code(Opcode code, uint32_t oprand, const SharedSpan &span) {
    m_code.push_back(static_cast<uint8_t>(code));
    m_code.push_back(oprand);
    m_spans.push_back(span);
    m_spans.push_back(span);
}

std::string Code::dissasemble() {
    std::stringstream ss;
    size_t count = 0;

#define SIMPLE_INSTRUCTION(name) \
        case Opcode::name:    \
            count = simple_instruction(ss, #name, count);\
            break

#define CONSTANT_INSTRUCTION(name) \
        case Opcode::name:    \
            count = constant_instruction(ss, #name, count);\
            break
#define OPRAND_INSTRUCTION(name) \
        case Opcode::name:    \
            count = oprand_instruction(ss, #name, count);\
            break

    while (count < m_code.size()) {
        auto opcode = static_cast<Opcode>(m_code[count]);
        switch (opcode) {
            CONSTANT_INSTRUCTION(LOAD_CONST);
            CONSTANT_INSTRUCTION(STORE_GLOBAL);
            CONSTANT_INSTRUCTION(LOAD_GLOBAL);
            CONSTANT_INSTRUCTION(CREATE_GLOBAL);
            CONSTANT_INSTRUCTION(CREATE_LOCAL);
            CONSTANT_INSTRUCTION(STORE_FAST);
            CONSTANT_INSTRUCTION(LOAD_FAST);

            OPRAND_INSTRUCTION(BUILD_LIST);

            SIMPLE_INSTRUCTION(BIN_ADD);
            SIMPLE_INSTRUCTION(BIN_SUB);
            SIMPLE_INSTRUCTION(BIN_MUL);
            SIMPLE_INSTRUCTION(BIN_DIV);
            SIMPLE_INSTRUCTION(RETURN);
            SIMPLE_INSTRUCTION(PUSH_TRUE);
            SIMPLE_INSTRUCTION(PUSH_FALSE);
            SIMPLE_INSTRUCTION(PUSH_NIL);
            SIMPLE_INSTRUCTION(PRINT);
            SIMPLE_INSTRUCTION(NE);
            SIMPLE_INSTRUCTION(EQ);
            SIMPLE_INSTRUCTION(LE);
            SIMPLE_INSTRUCTION(GT);
            SIMPLE_INSTRUCTION(GE);
            SIMPLE_INSTRUCTION(LT);
            SIMPLE_INSTRUCTION(POP_TOP);
            SIMPLE_INSTRUCTION(GET_ITEM);
            SIMPLE_INSTRUCTION(SET_ITEM);
        }

    }

#undef SIMPLE_INSTRUCTION
#undef CONSTANT_INSTRUCTION
    return ss.str();
}

size_t Code::simple_instruction(std::stringstream &ss, const char *name, size_t offset) {
    ss << name << "\n";
    return offset + 1;
}

size_t Code::constant_instruction(std::stringstream &ss, const char *name, size_t offset) {
    auto constant = m_constants[m_code[offset + 1]];
    ss << name << "     " << m_code[offset + 1] << ",  " << constant->str() << "\n";
    return offset + 2;
}

size_t Code::oprand_instruction(std::stringstream &ss, const char *name, size_t offset) {
    ss << name << "     " << m_code[offset + 1] << "\n";
    return offset + 2;
}

std::expected<GcPtr<Object>, RuntimeError> Code::$_bool() {
    return GarbageCollector::instance().make_immortal<Bool>(true);
}

GcPtr<Code> CodeGenerator::generate_code(const std::vector<std::shared_ptr<Node>> &nodes) {
    m_code = GarbageCollector::instance().make_immortal<Code>();

    for (const auto &node : nodes) {
        node->accept(this);
    }

    if (m_code->get_opcodes().empty()) {
        //FIXME: this will cause a crash when an error occurs
        //       in an empty block
        m_code->add_code(Opcode::RETURN, nullptr);
    } else {
        m_code->add_code(Opcode::RETURN, m_code->last_span());
    }
    return m_code;
}

void CodeGenerator::visit_bin_op(BinaryOp *expr) {
    expr->get_left()->accept(this);
    expr->get_right()->accept(this);

    switch (expr->get_op().get_type()) {
        case TokenType::PLUS:m_code->add_code(Opcode::BIN_ADD, expr->get_op().get_span());
            break;
        case TokenType::MINUS:m_code->add_code(Opcode::BIN_SUB, expr->get_op().get_span());
            break;
        case TokenType::STAR:m_code->add_code(Opcode::BIN_MUL, expr->get_op().get_span());
            break;
        case TokenType::SLASH:m_code->add_code(Opcode::BIN_DIV, expr->get_op().get_span());
            break;
        case TokenType::BANG_EQUAL:m_code->add_code(Opcode::NE, expr->get_op().get_span());
            break;
        case TokenType::EQUAL_EQUAL:m_code->add_code(Opcode::EQ, expr->get_op().get_span());
            break;
        case TokenType::LESS:m_code->add_code(Opcode::LT, expr->get_op().get_span());
            break;
        case TokenType::LESS_EQUAL:m_code->add_code(Opcode::LE, expr->get_op().get_span());
            break;
        case TokenType::GREATER:m_code->add_code(Opcode::GT, expr->get_op().get_span());
            break;
        case TokenType::GREATER_EQUAL:m_code->add_code(Opcode::GE, expr->get_op().get_span());
            break;
        default:break;
    }
}

void CodeGenerator::visit_unary(Unary *expr) {
    expr->get_expr()->accept(this);
    switch (expr->get_op().get_type()) {
        case TokenType::MINUS:m_code->add_code(Opcode::BIN_SUB, expr->get_op().get_span());
            break;
        default:break;
    }
}

void CodeGenerator::visit_true_lit(TrueLiteral *expr) {
    m_code->add_code(Opcode::PUSH_TRUE, expr->get_span());
}

void CodeGenerator::visit_false_lit(FalseLiteral *expr) {
    m_code->add_code(Opcode::PUSH_FALSE, expr->get_span());
}

void CodeGenerator::visit_num_lit(NumberLiteral *expr) {
    auto idx = m_code->add_constant<Number>(expr->get_value());
    m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());

}

void CodeGenerator::visit_string_lit(StringLiteral *expr) {
    auto idx = m_code->add_constant<String>(expr->get_value());
    m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());

}

void CodeGenerator::visit_nil_lit(NilLiteral *expr) {
    m_code->add_code(Opcode::PUSH_NIL, expr->get_span());
}

void CodeGenerator::visit_print(Print *stmnt) {
    stmnt->get_expr()->accept(this);
    m_code->add_code(Opcode::PRINT, stmnt->get_span());
}

void CodeGenerator::visit_expr_stmnt(ExprStmnt *stmnt) {
    stmnt->get_expr()->accept(this);
    m_code->add_code(Opcode::POP_TOP, stmnt->get_span());
}

void CodeGenerator::visit_identifier(Identifier *expr) {
    auto var = m_scopes->get(expr->get_name());
    auto idx = m_code->add_constant<String>(expr->get_name());

    if (var.has_value()) {
        auto v = var.value();

        if (v->is_global) {
            m_code->add_code(Opcode::LOAD_GLOBAL, idx, expr->get_span());
            return;
        }
        m_code->add_code(Opcode::LOAD_FAST, idx, expr->get_span());
        return;
    }
    m_ctx->error(expr->get_span(), fmt::format("Undefined variable {}", expr->get_name()));
}

void CodeGenerator::visit_new_var(NewVar *stmnt) {
    stmnt->get_expr()->accept(this);
    auto var = m_scopes->get(stmnt->get_name());
    auto idx = m_code->add_constant<String>(stmnt->get_name());

    if (var.has_value()) {
        m_code->add_code(Opcode::CREATE_GLOBAL, idx, stmnt->get_span());
        return;
    }

    m_scopes->declare(stmnt->get_name(), stmnt->get_span(), true, false);
    m_code->add_code(Opcode::CREATE_LOCAL, idx, stmnt->get_span());
}

void CodeGenerator::visit_assign(Assign *stmnt) {
    stmnt->get_expr()->accept(this);
    auto var = m_scopes->get(stmnt->get_name());
    auto idx = m_code->add_constant<String>(stmnt->get_name());

    if (var.has_value()) {
        auto v = var.value();
        if (v->is_global) {
            m_code->add_code(Opcode::STORE_GLOBAL, idx, stmnt->get_span());
            return;
        }
        m_code->add_code(Opcode::STORE_FAST, idx, stmnt->get_span());
        return;
    }
    m_ctx->error(stmnt->get_span(), "Undefined variable '" + stmnt->get_name() + "'.");
}

void CodeGenerator::visit_block(Block *stmnt) {
    m_scopes->new_scope();
    for (auto &s : stmnt->get_nodes()) {
        s->accept(this);
    }
    m_scopes->end_scope();
}

void CodeGenerator::visit_list(List *expr) {
    for (auto &e : expr->get_nodes()) {
        e->accept(this);
    }
    m_code->add_code(Opcode::BUILD_LIST, expr->get_nodes().size(), expr->get_span());
}

void CodeGenerator::visit_get_item(GetItem *expr) {
    expr->get_expr()->accept(this);
    expr->get_index()->accept(this);
    m_code->add_code(Opcode::GET_ITEM, expr->get_span());
}

void CodeGenerator::visit_set_item(SetItem *expr) {
    expr->get_expr()->accept(this);
    expr->get_index()->accept(this);
    expr->get_value()->accept(this);
    m_code->add_code(Opcode::SET_ITEM, expr->get_span());
}

} // bond