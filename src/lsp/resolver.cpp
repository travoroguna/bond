//
// Created by Travor Oguna Oneya on 21/05/2023.
//

#include <iostream>
#include "resolver.h"


namespace bond::lsp {

    void Resolver::visit(BinaryOp *expr) {
//        assert(false && "not implemented");
        expr->get_left()->accept(this);
        expr->get_right()->accept(this);

        auto right = pop();
        auto left = pop();

        if (!compare_types(right, left, expr->get_span())) {
            push(left);
        }
        push(left);
    }

    void Resolver::visit(Unary *expr) {
        //TODO: implement
        expr->get_expr()->accept(this);
        push(pop());
    }

    void Resolver::visit(TrueLiteral *expr) {
        push(BOOL_SYMBOL);
    }

    void Resolver::visit(FalseLiteral *expr) {
        push(BOOL_SYMBOL);
    }

    void Resolver::visit(NumberLiteral *expr) {
        if (expr->is_int()) {
            push(INT_SYMBOL);
            return;
        }
        push(FLOAT_SYMBOL);
    }

    void Resolver::visit(StringLiteral *expr) {
        push(STRING_SYMBOL);
    }

    void Resolver::visit(NilLiteral *expr) {
        push(NIL_SYMBOL);
    }

    void Resolver::visit(ExprStmnt *stmnt) {
        stmnt->get_expr()->accept(this);
        pop();
    }

    void Resolver::visit(Identifier *expr) {
        push(get_symbol(expr->get_name(), expr->get_span()));
    }

    void Resolver::visit(NewVar *stmnt) {
        stmnt->get_expr()->accept(this);
        auto expr_type = pop();

        auto declared_type = stmnt->get_type();

        if (!declared_type.has_value()) {
            declare(stmnt->get_name(), expr_type, stmnt->get_span());
            return;
        }

        auto sym = declared_type.value();
        auto sym_b = resolve_and_compare(sym, expr_type, stmnt->get_span());

        declare(stmnt->get_name(), sym_b, stmnt->get_span());
    }

    void Resolver::visit(Assign *stmnt) {
        stmnt->get_expr()->accept(this);
        auto expr_type = pop();

        auto declared_type = get_symbol(stmnt->get_name(), stmnt->get_span());

        if (!compare_types(declared_type, expr_type, stmnt->get_span())) {
            add_error(fmt::format("TypeError expected {}, got {}", declared_type->to_string(), expr_type->to_string()),
                      stmnt->get_span());
        }
        push(expr_type);
    }

    void Resolver::visit(Block *stmnt) {
        new_scope();

        for (auto &node: stmnt->get_nodes()) {
            node->accept(this);
        }

        pop_scope();
    }

    void Resolver::visit(ListLiteral *expr) {
        for (auto &item: expr->get_nodes()) {
            item->accept(this);
        }

        //TODO: implement
        std::vector<std::shared_ptr<Symbol>> types;

        for (auto i = 0; i < expr->get_nodes().size(); i++) {
            types.insert(types.begin(), pop());
        }

        push(LIST_SYMBOL);
    }

    void Resolver::visit(GetItem *expr) {
        //TODO: implement
        expr->get_expr()->accept(this);
        expr->get_index()->accept(this);

        auto index = pop();
        auto e = pop();

        switch (e->m_kind) {
            case Symbol::Kind::NATIVE_STRUCT:
                if (!e->m_native_struct->has_method("__getitem__")) {
                    add_error(
                            fmt::format("can not get item on {}, __getitem__ is not defined for type", e->to_string()),
                            expr->get_span());
                }
                break;
            case Symbol::Kind::STRUCT:
                if (!e->m_struct->m_methods.contains("__getitem__")) {
                    add_error(
                            fmt::format("can not get item on {}, __getitem__ is not defined for type", e->to_string()),
                            expr->get_span());
                }
                break;
            case Symbol::Kind::INSTANCE:
                break;
            case Symbol::Kind::FUNCTION:
            case Symbol::Kind::RESULT:
                add_error(fmt::format("can not get item on {}", e->to_string()), expr->get_span());
        }

        push(ANY_SYMBOL);
    }

