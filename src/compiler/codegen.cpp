//
// Created by travor on 3/18/23.
//

#include "codegen.h"
#include "../object.h"
#include "parser.h"

namespace bond {

    CodeGenerator::CodeGenerator(Context *ctx, Scopes *scopes)
            : m_ctx(ctx), m_scopes(scopes) {}


    void CodeGenerator::finish_generation(bool can_error) {
//        if (m_code->get_opcodes().empty() or static_cast<Opcode>(m_code->get_opcodes().back()) != Opcode::RETURN) {
//            m_code->add_ins(Opcode::PUSH_NIL, std::make_shared<Span>(0, 0, 0, 0));
//            m_code->add_ins(Opcode::RETURN, std::make_shared<Span>(0, 0, 0, 0));
//        }

        //TODO: the above code sometimes does not work
        m_code->add_ins(Opcode::PUSH_NIL, std::make_shared<Span>(0, 0, 0, 0));
        if (can_error) m_code->add_ins(Opcode::MAKE_OK, std::make_shared<Span>(0, 0, 0, 0));
        m_code->add_ins(Opcode::RETURN, std::make_shared<Span>(0, 0, 0, 0));
    }

    GcPtr<Code> CodeGenerator::generate_code(const std::vector<std::shared_ptr<Node>> &nodes) {
        try {
            m_code = CODE_STRUCT->create_immortal<Code>();

            for (const auto &node: nodes) {
                node->accept(this);
            }

            finish_generation(false);
            return m_code;
        }
        catch (ParserError &err) {
            m_ctx->error(err.span, err.error);
            return m_code;
        }
    }

    void CodeGenerator::visit(BinaryOp *expr) {
        expr->get_left()->accept(this);
        expr->get_right()->accept(this);

        switch (expr->get_op().get_type()) {
            case TokenType::PLUS:
                m_code->add_ins(Opcode::BIN_ADD, expr->get_op().get_span());
                break;
            case TokenType::MINUS:
                m_code->add_ins(Opcode::BIN_SUB, expr->get_op().get_span());
                break;
            case TokenType::STAR:
                m_code->add_ins(Opcode::BIN_MUL, expr->get_op().get_span());
                break;
            case TokenType::SLASH:
                m_code->add_ins(Opcode::BIN_DIV, expr->get_op().get_span());
                break;
            case TokenType::BANG_EQUAL:
                m_code->add_ins(Opcode::NE, expr->get_op().get_span());
                break;
            case TokenType::EQUAL_EQUAL:
                m_code->add_ins(Opcode::EQ, expr->get_op().get_span());
                break;
            case TokenType::LESS:
                m_code->add_ins(Opcode::LT, expr->get_op().get_span());
                break;
            case TokenType::LESS_EQUAL:
                m_code->add_ins(Opcode::LE, expr->get_op().get_span());
                break;
            case TokenType::GREATER:
                m_code->add_ins(Opcode::GT, expr->get_op().get_span());
                break;
            case TokenType::GREATER_EQUAL:
                m_code->add_ins(Opcode::GE, expr->get_op().get_span());
                break;
            case TokenType::OR:
                m_code->add_ins(Opcode::OR, expr->get_op().get_span());
                break;
            case TokenType::AND:
                m_code->add_ins(Opcode::AND, expr->get_op().get_span());
                break;
            case TokenType::MOD:
                m_code->add_ins(Opcode::BIN_MOD, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_OR:
                m_code->add_ins(Opcode::BIT_OR, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_AND:
                m_code->add_ins(Opcode::BIT_AND, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_XOR:
                m_code->add_ins(Opcode::BIT_XOR, expr->get_op().get_span());
                break;
            default:
                break;
        }
    }

    void CodeGenerator::visit(Unary *expr) {
        expr->get_expr()->accept(this);
        switch (expr->get_op().get_type()) {
            case TokenType::MINUS:
                m_code->add_ins(Opcode::UNARY_SUB, expr->get_op().get_span());
                break;
            case TokenType::BANG:
                m_code->add_ins(Opcode::NOT, expr->get_op().get_span());
                break;
            default:
                break;
        }
    }

    void CodeGenerator::visit(TrueLiteral *expr) {
        m_code->add_ins(Opcode::PUSH_TRUE, expr->get_span());
    }

    void CodeGenerator::visit(FalseLiteral *expr) {
        m_code->add_ins(Opcode::PUSH_FALSE, expr->get_span());
    }

    void CodeGenerator::visit(NumberLiteral *expr) {
        size_t idx = 0;

        if (expr->is_int()) idx = m_code->add_constant(INT_STRUCT->create_immortal<Int>(std::stoi(expr->get_value())));
        else idx = m_code->add_constant(FLOAT_STRUCT->create_immortal<Float>(std::stof(expr->get_value())));

        m_code->add_ins(Opcode::LOAD_CONST, idx, expr->get_span());
    }

    void CodeGenerator::visit(StringLiteral *expr) {
        auto idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(expr->get_value()));
        m_code->add_ins(Opcode::LOAD_CONST, idx, expr->get_span());

    }

    void CodeGenerator::visit(NilLiteral *expr) {
        m_code->add_ins(Opcode::PUSH_NIL, expr->get_span());
    }

    void CodeGenerator::visit(ExprStmnt *stmnt) {
        stmnt->get_expr()->accept(this);

        if (!m_is_repl) m_code->add_ins(Opcode::POP_TOP, stmnt->get_span());
    }

    void CodeGenerator::visit(Identifier *expr) {
        auto var = m_scopes->get(expr->get_name());
        auto idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(expr->get_name()));

        if (var.has_value()) {
            auto v = var.value();

            if (v->is_global) {
                m_code->add_ins(Opcode::LOAD_GLOBAL, idx, expr->get_span());
                return;
            }

            m_code->add_ins(Opcode::LOAD_FAST, idx, expr->get_span());
            return;
        }
        m_ctx->error(expr->get_span(), fmt::format("Undefined variable {}", expr->get_name()));
    }

    void CodeGenerator::visit(NewVar *stmnt) {
        stmnt->get_expr()->accept(this);
        auto var = m_scopes->get(stmnt->get_name());
        auto idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_name()));

