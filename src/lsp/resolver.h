//
// Created by Travor Oguna Oneya on 21/05/2023.
//

#pragma once

#include <utility>

#include "../bond.h"
#include <variant>
#include "uri.hh"

namespace bond::lsp {
    class Any : NativeInstance {

    };

    static GcPtr<NativeStruct> ANY_STRUCT;

    struct Symbol;


    struct FunctionSymbol {
        FunctionSymbol() = default;

        FunctionSymbol(std::vector<std::shared_ptr<Symbol>> params, std::shared_ptr<Symbol> return_type)
                : m_return_type(std::move(return_type)), m_params(std::move(params)) {}

        std::shared_ptr<Symbol> m_return_type;
        std::vector<std::shared_ptr<Symbol>> m_params;
    };

    struct StructSymbol {
        StructSymbol() = default;

        StructSymbol(std::unordered_map<t_string, std::shared_ptr<Symbol>> fields,
                     std::unordered_map<t_string, std::shared_ptr<Symbol>> methods) : m_fields(
                std::move(fields)), m_methods(std::move(methods)) {}

        std::vector<std::shared_ptr<Symbol>> m_fields_vec;
        std::unordered_map<t_string, std::shared_ptr<Symbol>> m_fields;
        std::unordered_map<t_string, std::shared_ptr<Symbol>> m_methods;
    };


    struct ResultSymbol {
        ResultSymbol() = default;

        std::shared_ptr<Symbol> m_ok;
        std::shared_ptr<Symbol> m_err;
    };

    struct InstanceSymbol {
        InstanceSymbol(std::shared_ptr<Symbol>& sym) { m_struct = std::move(sym); }
        std::shared_ptr<Symbol> m_struct;
    };

    struct Symbol {
        enum class Kind {
            NATIVE_STRUCT,
            RESULT,
            STRUCT,
            FUNCTION,
            INSTANCE
        };

        t_string m_name;
        Kind m_kind;

        GcPtr<NativeStruct> m_native_struct;
        std::shared_ptr<ResultSymbol> m_result;
        std::shared_ptr<StructSymbol> m_struct;
        std::shared_ptr<FunctionSymbol> m_function;
        std::shared_ptr<InstanceSymbol> m_instance;

        explicit Symbol(t_string name) : m_name(std::move(name)) {
            m_kind = Kind::NATIVE_STRUCT;
            m_native_struct = ANY_STRUCT;
        }

        explicit Symbol(GcPtr<NativeStruct> symbol) : m_native_struct(std::move(symbol)) {
            m_kind = Kind::NATIVE_STRUCT;
        }

        explicit Symbol(std::shared_ptr<ResultSymbol> result) : m_result(std::move(result)) {
            m_kind = Kind::RESULT;
        }

        explicit Symbol(const t_string& name, std::shared_ptr<StructSymbol> struct_) : m_struct(std::move(struct_)) {
            m_kind = Kind::STRUCT;
            m_name = name;
        }

        explicit Symbol(std::shared_ptr<FunctionSymbol> function) : m_function(std::move(function)) {
            m_kind = Kind::FUNCTION;
        }

        explicit Symbol(std::shared_ptr<InstanceSymbol> instance) : m_instance(std::move(instance)) {
            m_kind = Kind::INSTANCE;
        }

        [[nodiscard]] t_string to_string() const {
            switch (m_kind) {
                case Kind::NATIVE_STRUCT:
                    return m_native_struct->get_name();
                case Kind::RESULT:
                    return fmt::format("!<{}, {}>", m_result->m_ok->to_string(), m_result->m_err->to_string());
                case Kind::STRUCT:
                    return m_name;
                case Kind::FUNCTION: {
                    t_string params;
                    for (auto& param : m_function->m_params) {
                        params += param->to_string() + ", ";
                    }
                    return fmt::format("fn({}) -> {}", params, m_function->m_return_type->to_string());
                }
                case Kind::INSTANCE:
                    return fmt::format("<Instance of {}>", m_instance->m_struct->to_string());
            }

            return "Unknown";
        }
    };