    void Resolver::visit(SetItem *expr) {
        //TODO: implement
        push(ANY_SYMBOL);
    }

    void Resolver::visit(If *stmnt) {
        new_scope();
        stmnt->get_then()->accept(this);
        pop_scope();

        new_scope();
        auto else_stmnt = stmnt->get_else();
        if (else_stmnt.has_value()) {
            else_stmnt.value()->accept(this);
        }
        pop_scope();
    }

    void Resolver::visit(While *stmnt) {
        new_scope();
        stmnt->get_statement()->accept(this);
        pop_scope();
    }

    void Resolver::visit(Call *expr) {
        expr->get_expr()->accept(this);
        auto expr_type = pop();

        for (auto &arg: expr->get_args()) {
            arg->accept(this);
        }

        std::vector<std::shared_ptr<Symbol>> arg_types;

        for (auto i = 0; i < expr->get_args().size(); i++) {
            arg_types.insert(arg_types.begin(), pop());
        }

        switch (expr_type->m_kind) {
            case Symbol::Kind::NATIVE_STRUCT: {
                auto symbols = std::vector{INT_SYMBOL, FLOAT_SYMBOL, BOOL_SYMBOL, STRING_SYMBOL, LIST_SYMBOL,
                                           NIL_SYMBOL};

                for (auto &k: symbols) {
                    if (compare_types(k, expr_type, expr->get_span())) {
                        add_error(fmt::format("cannot call {}", k->to_string()), expr->get_span());
                        break;
                    }
                }

                //we do not know the type of the function
                // in the future we could parse the doc
                push(std::make_shared<Symbol>(std::make_shared<InstanceSymbol>(expr_type)));
                break;
            }
            case Symbol::Kind::RESULT:
                add_error("cannot call result", expr->get_span());
                break;

            case Symbol::Kind::STRUCT: {
                if (arg_types.size() != expr_type->m_struct->m_fields.size()) {
                    add_error(fmt::format("Expected {} arguments, got {}",
                                          expr_type->m_struct->m_fields.size(), arg_types.size()), expr->get_span());
                    break;
                }

                for (auto i = 0; i < arg_types.size(); i++) {
                    auto &arg_type = arg_types[i];
                    auto &field = expr_type->m_struct->m_fields_vec[i];

                    if (!compare_types(field, arg_type, expr->get_span())) {
                        add_error(
                                fmt::format("type mismatch for argument {}, expected {} got {}", i, field->to_string(),
                                            arg_type->to_string()), expr->get_span());
                    }
                }

                push(std::make_shared<Symbol>(std::make_shared<InstanceSymbol>(expr_type)));
                break;
            }
            case Symbol::Kind::FUNCTION: {
                if (arg_types.size() != expr_type->m_function->m_params.size()) {
                    add_error(fmt::format("Expected {} arguments, got {}", expr_type->m_function->m_params.size(),
                                          arg_types.size()), expr->get_span());
                    break;
                }

                for (auto i = 0; i < arg_types.size(); i++) {
                    auto &arg_type = arg_types[i];
                    auto &arg = expr_type->m_function->m_params[i];

                    if (!compare_types(arg, arg_type, expr->get_span())) {
                        add_error(fmt::format("type mismatch for argument {}", i), expr->get_span());
                    }
                }

                push(expr_type->m_function->m_return_type);
                break;
            }
            case Symbol::Kind::INSTANCE:
                add_error("cannot call instance", expr->get_span());
                break;
        }

        push(ANY_SYMBOL);
    }

    void Resolver::visit(For *stmnt) {
        new_scope();

        for (auto &var: stmnt->get_variables()) {
            declare(var.first, ANY_SYMBOL, stmnt->get_span());
        }
        stmnt->get_statement()->accept(this);
        pop_scope();
    }

    void Resolver::visit(FuncDef *stmnt) {
        auto func = visit_func(stmnt);
        declare(stmnt->get_name(), func, stmnt->get_span());
    }

