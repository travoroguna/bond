//
// Created by travor on 3/18/23.
//

#include "code.h"
#include "object.h"
#include "parser.h"
#include "bfmt.h"

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
                CONSTANT_INSTRUCTION(MAKE_ASYNC);

                CONSTANT_INSTRUCTION(UNPACK_SEQ);
                OPRAND_INSTRUCTION(BUILD_LIST);
                OPRAND_INSTRUCTION(JUMP_IF_FALSE);
                OPRAND_INSTRUCTION(JUMP);
                OPRAND_INSTRUCTION(CALL);
                OPRAND_INSTRUCTION(ITER_END);
                OPRAND_INSTRUCTION(CREATE_FUNCTION);
                OPRAND_INSTRUCTION(CREATE_STRUCT);
                OPRAND_INSTRUCTION(TRY);
                OPRAND_INSTRUCTION(BREAK);
                OPRAND_INSTRUCTION(CONTINUE);


                SIMPLE_INSTRUCTION(BIT_OR);
                SIMPLE_INSTRUCTION(BIT_XOR);
                SIMPLE_INSTRUCTION(BIT_AND);
                SIMPLE_INSTRUCTION(ITER);
                SIMPLE_INSTRUCTION(BIN_ADD);
                SIMPLE_INSTRUCTION(BIN_SUB);
                SIMPLE_INSTRUCTION(BIN_MUL);
                SIMPLE_INSTRUCTION(NOT);
                SIMPLE_INSTRUCTION(UNARY_SUB);
                SIMPLE_INSTRUCTION(BIN_DIV);
                SIMPLE_INSTRUCTION(BIN_MOD);
                SIMPLE_INSTRUCTION(RETURN);
                SIMPLE_INSTRUCTION(PUSH_TRUE);
                SIMPLE_INSTRUCTION(PUSH_FALSE);
                SIMPLE_INSTRUCTION(PUSH_NIL);
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
                SIMPLE_INSTRUCTION(AWAIT);
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
        return Globs::BondTrue;
    }

    uint32_t Code::add_constant(const GcPtr<Object> &obj) {
        m_constants.push_back(obj);
        return m_constants.size() - 1;
    }

    template<typename T, typename... Args>
    uint32_t Code::add_constant(Args &&... args) {
        GarbageCollector::instance().stop_gc();
        auto tmp = GarbageCollector::instance().make<T>(args...);

        if (m_const_map.contains(tmp)) {
            GarbageCollector::instance().resume_gc();
            return m_const_map[tmp];
        }

        auto t = GarbageCollector::instance().make_immortal<T>(
                std::forward<Args>(args)...);
        m_constants.push_back(t);
        m_const_map[tmp] = m_constants.size() - 1;

        GarbageCollector::instance().resume_gc();

        return m_constants.size() - 1;
    }


    void CodeGenerator::finish_generation() {
        if (m_code->get_opcodes().empty()) {
            m_code->add_code(Opcode::PUSH_NIL, std::make_shared<Span>(0, 0, 0, 0));
            m_code->add_code(Opcode::RETURN, std::make_shared<Span>(0, 0, 0, 0));
        } else {
            m_code->add_code(Opcode::PUSH_NIL, m_code->last_span());
            m_code->add_code(Opcode::RETURN, m_code->last_span());
        }
    }

    GcPtr<Code> CodeGenerator::generate_code(const std::vector<std::shared_ptr<Node>> &nodes) {
        try {
            m_code = GarbageCollector::instance().make_immortal<Code>();

            for (const auto &node: nodes) {
                node->accept(this);
            }

            finish_generation();

            std::filesystem::create_directory("build");

            write_code_file("build/main.bdc", m_code);
            auto code = read_code_file("build/main.bdc");
            if (!code) {
                fmt::print("error reading code file, {}\n", code.error());
            }
            fmt::print("{}\n", code.value()->dissasemble());
            return m_code;
        }
        catch (ParserError &err) {
            m_ctx->error(err.span, err.error);
            return m_code;
        }
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
                break;
            case TokenType::MOD:
                m_code->add_code(Opcode::BIN_MOD, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_OR:
                m_code->add_code(Opcode::BIT_OR, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_AND:
                m_code->add_code(Opcode::BIT_AND, expr->get_op().get_span());
                break;
            case TokenType::BITWISE_XOR:
                m_code->add_code(Opcode::BIT_XOR, expr->get_op().get_span());
                break;
            default:
                break;
        }
    }

    void CodeGenerator::visit_unary(Unary *expr) {
        expr->get_expr()->accept(this);
        switch (expr->get_op().get_type()) {
            case TokenType::MINUS:
                m_code->add_code(Opcode::UNARY_SUB, expr->get_op().get_span());
                break;
            case TokenType::BANG:
                m_code->add_code(Opcode::NOT, expr->get_op().get_span());
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
        size_t idx = 0;

        if (expr->is_int()) idx = m_code->add_constant<Integer>(std::stoi(expr->get_value()));
        else idx = m_code->add_constant<Float>(std::stof(expr->get_value()));

        m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());
    }

    void CodeGenerator::visit_string_lit(StringLiteral *expr) {
        auto idx = m_code->add_constant<String>(expr->get_value());
        m_code->add_code(Opcode::LOAD_CONST, idx, expr->get_span());

    }

    void CodeGenerator::visit_nil_lit(NilLiteral *expr) {
        m_code->add_code(Opcode::PUSH_NIL, expr->get_span());
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
        start_loop();
        auto start = m_code->current_index();
        stmnt->get_condition()->accept(this);
        auto next = m_code->current_index() + 1;
        m_code->add_code(Opcode::JUMP_IF_FALSE, next, stmnt->get_span());

        stmnt->get_statement()->accept(this);
        m_code->add_code(Opcode::JUMP, start, stmnt->get_span());

        auto end = m_code->current_index();
        m_code->patch_code(next, end);
        finish_loop(end, start);
    }

    void CodeGenerator::visit_call(Call *expr) {
        expr->get_expr()->accept(this);
        for (auto &e: expr->get_args()) {
            e->accept(this);
        }
        m_code->add_code(Opcode::CALL, expr->get_args().size(), expr->get_span());
    }

    void CodeGenerator::visit_for(For *stmnt) {
        start_loop();
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

        auto end = m_code->current_index();
        m_code->patch_code(start + 1, end);
        finish_loop(end, start);
        m_code->add_code(Opcode::POP_TOP, stmnt->get_span());
        m_scopes->end_scope();

    }

    void CodeGenerator::visit_func_def(FuncDef *stmnt) {
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

        auto code = generator.generate_code(stmnt->get_body());
        generator.m_in_function = false;

        m_scopes->end_scope();
        auto idx = m_code->add_constant<Function>(code, stmnt->get_params(), stmnt->get_name());
        auto name = m_code->add_constant<String>(stmnt->get_name());

        if (is_async) {
            m_code->add_code(Opcode::MAKE_ASYNC, idx, stmnt->get_span());
        } else {
            m_code->add_code(Opcode::CREATE_FUNCTION, idx, stmnt->get_span());
        }

        if (var.has_value()) {
            m_code->add_code(Opcode::CREATE_GLOBAL, name, stmnt->get_span());
            return;
        }
        m_code->add_code(Opcode::STORE_FAST, name, stmnt->get_span());

        m_in_function = false;

        finish_generation();
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
        try {
            m_code = GarbageCollector::instance().make_immortal<Code>();
            node->accept(this);
            finish_generation();
            return m_code;
        }
        catch (ParserError &err) {
            m_ctx->error(err.span, err.error);
            return m_code;
        }
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


    std::expected<std::string, std::string> CodeGenerator::path_resolver(const std::string &path) {
#ifdef _WIN32
        auto test_compiled_native = m_ctx->get_lib_path() + path + ".dll";
        auto test_compiled_c_path = path + ".dll";
#else
        auto lib_p = std::string("lib") + path + ".so";
        auto test_compiled_native = m_ctx->get_lib_path() + lib_p;
        const auto& test_compiled_c_path = lib_p;
#endif

        auto test_native = m_ctx->get_lib_path() + path + ".bd";
        auto test_user = path + ".bd";


        std::array<std::string, 4> paths = {test_compiled_native, test_compiled_c_path, test_native, test_user};

        for (auto &p: paths) {
            fmt::print("[import] test {} {}\n", p, std::filesystem::exists(p));
            if (std::filesystem::exists(p)) {
                return p;
            }
        }
//
        return std::unexpected(fmt::format("failed to resolve path {}", path));
    }

    void CodeGenerator::visit_import(ImportDef *stmnt) {
        auto name = m_code->add_constant<String>(stmnt->get_name());
        auto alias = m_code->add_constant<String>(stmnt->get_alias());

        auto path = path_resolver(stmnt->get_name());

        if (!path.has_value()) {
            m_ctx->error(stmnt->get_span(), path.error());
            return;
        }

        m_code->add_code(Opcode::LOAD_CONST, name, stmnt->get_span());
        m_code->add_code(Opcode::IMPORT, alias, stmnt->get_span());
    }

    void CodeGenerator::visit_try(Try *stmnt) {
        stmnt->get_expr()->accept(this);
        auto next = m_code->current_index() + 3;
        m_code->add_code(Opcode::TRY, next, stmnt->get_span());
        m_code->add_code(Opcode::RETURN, stmnt->get_span());

    }

    void CodeGenerator::visit_break(Break *stmnt) {
        auto next = m_code->current_index() + 1;
        m_code->add_code(Opcode::BREAK, next, stmnt->get_span());
        m_break_stack.back().push_back(next);
    }

    void CodeGenerator::visit_continue(Continue *stmnt) {
        auto next = m_code->current_index() + 1;
        m_code->add_code(Opcode::CONTINUE, next, stmnt->get_span());
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

    void CodeGenerator::visit_async_def(AsyncDef *stmnt) {
        func_def(dynamic_cast<FuncDef *>(stmnt->get_function().get()), true);
        m_ctx->error(stmnt->get_span(), "async def incomplete implementation");
    }

    void CodeGenerator::visit_await(Await *expr) {
        expr->get_expr()->accept(this);
        m_code->add_code(Opcode::AWAIT, expr->get_span());
        m_ctx->error(expr->get_span(), "await incomplete implementation");

    }

    void CodeGenerator::visit_structured_assign(StructuredAssign *stmnt) {
        std::vector<std::pair<std::string, bool>> targets;

        for (auto &target: stmnt->get_targets()) {
            auto t = dynamic_cast<Identifier *>(target.get());
            auto is_glob = m_scopes->is_global(t->get_name());
            targets.emplace_back(t->get_name(), is_glob);
        }

        stmnt->get_value()->accept(this);
        m_code->add_code(Opcode::UNPACK_SEQ, targets.size(), stmnt->get_span());

        for (auto &[name, is_global]: std::ranges::reverse_view(targets)) {
            auto idx = m_code->add_constant<String>(name);

            if (is_global) {
                m_code->add_code(Opcode::CREATE_GLOBAL, idx, stmnt->get_span());
            } else {
                m_code->add_code(Opcode::CREATE_LOCAL, idx, stmnt->get_span());
            }

        }

    }

} // bond