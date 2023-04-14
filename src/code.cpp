//
// Created by travor on 3/18/23.
//

#include "code.h"
#include "object.h"

namespace bond {

    CodeGenerator::CodeGenerator(Context *ctx, Scopes *scopes) : m_ctx(ctx), m_scopes(scopes) {
//        m_code = GarbageCollector::instance().make_immortal<Code>();
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

        size_t pre_line = 0;

        while (count < m_code.size()) {
            auto opcode = static_cast<Opcode>(m_code[count]);

            if (pre_line != m_spans[count]->line) {
                ss << fmt::format("{:6}  {:04} ", m_spans[count]->line, count);
                pre_line = m_spans[count]->line;

            } else {
                ss << fmt::format("{:6}  {:04} ", "", count);
            }

            switch (opcode) {
                CONSTANT_INSTRUCTION(LOAD_CONST);
                CONSTANT_INSTRUCTION(STORE_GLOBAL);
                CONSTANT_INSTRUCTION(LOAD_GLOBAL);
                CONSTANT_INSTRUCTION(CREATE_GLOBAL);
                CONSTANT_INSTRUCTION(CREATE_LOCAL);
                CONSTANT_INSTRUCTION(STORE_FAST);
                CONSTANT_INSTRUCTION(LOAD_FAST);
                CONSTANT_INSTRUCTION(ITER_NEXT);
                CONSTANT_INSTRUCTION(GET_ATTRIBUTE);
                CONSTANT_INSTRUCTION(SET_ATTRIBUTE);
                CONSTANT_INSTRUCTION(IMPORT);

                OPRAND_INSTRUCTION(BUILD_LIST);
                OPRAND_INSTRUCTION(JUMP_IF_FALSE);
                OPRAND_INSTRUCTION(JUMP);
                OPRAND_INSTRUCTION(CALL);
                OPRAND_INSTRUCTION(ITER_END);
                OPRAND_INSTRUCTION(CREATE_FUNCTION);
                OPRAND_INSTRUCTION(CREATE_STRUCT);
                OPRAND_INSTRUCTION(TRY);

                SIMPLE_INSTRUCTION(ITER);
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
                SIMPLE_INSTRUCTION(OR);
                SIMPLE_INSTRUCTION(AND);
            }

        }

#undef SIMPLE_INSTRUCTION
#undef CONSTANT_INSTRUCTION

        for (auto &constant: m_constants) {
            if (constant->is<Function>()) {
                auto func = constant->as<Function>();
                ss << fmt::format("\ndisassembly of function {}\n", func->get_name());
                ss << func->get_code()->dissasemble();
            } else if (constant->is<Struct>()) {
                auto st = constant->as<Struct>();
                ss << fmt::format("\ndisassembly of struct {}\n", st->get_name());

                for (auto &[name, method]: st->get_methods()->get_map()) {
                    ss << fmt::format("\ndisassembly of {}.{}.{}\n", st->get_name(), st->get_name(),
                                      name->as<String>()->get_value());
                    ss << method->as<Function>()->get_code()->dissasemble();
                }
            }
        }

        return ss.str();
    }

    size_t Code::simple_instruction(std::stringstream &ss, const char *name, size_t offset) {
        ss << fmt::format("{}\n", name);
        return offset + 1;
    }

    size_t Code::constant_instruction(std::stringstream &ss, const char *name, size_t offset) {
        auto constant = m_constants[m_code[offset + 1]];
        ss << fmt::format("{:<16} {:0>4}, {:<16}\n", name, m_code[offset + 1], constant->str());
        return offset + 2;
    }

    size_t Code::oprand_instruction(std::stringstream &ss, const char *name, size_t offset) {
        ss << fmt::format("{:<16} {:<4}\n", name, m_code[offset + 1]);
        return offset + 2;
    }

    std::expected<GcPtr<Object>, RuntimeError> Code::$_bool() {
        return GarbageCollector::instance().make<Bool>(true);
    }

    void CodeGenerator::finish_generation() {
        if (m_code->get_opcodes().empty()) {
            m_code->add_code(Opcode::PUSH_NIL, std::make_shared<Span>(0, 0, 0, 0));
            m_code->add_code(Opcode::RETURN, std::make_shared<Span>(0, 0, 0, 0));
        } else {
            if (static_cast<Opcode>(m_code->get_opcodes().back()) != Opcode::RETURN) {
                m_code->add_code(Opcode::PUSH_NIL, m_code->last_span());
                m_code->add_code(Opcode::RETURN, m_code->last_span());
            }
        }
    }