    std::shared_ptr<Symbol> Resolver::visit_func(FuncDef *stmnt) {
        new_scope();

        auto func_type = std::make_shared<Symbol>(std::make_shared<FunctionSymbol>());
        push(func_type);

        auto func = func_type->m_function;

        for (auto &param: stmnt->get_params()) {
            if (param->type.has_value()) {
                func->m_params.push_back(map_type_or_create(param->type.value(), stmnt->get_span()));
                declare(param->name, func->m_params.back(), stmnt->get_span());
            } else {
                func->m_params.push_back(ANY_SYMBOL);
                declare(param->name, ANY_SYMBOL, stmnt->get_span());
            }
        }

        auto ret_t = stmnt->get_return_type();

        if (ret_t.has_value()) {
            func->m_return_type = map_type_or_create(ret_t.value(), stmnt->get_span());
        } else {
            func->m_return_type = ANY_SYMBOL;
        }

        //temp add the function to the scope

        declare(stmnt->get_name(), func_type, stmnt->get_span());

        stmnt->get_body()->accept(this);

        pop_scope();

        return func_type;
    }

    void Resolver::visit(Return *stmnt) {
        stmnt->get_expr()->accept(this);
    }

    void Resolver::visit(ClosureDef *stmnt) {
        new_scope();

        auto the_func = stmnt->get_func_def();

        auto func_type = visit_func(the_func.get());

        if (stmnt->is_expression()) {
            push(func_type);
        } else {
            declare(stmnt->get_name(), func_type, stmnt->get_span());
        }
        pop_scope();
    }

    void Resolver::visit(StructNode *stmnt) {
        new_scope();

        auto struct_type = std::make_shared<Symbol>(stmnt->get_name(), std::make_shared<StructSymbol>());
        push(struct_type);

        auto struct_s = struct_type->m_struct;

        for (auto &field: stmnt->get_params()) {
            if (field->type.has_value()) {
                auto val = map_type_or_create(field->type.value(), field->span);
                struct_s->m_fields[field->name] = val;
                struct_s->m_fields_vec.push_back(val);
            } else {
                struct_s->m_fields[field->name] = ANY_SYMBOL;
                struct_s->m_fields_vec.push_back(ANY_SYMBOL);
            }
        }

        declare(stmnt->get_name(), struct_type, stmnt->get_span());


        for (auto &meth: stmnt->get_methods()) {
            auto func_def = dynamic_pointer_cast<FuncDef>(meth).get();
            auto func_type = visit_func(func_def);
            struct_s->m_methods[func_def->get_name()] = func_type;
        }


        pop_scope();
    }

    void Resolver::visit(GetAttribute *expr) {
//        assert(false && "not implemented");
        //TODO: implement
        push(ANY_SYMBOL);
    }

    void Resolver::visit(SetAttribute *expr) {
//        assert(false && "not implemented");
        //TODO: implement
        push(ANY_SYMBOL);
    }

    void Resolver::visit(ImportDef *stmnt) {
//        assert(false && "not implemented");
        //TODO: implement
        declare(stmnt->get_alias(), ANY_SYMBOL, stmnt->get_span());
    }


    void Resolver::visit(Try *stmnt) {
//        assert(false && "not implemented");
        //TODO: implement
        stmnt->get_expr()->accept(this);
    }

    void Resolver::visit(Break *stmnt) {
//        assert(false && "not implemented");
    }

    void Resolver::visit(Continue *stmnt) {
//        assert(false && "not implemented");
    }

    void Resolver::visit(AsyncDef *stmnt) {
//        assert(false && "not implemented");
    }

    void Resolver::visit(Await *expr) {
//        assert(false && "not implemented");
    }

    void Resolver::visit(StructuredAssign *stmnt) {
        // TODO: check that the types match
    }

    void Resolver::visit(CallMethod *expr) {
        //FIXME: this is a hack
        push(ANY_SYMBOL);
    }

    void Resolver::visit(ResultStatement *expr) {
        expr->get_expr()->accept(this);
    }

    void Resolver::visit(DictLiteral *expr) {
        push(HASHMAP_SYMBOL);
    }