    static std::shared_ptr<Symbol> INT_SYMBOL;
    static std::shared_ptr<Symbol> FLOAT_SYMBOL;
    static std::shared_ptr<Symbol> STRING_SYMBOL;
    static std::shared_ptr<Symbol> LIST_SYMBOL;
    static std::shared_ptr<Symbol> HASHMAP_SYMBOL;
    static std::shared_ptr<Symbol> BOOL_SYMBOL;
    static std::shared_ptr<Symbol> NIL_SYMBOL;
    static std::shared_ptr<Symbol> ANY_SYMBOL;

    void init_symbols();

    struct ResolveError {
        t_string m_message;
        SharedSpan m_span;

        ResolveError(t_string message, SharedSpan span) : m_message(std::move(message)), m_span(std::move(span)) {}
    };

    class Resolver : public NodeVisitor {
    public:
        Resolver(Context *context, std::vector<SharedNode> &nodes) : m_context(context), m_nodes(nodes) {}

        // what am I doing here?
        std::expected<std::unordered_map<t_string, std::shared_ptr<Symbol>>, std::vector<ResolveError>> resolve();

        void visit(BinaryOp *expr) override;

        void visit(Unary *expr) override;

        void visit(TrueLiteral *expr) override;

        void visit(FalseLiteral *expr) override;

        void visit(NumberLiteral *expr) override;

        void visit(StringLiteral *expr) override;

        void visit(NilLiteral *expr) override;

        void visit(ExprStmnt *stmnt) override;

        void visit(Identifier *expr) override;

        void visit(NewVar *stmnt) override;

        void visit(Assign *stmnt) override;

        void visit(Block *stmnt) override;

        void visit(ListLiteral *expr) override;

        void visit(GetItem *expr) override;

        void visit(SetItem *expr) override;

        void visit(If *stmnt) override;

        void visit(While *stmnt) override;

        void visit(Call *expr) override;

        void visit(For *stmnt) override;

        void visit(FuncDef *stmnt) override;

        void visit(Return *stmnt) override;

        void visit(ClosureDef *stmnt) override;

        void visit(StructNode *stmnt) override;

        void visit(GetAttribute *expr) override;

        void visit(SetAttribute *expr) override;

        void visit(ImportDef *stmnt) override;

        void visit(Try *stmnt) override;

        void visit(Break *stmnt) override;

        void visit(Continue *stmnt) override;

        void visit(AsyncDef *stmnt) override;

        void visit(Await *expr) override;

        void visit(StructuredAssign *stmnt) override;

        void visit(CallMethod *expr) override;

        void visit(ResultStatement *expr) override;

        void visit(DictLiteral *expr) override;


//        void push(const std::shared_ptr<Type> &type);
//        std::shared_ptr<Type> pop();

    private:
        Context *m_context;
        t_string m_file_uri;
        std::unordered_map<t_string, std::shared_ptr<Symbol>> m_symbols;
        std::vector<std::shared_ptr<Symbol>> type_stack;

        void push(const std::shared_ptr<Symbol> &type);

        std::shared_ptr<Symbol> pop();

        std::vector<ResolveError> m_errors;
        std::vector<SharedNode> m_nodes;

        void new_scope();

        void pop_scope();

        void fill_global_scope();

        void declare(const t_string &name, const std::shared_ptr<Symbol> &symbol);

        std::vector<std::unordered_map<t_string, std::shared_ptr<Symbol>>> m_scopes;

        void declare(const t_string &name, const std::shared_ptr<Symbol> &symbol, const SharedSpan &span);

        void add_error(const t_string &message, const SharedSpan &span);

        std::shared_ptr<Symbol> get_symbol(const t_string &name, const SharedSpan &span);

        std::shared_ptr<Symbol>
        resolve_and_compare(const std::shared_ptr<TypeNode> &sym, const std::shared_ptr<Symbol> &expr_type,
                            const SharedSpan &span);

        std::shared_ptr<Symbol> map_type_or_create(const std::shared_ptr<TypeNode> &sym, const SharedSpan &span);

        bool compare_types(const std::shared_ptr<Symbol> &expected, const std::shared_ptr<Symbol> &expr_type,
                           const SharedSpan &span);

        std::shared_ptr<Symbol> visit_func(FuncDef *stmnt);
    };

}