    GcPtr<Code> CodeGenerator::generate_code(const std::vector<std::shared_ptr<Node>> &nodes) {
        m_code = GarbageCollector::instance().make_immortal<Code>();

        for (const auto &node: nodes) {
            node->accept(this);
        }

        finish_generation();
        return m_code;
    }

    void CodeGenerator::visit_bin_op(BinaryOp *expr) {
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
            case TokenType::BANG_EQUAL:
                m_code->add_code(Opcode::NE, expr->get_op().get_span());
                break;
            case TokenType::EQUAL_EQUAL:
                m_code->add_code(Opcode::EQ, expr->get_op().get_span());
                break;
            case TokenType::LESS:
                m_code->add_code(Opcode::LT, expr->get_op().get_span());
                break;
            case TokenType::LESS_EQUAL:
                m_code->add_code(Opcode::LE, expr->get_op().get_span());
                break;
            case TokenType::GREATER:
                m_code->add_code(Opcode::GT, expr->get_op().get_span());
                break;
            case TokenType::GREATER_EQUAL:
                m_code->add_code(Opcode::GE, expr->get_op().get_span());
                break;
            case TokenType::OR:
                m_code->add_code(Opcode::OR, expr->get_op().get_span());
                break;
            case TokenType::AND:
                m_code->add_code(Opcode::AND, expr->get_op().get_span());
            default:
                break;
        }
    }

    void CodeGenerator::visit_unary(Unary *expr) {
        expr->get_expr()->accept(this);
        switch (expr->get_op().get_type()) {
            case TokenType::MINUS:
                m_code->add_code(Opcode::BIN_SUB, expr->get_op().get_span());
                break;
            default:
                break;
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
        for (auto &s: stmnt->get_nodes()) {
            s->accept(this);
        }
        m_scopes->end_scope();
    }

    void CodeGenerator::visit_list(List *expr) {
        for (auto &e: expr->get_nodes()) {
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

    void CodeGenerator::visit_if(If *stmnt) {
        stmnt->get_condition()->accept(this);
        auto next = m_code->current_index() + 1;
        m_code->add_code(Opcode::JUMP_IF_FALSE, next, stmnt->get_span());

        stmnt->get_then()->accept(this);
        m_code->patch_code(next, m_code->current_index() + 2);

        next = m_code->current_index() + 1;
        m_code->add_code(Opcode::JUMP, next, stmnt->get_span());

        if (stmnt->get_else().has_value()) {
            stmnt->get_else().value()->accept(this);
        }
        m_code->patch_code(next, m_code->current_index());
    }

    void CodeGenerator::visit_while(While *stmnt) {
        auto start = m_code->current_index();
        stmnt->get_condition()->accept(this);
        auto next = m_code->current_index() + 1;
        m_code->add_code(Opcode::JUMP_IF_FALSE, next, stmnt->get_span());

        stmnt->get_statement()->accept(this);
        m_code->add_code(Opcode::JUMP, start, stmnt->get_span());
        m_code->patch_code(next, m_code->current_index());
    }

    void CodeGenerator::visit_call(Call *expr) {
        expr->get_expr()->accept(this);
        for (auto &e: expr->get_args()) {
            e->accept(this);
        }
        m_code->add_code(Opcode::CALL, expr->get_args().size(), expr->get_span());
    }

    void CodeGenerator::visit_for(For *stmnt) {

        m_scopes->new_scope();
        m_scopes->declare(stmnt->get_name(), stmnt->get_span(), true, false);
        auto local_name = m_code->add_constant<String>(stmnt->get_name());
        m_code->add_code(Opcode::PUSH_NIL, stmnt->get_span());
        m_code->add_code(Opcode::CREATE_LOCAL, local_name, stmnt->get_span());

        stmnt->get_expr()->accept(this);
        m_code->add_code(Opcode::ITER, stmnt->get_span());

        auto start = m_code->current_index();
        m_code->add_code(Opcode::ITER_END, 0, stmnt->get_span());

        m_code->add_code(Opcode::ITER_NEXT, local_name, stmnt->get_span());

        stmnt->get_statement()->accept(this);
        m_code->add_code(Opcode::JUMP, start, stmnt->get_span());

        m_code->patch_code(start + 1, m_code->current_index());
        m_code->add_code(Opcode::POP_TOP, stmnt->get_span());
        m_scopes->end_scope();

    }

    void CodeGenerator::visit_func_def(FuncDef *stmnt) {
        m_in_function = true;

        auto var = m_scopes->get(stmnt->get_name());

        auto generator = CodeGenerator(m_ctx, m_scopes);
        generator.m_in_function = true;

        m_scopes->new_scope();
        for (auto &[name, span]: stmnt->get_params()) {
            m_scopes->declare(name, span, true, false);
        }

        auto code = generator.generate_code(stmnt->get_body());
        generator.m_in_function = false;

        m_scopes->end_scope();
        auto idx = m_code->add_constant<Function>(code, stmnt->get_params(), stmnt->get_name());
        auto name = m_code->add_constant<String>(stmnt->get_name());

        m_code->add_code(Opcode::CREATE_FUNCTION, idx, stmnt->get_span());
        if (var.has_value()) {
            m_code->add_code(Opcode::CREATE_GLOBAL, name, stmnt->get_span());
            return;
        }
        m_code->add_code(Opcode::STORE_FAST, name, stmnt->get_span());

        m_in_function = false;
    }

    GcPtr<Function> CodeGenerator::create_function(FuncDef *stmnt) {
        m_in_function = true;

        auto var = m_scopes->get(stmnt->get_name());

        auto generator = CodeGenerator(m_ctx, m_scopes);
        generator.m_in_function = true;

        m_scopes->new_scope();
        for (auto &[name, span]: stmnt->get_params()) {
            m_scopes->declare(name, span, true, false);
        }

        auto code = generator.generate_code(stmnt->get_body());
        generator.m_in_function = false;

        m_scopes->end_scope();
        m_in_function = false;

        return GarbageCollector::instance().make_immortal<Function>(code, stmnt->get_params(), stmnt->get_name());
    }

    GcPtr<Code> CodeGenerator::generate_code(const std::shared_ptr<Node> &node) {
        m_code = GarbageCollector::instance().make_immortal<Code>();
        node->accept(this);
        finish_generation();
        return m_code;
    }

    void CodeGenerator::visit_return(Return *stmnt) {
        if (stmnt->get_expr().get() == nullptr) {
            m_code->add_code(Opcode::PUSH_NIL, stmnt->get_span());
        } else {
            stmnt->get_expr()->accept(this);
        }

        m_code->add_code(Opcode::RETURN, stmnt->get_span());
    }

    void CodeGenerator::visit_closure_def(ClosureDef *stmnt) {
        bool prev = m_in_closure;
        m_in_closure = true;

        m_in_closure = prev;
    }

    void CodeGenerator::visit_struct(StructNode *stmnt) {
        auto var = m_scopes->get(stmnt->get_name());

        std::vector<GcPtr<String>> ins_var;

        auto params = stmnt->get_params();
        std::for_each(params.begin(), params.end(), [&](const auto &item) {
            ins_var.emplace_back(GarbageCollector::instance().make_immortal<String>(item));
        });

        auto name = GarbageCollector::instance().make_immortal<String>(stmnt->get_name());
        auto _struct = GarbageCollector::instance().make_immortal<Struct>(name, ins_var);
        auto idx = m_code->add_constant(_struct);


        for (auto &meth: stmnt->get_methods()) {
            auto func = dynamic_cast<FuncDef *>(meth.get());
            auto function = create_function(func);
            auto func_name = GarbageCollector::instance().make_immortal<String>(func->get_name());
            _struct->add_method(func_name, function);
        }

        m_code->add_code(Opcode::CREATE_STRUCT, idx, stmnt->get_span());
        auto struct_idx = m_code->add_constant<String>(stmnt->get_name());

        if (var.has_value()) {
            m_code->add_code(Opcode::CREATE_GLOBAL, struct_idx, stmnt->get_span());
            return;
        }
        m_code->add_code(Opcode::STORE_FAST, struct_idx, stmnt->get_span());
    }

    void CodeGenerator::visit_get_attribute(GetAttribute *expr) {
        expr->get_expr()->accept(this);
        auto name = m_code->add_constant<String>(expr->get_name());
        m_code->add_code(Opcode::GET_ATTRIBUTE, name, expr->get_span());
    }

    void CodeGenerator::visit_set_attribute(SetAttribute *expr) {
        expr->get_expr()->accept(this);
        auto name = m_code->add_constant<String>(expr->get_name());
        expr->get_value()->accept(this);
        m_code->add_code(Opcode::SET_ATTRIBUTE, name, expr->get_span());

    }

    void CodeGenerator::visit_import(ImportDef *stmnt) {
        auto name = m_code->add_constant<String>(stmnt->get_name());
        auto alias = m_code->add_constant<String>(stmnt->get_alias());

        m_code->add_code(Opcode::LOAD_CONST, name, stmnt->get_span());
        m_code->add_code(Opcode::IMPORT, alias, stmnt->get_span());
    }

    void CodeGenerator::visit_try(Try *stmnt) {
        stmnt->get_expr()->accept(this);
        auto next = m_code->current_index() + 3;
        m_code->add_code(Opcode::TRY, next, stmnt->get_span());
        m_code->add_code(Opcode::RETURN, stmnt->get_span());

    }

} // bond