    bool Resolver::compare_types(const std::shared_ptr<Symbol> &expected, const std::shared_ptr<Symbol> &expr_type,
                                 const SharedSpan &span) {
        if (expected->m_kind == Symbol::Kind::NATIVE_STRUCT) {
            if (expected->m_native_struct.get() == ANY_STRUCT.get()) {
                return true;
            }
        }

//        if (expected == expr_type) {
//            return true;
//        }

        switch (expected->m_kind) {
            case Symbol::Kind::NATIVE_STRUCT: {
                if (expr_type->m_kind == Symbol::Kind::NATIVE_STRUCT) {
                    // native structs are unique, so we can compare pointers
                    if (expected->m_native_struct.get() == expr_type->m_native_struct.get()) {
                        return true;
                    }
                }
                break;
            }
            case Symbol::Kind::RESULT: {
                if (expr_type->m_kind == Symbol::Kind::RESULT) {
                    auto expected_result = expected->m_result;
                    auto expr_result = expr_type->m_result;
                    return compare_types(expected_result->m_ok, expr_result->m_ok, span);
                }

                auto ok = compare_types(expected->m_result->m_ok, expr_type, span);
                auto err = compare_types(expected->m_result->m_err, expr_type, span);

                if (ok || err) {
                    return true;
                }

                break;
            }
            case Symbol::Kind::STRUCT: {
                // this should also be unique within each module
                if (expected->m_struct == expr_type->m_struct) {
                    return true;
                }
                break;
            }
            case Symbol::Kind::FUNCTION: {
                if (expr_type->m_kind == Symbol::Kind::FUNCTION) {
                    auto expected_func = expected->m_function;
                    auto expr_func = expr_type->m_function;

                    if (expected_func->m_params.size() != expr_func->m_params.size()) {
                        add_error(fmt::format("Expected {} got {}", expected->to_string(), expr_type->to_string()),
                                  span);
                        return false;
                    }

                    if (expected_func->m_params.size() == expr_func->m_params.size()) {
                        for (size_t i = 0; i < expected_func->m_params.size(); i++) {
                            if (!compare_types(expected_func->m_params[i], expr_func->m_params[i], span)) {
                                add_error(fmt::format("Expected {} got {}", expected->to_string(),
                                                      expr_type->to_string()), span);
                                return false;
                            }
                        }

                        if (!compare_types(expected_func->m_return_type, expr_func->m_return_type, span)) {
                            add_error(fmt::format("Expected {} got {}", expected->to_string(), expr_type->to_string()),
                                      span);
                            return false;
                        }
                    }
                }
                break;
            }
            case Symbol::Kind::INSTANCE: {
                if (expr_type->m_kind == Symbol::Kind::INSTANCE) {
                    return false;
                }

                if (expr_type->m_kind == Symbol::Kind::STRUCT) {
                    if (compare_types(expected->m_instance->m_struct, expr_type->m_instance->m_struct, span)) {
                        return true;
                    }
                }

                add_error(
                        fmt::format("Type mismatch, Expected {} got {}", expected->to_string(), expr_type->to_string()),
                        span);
                break;
            }
        }

        return false;
    }

    std::shared_ptr<Symbol>
    Resolver::resolve_and_compare(const std::shared_ptr<TypeNode> &sym, const std::shared_ptr<Symbol> &expr_type,
                                  const SharedSpan &span) {
        auto known_sym = map_type_or_create(sym, span);

        if (!compare_types(known_sym, expr_type, span)) {
            add_error(fmt::format("TypeError expected {} but got {}", known_sym->to_string(), expr_type->to_string()),
                      span);
        }

        return known_sym;
    }

