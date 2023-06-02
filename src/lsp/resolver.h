//
// Created by Travor Oguna Oneya on 21/05/2023.
//

#pragma once
#include <utility>

#include "../bond.h"
#include "uri.hh"

namespace bond::lsp {
    enum class TypeType{
        Multi,
        List,
        Object,
        Function,
        Struct,
        Closure,
        Unknown
    };

    struct Type {
        Type() = default;
        virtual ~Type() = default;
        std::shared_ptr<Span> span;
        std::string name;
        TypeType type_type = TypeType::Unknown;
    };

    struct MultiType : Type {
        MultiType(std::string name, std::shared_ptr<Span> span, std::vector<std::shared_ptr<Type>> possible_types);
        std::vector<std::shared_ptr<Type>> possible_types;
    };

    struct ListType: Type {
        ListType(std::string name, std::shared_ptr<Span> span, std::shared_ptr<Type> element_type);
        std::shared_ptr<Type> element_type;
    };


    struct ObjectType : Type {
        ObjectType(std::string name, std::shared_ptr<Span> span);
        std::vector<std::shared_ptr<Type>> attributes;
    };

    struct FunctionType : Type {
        FunctionType(std::string name, std::shared_ptr<Span> span, std::vector<std::shared_ptr<Type>> params, std::shared_ptr<Type> return_type);
        std::vector<std::shared_ptr<Type>> params;
        std::shared_ptr<Type> return_type;
    };

    struct StructType : Type {
        StructType(std::string name, std::shared_ptr<Span> span, std::vector<std::shared_ptr<Type>> attributes);
        std::vector<std::shared_ptr<Type>> attributes;
    };

    struct Sym{
        Sym(const std::string& name, std::shared_ptr<Span> span);
        Sym(const std::string& name, std::shared_ptr<Span> span, std::shared_ptr<Type> possible_types);

        std::string name;
        std::shared_ptr<Span> span;
        std::shared_ptr<Type> type;
    };


    class Resolver: NodeVisitor {
    public:
        Resolver(Context* context, std::string file_uri) : m_context(context), m_file_uri(std::move(file_uri)) {}
        static std::string open_file(const std::string& file_uri);


        void visit_bin_op(BinaryOp *expr) override;

        void visit_unary(Unary *expr) override;

        void visit_true_lit(TrueLiteral *expr) override;

        void visit_false_lit(FalseLiteral *expr) override;

        void visit_num_lit(NumberLiteral *expr) override;

        void visit_string_lit(StringLiteral *expr) override;

        void visit_nil_lit(NilLiteral *expr) override;

        void visit_expr_stmnt(ExprStmnt *stmnt) override;

        void visit_identifier(Identifier *expr) override;

        void visit_new_var(NewVar *stmnt) override;

        void visit_assign(Assign *stmnt) override;

        void visit_block(Block *stmnt) override;

        void visit_list(List *expr) override;

        void visit_get_item(GetItem *expr) override;

        void visit_set_item(SetItem *expr) override;

        void visit_if(If *stmnt) override;

        void visit_while(While *stmnt) override;

        void visit_call(Call *expr) override;

        void visit_for(For *stmnt) override;

        void visit_func_def(FuncDef *stmnt) override;

        void visit_return(Return *stmnt) override;

        void visit_closure_def(ClosureDef *stmnt) override;

        void visit_struct(StructNode *stmnt) override;

        void visit_get_attribute(GetAttribute *expr) override;

        void visit_set_attribute(SetAttribute *expr) override;

        void visit_import(ImportDef *stmnt) override;

        void visit_try(Try *stmnt) override;

        void visit_break(Break *stmnt) override;

        void visit_continue(Continue *stmnt) override;

        void visit_async_def(AsyncDef *stmnt) override;

        void visit_await(Await *expr) override;

        void visit_structured_assign(StructuredAssign *stmnt) override;

        void push(const std::shared_ptr<Type> &type);
        std::shared_ptr<Type> pop();

    private:
        Context* m_context;
        std::string m_file_uri;
        std::unordered_map<std::string, std::shared_ptr<Sym>> m_symbols;
        std::vector<std::shared_ptr<Type>> type_stack;
    };

}