        if (var.has_value()) {
            m_code->add_ins(Opcode::CREATE_GLOBAL, idx, stmnt->get_span());
            return;
        }

        m_scopes->declare(stmnt->get_name(), stmnt->get_span(), true, false);
        m_code->add_ins(Opcode::CREATE_LOCAL, idx, stmnt->get_span());
    }

    void CodeGenerator::visit(Assign *stmnt) {
        stmnt->get_expr()->accept(this);
        auto var = m_scopes->get(stmnt->get_name());
        auto idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_name()));

        if (var.has_value()) {
            auto v = var.value();
            if (v->is_global) {
                m_code->add_ins(Opcode::STORE_GLOBAL, idx, stmnt->get_span());
                return;
            }
            m_code->add_ins(Opcode::STORE_FAST, idx, stmnt->get_span());
            return;
        }
        m_ctx->error(stmnt->get_span(), "Undefined variable '" + stmnt->get_name() + "'.");
    }

    void CodeGenerator::visit(Block *stmnt) {
        m_scopes->new_scope();
        for (auto &s: stmnt->get_nodes()) {
            s->accept(this);
        }
        m_scopes->end_scope();
    }

    void CodeGenerator::visit(ListLiteral *expr) {
        for (auto &e: expr->get_nodes()) {
            e->accept(this);
        }
        m_code->add_ins(Opcode::BUILD_LIST, expr->get_nodes().size(), expr->get_span());
    }

    void CodeGenerator::visit(GetItem *expr) {
        expr->get_expr()->accept(this);
        expr->get_index()->accept(this);
        m_code->add_ins(Opcode::GET_ITEM, expr->get_span());
    }

    void CodeGenerator::visit(SetItem *expr) {
        expr->get_expr()->accept(this);
        expr->get_index()->accept(this);
        expr->get_value()->accept(this);
        m_code->add_ins(Opcode::SET_ITEM, expr->get_span());
    }

    void CodeGenerator::visit(If *stmnt) {
        stmnt->get_condition()->accept(this);
        auto next = m_code->current_index() + 1;
        m_code->add_ins(Opcode::JUMP_IF_FALSE, next, stmnt->get_span());

        stmnt->get_then()->accept(this);
        m_code->patch_code(next, m_code->current_index() + 2);

        next = m_code->current_index() + 1;
        m_code->add_ins(Opcode::JUMP, next, stmnt->get_span());

        if (stmnt->get_else().has_value()) {
            stmnt->get_else().value()->accept(this);
        }
        m_code->patch_code(next, m_code->current_index());
    }

    void CodeGenerator::visit(While *stmnt) {
        start_loop();
        auto start = m_code->current_index();
        stmnt->get_condition()->accept(this);
        auto next = m_code->current_index() + 1;
        m_code->add_ins(Opcode::JUMP_IF_FALSE, next, stmnt->get_span());

        stmnt->get_statement()->accept(this);
        m_code->add_ins(Opcode::JUMP, start, stmnt->get_span());

        auto end = m_code->current_index();
        m_code->patch_code(next, end);
        finish_loop(end, start);
    }

    void CodeGenerator::visit(Call *expr) {
        expr->get_expr()->accept(this);
        for (auto &e: expr->get_args()) {
            e->accept(this);
        }
        m_code->add_ins(Opcode::CALL, expr->get_args().size(), expr->get_span());
    }

    void CodeGenerator::visit(For *stmnt) {
        start_loop();
        m_scopes->new_scope();

        m_scopes->declare(stmnt->get_name(), stmnt->get_span(), true, false);
        auto local_name = m_code->add_constant(make_string(stmnt->get_name()));

        m_code->add_ins(Opcode::PUSH_NIL, stmnt->get_span());
        m_code->add_ins(Opcode::CREATE_LOCAL, local_name, stmnt->get_span());

        stmnt->get_expr()->accept(this);
        m_code->add_ins(Opcode::ITER, stmnt->get_span());

        auto start = m_code->current_index();
        m_code->add_ins(Opcode::ITER_END, 0, stmnt->get_span());

        m_code->add_ins(Opcode::ITER_NEXT, local_name, stmnt->get_span());

        stmnt->get_statement()->accept(this);
        m_code->add_ins(Opcode::JUMP, start, stmnt->get_span());

        auto end = m_code->current_index();
        m_code->patch_code(start + 1, end);
        finish_loop(end, start);
        m_code->add_ins(Opcode::POP_TOP, stmnt->get_span());
        m_scopes->end_scope();

    }

    void CodeGenerator::visit(FuncDef *stmnt) {
        func_def(stmnt, false);
    }

    void CodeGenerator::func_def(FuncDef *stmnt, bool is_async) {
        m_in_function = true;

        auto var = m_scopes->get(stmnt->get_name());

        auto generator = CodeGenerator(m_ctx, m_scopes);
        generator.m_in_function = true;

        m_scopes->new_scope();
        for (auto &[name, span]: stmnt->get_params()) {
            m_scopes->declare(name, span, true, false);
        }

        auto code = generator.generate_code(stmnt->get_body(), stmnt->can_error());

        if (!instanceof<Block>(stmnt->get_body().get())) {
            code->add_ins(Opcode::RETURN, stmnt->get_span());
        }

        generator.m_in_function = false;

        m_scopes->end_scope();
        auto fn = FUNCTION_STRUCT->create_immortal<Function>(stmnt->get_name(), stmnt->get_params(), code);
        auto idx = m_code->add_constant(fn);
        auto name = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_name()));

        if (is_async) {
            m_code->add_ins(Opcode::MAKE_ASYNC, idx, stmnt->get_span());
        } else {
            m_code->add_ins(Opcode::CREATE_FUNCTION, idx, stmnt->get_span());
        }

        if (var.has_value()) {
            m_code->add_ins(Opcode::CREATE_GLOBAL, name, stmnt->get_span());
            return;
        }
        m_code->add_ins(Opcode::STORE_FAST, name, stmnt->get_span());

        m_in_function = false;