    std::shared_ptr<Symbol> Resolver::map_type_or_create(const std::shared_ptr<TypeNode> &sym, const SharedSpan &span) {
        // TODO: also check inner types
        if (instanceof<ListType>(sym.get())) {
            return LIST_SYMBOL;
        }
        if (instanceof<FunctionType>(sym.get())) {
            auto fn = std::dynamic_pointer_cast<FunctionType>(sym);
            auto fn_sym = std::make_shared<FunctionSymbol>();
            fn_sym->m_return_type = map_type_or_create(fn->get_ret(), span);

            for (const auto &arg: fn->get_args()) {
                fn_sym->m_params.push_back(map_type_or_create(arg, span));
            }

            return std::make_shared<Symbol>(fn_sym);
        }
        if (instanceof<ResultType>(sym.get())) {
            auto res = std::dynamic_pointer_cast<ResultType>(sym);
            auto res_sym = std::make_shared<ResultSymbol>();
            res_sym->m_ok = map_type_or_create(res->get_ok(), span);
            res_sym->m_err = map_type_or_create(res->get_err(), span);
            return std::make_shared<Symbol>(res_sym);
        }

        // TODO: add missing types, generic, compund

        // check if type is already known
        return get_symbol(sym->get_name(), span);
    }

    std::expected<std::unordered_map<t_string, std::shared_ptr<Symbol>>, std::vector<ResolveError>>
    Resolver::resolve() {
        fill_global_scope();
        for (auto &stmnt: m_nodes) {
            stmnt->accept(this);
        }

        if (m_errors.empty()) {
            return m_scopes[0];
        }

        return std::unexpected(m_errors);
    }

    void Resolver::push(const std::shared_ptr<Symbol> &type) {
        type_stack.push_back(type);
    }

    std::shared_ptr<Symbol> Resolver::pop() {
        assert(!type_stack.empty() && "type stack is empty");
        auto val = type_stack.back();
        type_stack.pop_back();
        return val;
    }

    void Resolver::new_scope() {
        m_scopes.emplace_back();
    }

    void Resolver::pop_scope() {
        m_scopes.pop_back();
    }

    std::shared_ptr<Symbol> Resolver::get_symbol(const t_string &name, const SharedSpan &span) {
        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.contains(name)) {
                return scope[name];
            }
        }
        add_error(fmt::format("variable {} is not declared", name), span);
        return ANY_SYMBOL;
    }

    void Resolver::fill_global_scope() {
        auto globs = Runtime::ins()->MAP_STRUCT->create_instance<StringMap>();
        add_builtins_to_globals(globs);
        new_scope();
        for (auto &[name, _]: globs->get_value()) {
            //TODO: get proper type, for now just use any
            declare(name, ANY_SYMBOL);
        }

        //rewrite known symbols
        declare("Int", INT_SYMBOL);
        declare("Float", FLOAT_SYMBOL);
        declare("Bool", BOOL_SYMBOL);
        declare("String", STRING_SYMBOL);
        declare("List", LIST_SYMBOL);
        declare("HashMap", HASHMAP_SYMBOL);
        declare("List", LIST_SYMBOL);
        declare("Nil", NIL_SYMBOL);
    }

    void Resolver::declare(const t_string &name, const std::shared_ptr<Symbol> &symbol) {
        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            // no span ignore
            if (scope.contains(name)) {
                scope[name] = symbol;
                return;
            }
        }
        m_scopes.back()[name] = symbol;
    }

    void Resolver::declare(const t_string &name, const std::shared_ptr<Symbol> &symbol, const SharedSpan &span) {
        for (auto &scope: std::ranges::reverse_view(m_scopes)) {
            if (scope.contains(name)) {
                add_error(fmt::format("variable {} already declared", name), span);
                return;
            }
        }
        m_scopes.back()[name] = symbol;
    }

    void Resolver::add_error(const t_string &message, const SharedSpan &span) {
        m_errors.emplace_back(message, span);
    }

    void init_symbols() {
        INT_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->INT_STRUCT);
        FLOAT_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->FLOAT_STRUCT);
        STRING_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->STRING_STRUCT);
        LIST_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->LIST_STRUCT);
        HASHMAP_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->HASHMAP_STRUCT);
        BOOL_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->BOOL_STRUCT);
        NIL_SYMBOL = std::make_shared<Symbol>(Runtime::ins()->NONE_STRUCT);
        ANY_STRUCT = make<NativeStruct>("Any", "Any(object: Any)", bond::c_Default<Any>);
        ANY_SYMBOL = std::make_shared<Symbol>(ANY_STRUCT);
    }
}