//        finish_generation(stmnt->can_error());
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

        bool is_expr = !instanceof<Block>(stmnt->get_body().get());

        auto code = generator.generate_code(stmnt->get_body(), stmnt->can_error(), !is_expr);

        if (is_expr) {
            code->add_ins(Opcode::RETURN, stmnt->get_span());
        }

        generator.m_in_function = false;

        m_scopes->end_scope();
        m_in_function = false;

        return FUNCTION_STRUCT->create_immortal<Function>(stmnt->get_name(), stmnt->get_params(), code);
    }

    GcPtr<Code> CodeGenerator::generate_code(const std::shared_ptr<Node> &node, bool can_error, bool f_generation) {
        try {
            m_code = CODE_STRUCT->create_immortal<Code>();
            node->accept(this);
            if (f_generation) {
                finish_generation(can_error);
            }
            return m_code;
        }
        catch (ParserError &err) {
            m_ctx->error(err.span, err.error);
            return m_code;
        }
    }

    void CodeGenerator::visit(Return *stmnt) {
        if (stmnt->get_expr().get() == nullptr) {
            m_code->add_ins(Opcode::PUSH_NIL, stmnt->get_span());
        } else {
            stmnt->get_expr()->accept(this);
        }

        m_code->add_ins(Opcode::RETURN, stmnt->get_span());
    }

    void CodeGenerator::visit(ClosureDef *stmnt) {
        bool prev = m_in_closure;
        m_in_closure = true;

        //FIXME: for now copy all locals from parent scope
        auto func = create_function(stmnt->get_func_def().get());
        auto idx = m_code->add_constant(func);

        if (stmnt->is_expression()) {
            m_code->add_ins(Opcode::CREATE_CLOSURE_EX, idx, stmnt->get_span());
        }
        else {
            m_scopes->declare(stmnt->get_name(), stmnt->get_span(), false, false);
            m_code->add_ins(Opcode::CREATE_CLOSURE, idx, stmnt->get_span());
        }

        m_in_closure = prev;
    }

    void CodeGenerator::visit(StructNode *stmnt) {
        auto var = m_scopes->get(stmnt->get_name());
        auto _struct = STRUCT_STRUCT->create_immortal<Struct>(stmnt->get_name(), stmnt->get_params());
        auto idx = m_code->add_constant(_struct);


        for (auto &meth: stmnt->get_methods()) {
            auto func = dynamic_cast<FuncDef *>(meth.get());
            auto function = create_function(func);
            _struct->add_method(func->get_name(), function);
        }

        m_code->add_ins(Opcode::CREATE_STRUCT, idx, stmnt->get_span());
        auto struct_idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_name()));

        if (var.has_value()) {
            m_code->add_ins(Opcode::CREATE_GLOBAL, struct_idx, stmnt->get_span());
            return;
        }
        m_code->add_ins(Opcode::STORE_FAST, struct_idx, stmnt->get_span());
    }

    void CodeGenerator::visit(GetAttribute *expr) {
        expr->get_expr()->accept(this);
        auto name = m_code->add_constant(STRING_STRUCT->create_immortal<String>(expr->get_name()));
        m_code->add_ins(Opcode::GET_ATTRIBUTE, name, expr->get_span());
    }

    void CodeGenerator::visit(SetAttribute *expr) {
        expr->get_expr()->accept(this);
        auto name = m_code->add_constant(STRING_STRUCT->create_immortal<String>(expr->get_name()));
        expr->get_value()->accept(this);
        m_code->add_ins(Opcode::SET_ATTRIBUTE, name, expr->get_span());

    }

    void CodeGenerator::visit(ImportDef *stmnt) {
        auto a_path = stmnt->get_actual_path();

        if (!a_path.empty()) {
            // if get_actual path is set this means that a call to new_module
            // has already been made and will return the same module id

            auto pre_compiled_id = m_ctx->new_module(a_path);
            auto alias = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_alias()));

            m_code->add_ins(Opcode::LOAD_CONST, alias, stmnt->get_span());
            m_code->add_ins(Opcode::IMPORT_PRE_COMPILED, pre_compiled_id, stmnt->get_span());
            return;
        }

        auto name = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_name()));
        auto alias = m_code->add_constant(STRING_STRUCT->create_immortal<String>(stmnt->get_alias()));

        m_code->add_ins(Opcode::LOAD_CONST, name, stmnt->get_span());
        m_code->add_ins(Opcode::IMPORT, alias, stmnt->get_span());
    }

    void CodeGenerator::visit(Try *stmnt) {
        stmnt->get_expr()->accept(this);
        auto next = m_code->current_index() + 3;
        m_code->add_ins(Opcode::TRY, next, stmnt->get_span());
        m_code->add_ins(Opcode::RETURN, stmnt->get_span());

    }

    void CodeGenerator::visit(Break *stmnt) {
        auto next = m_code->current_index() + 1;
        m_code->add_ins(Opcode::BREAK, next, stmnt->get_span());
        m_break_stack.back().push_back(next);
    }

    void CodeGenerator::visit(Continue *stmnt) {
        auto next = m_code->current_index() + 1;
        m_code->add_ins(Opcode::CONTINUE, next, stmnt->get_span());
        m_continue_stack.back().push_back(next);
    }

    void CodeGenerator::finish_loop(uint32_t loop_end, uint32_t loop_start) {

        auto &breaks = m_break_stack.back();
        auto &continues = m_continue_stack.back();

        for (auto idx: breaks) {
            m_code->patch_code(idx, loop_end);
        }

        for (auto idx: continues) {
            m_code->patch_code(idx, loop_start);
        }

        m_break_stack.pop_back();
        m_continue_stack.pop_back();
    }

    void CodeGenerator::start_loop() {
        m_break_stack.emplace_back();
        m_continue_stack.emplace_back();
    }

    void CodeGenerator::visit(AsyncDef *stmnt) {
        func_def(dynamic_cast<FuncDef *>(stmnt->get_function().get()), true);
        m_ctx->error(stmnt->get_span(), "async def incomplete implementation");
    }

    void CodeGenerator::visit(Await *expr) {
        expr->get_expr()->accept(this);
        m_code->add_ins(Opcode::AWAIT, expr->get_span());
        m_ctx->error(expr->get_span(), "await incomplete implementation");

    }

    void CodeGenerator::visit(StructuredAssign *stmnt) {
        std::vector<std::pair<std::string, bool>> targets;

        for (auto &target: stmnt->get_targets()) {
            auto t = dynamic_cast<Identifier *>(target.get());
            auto is_glob = m_scopes->is_global(t->get_name());
            targets.emplace_back(t->get_name(), is_glob);
        }

        stmnt->get_value()->accept(this);
        m_code->add_ins(Opcode::UNPACK_SEQ, targets.size(), stmnt->get_span());

        for (auto &[name, is_global]: std::ranges::reverse_view(targets)) {
            auto idx = m_code->add_constant(STRING_STRUCT->create_immortal<String>(name));

            if (is_global) {
                m_code->add_ins(Opcode::CREATE_GLOBAL, idx, stmnt->get_span());
            } else {
                // TODO: this might be safe to do as the parser should have already
                //  checked that the name is not already declared
                // we do this for local variables as we did not keep track of the
                // local variables in the scope
                // in the future we should keep track of the local variables
                m_scopes->declare(name, stmnt->get_span(), true);
                m_code->add_ins(Opcode::CREATE_LOCAL, idx, stmnt->get_span());
            }

        }

    }

    void CodeGenerator::visit(CallMethod *expr) {
        auto get_attr = expr->get_node();
        get_attr->get_expr()->accept(this);

        auto name = m_code->add_constant(STRING_STRUCT->create_immortal<String>(get_attr->get_name()));
        m_code->add_ins(Opcode::LOAD_CONST, name, expr->get_span());

        for (auto &arg: expr->get_args()) {
            arg->accept(this);
        }

        m_code->add_ins(Opcode::CALL_METHOD, expr->get_args().size(), expr->get_span());
    }

    void CodeGenerator::visit(ResultStatement *expr) {
        expr->get_expr()->accept(this);
        m_code->add_ins(expr->is_error() ? Opcode::MAKE_ERROR : Opcode::MAKE_OK, expr->get_span());
        m_code->add_ins(Opcode::RETURN, expr->get_span());
    }

    void CodeGenerator::visit(DictLiteral *expr) {
        for (auto &e: expr->get_pairs()) {
            e.first->accept(this);
            e.second->accept(this);
        }
        m_code->add_ins(Opcode::BUILD_DICT, expr->get_pairs().size(), expr->get_span());
    }


} // bond