//
// Created by Travor Oguna Oneya on 21/05/2023.
//

#include "resolver.h"


namespace bond::lsp {
    const auto default_span = std::make_shared<Span>(0, 0, 0, 0);
    const auto ANY = std::make_shared<ObjectType>("any", default_span);
    const auto INT = std::make_shared<ObjectType>("int", default_span);
    const auto FLOAT = std::make_shared<ObjectType>("float", default_span);
    const auto STRING = std::make_shared<ObjectType>("str", default_span);
    const auto BOOL = std::make_shared<ObjectType>("bool", default_span);
    const auto NIL = std::make_shared<ObjectType>("nil", default_span);

    std::string Resolver::open_file(const std::string &file_uri) {
        auto file_path = uri(file_uri).get_path();
        int index;

        while ((index = file_path.find("%3A")) != std::string::npos) {
            file_path.replace(index, 3, ":");
        }

        return bond::Context::read_file(file_path);
    }

    void Resolver::visit_bin_op(BinaryOp *expr) {
        expr->get_left()->accept(this);
        expr->get_right()->accept(this);

        auto right = pop();
        auto left = pop();

        if (right == left) {
            push(right);
            return;
        }

        push(std::make_shared<MultiType>(fmt::format("{} | {}", left->name, right->name), Parser::span_from_spans(left->span, right->span), std::vector{left, right}));
    }

    void Resolver::push(const std::shared_ptr<Type> &type) {
        type_stack.push_back(type);
    }

    std::shared_ptr<Type> Resolver::pop() {
        auto last = type_stack.back();
        type_stack.pop_back();
        return last;
    }

    void Resolver::visit_unary(Unary *expr) {
        expr->get_expr()->accept(this);
        push(pop());
    }

    void Resolver::visit_true_lit(TrueLiteral *expr) {
        push(BOOL);
    }

    void Resolver::visit_false_lit(FalseLiteral *expr) {
        push(BOOL);
    }

    void Resolver::visit_num_lit(NumberLiteral *expr) {
        push(INT);
    }

    void Resolver::visit_string_lit(StringLiteral *expr) {
        push(STRING);
    }

    void Resolver::visit_nil_lit(NilLiteral *expr) {
        push(NIL);
    }

    void Resolver::visit_expr_stmnt(ExprStmnt *stmnt) {
        stmnt->get_expr()->accept(this);
        push(pop());
    }

    void Resolver::visit_identifier(Identifier *expr) {
        auto name = expr->get_name();
        // TODO: Check if name is in scope

        if (m_symbols.contains(name)) {
            push(m_symbols[name]->type);
        } else {
            push(ANY);
        }
    }

    void Resolver::visit_new_var(NewVar *stmnt) {
        auto name = stmnt->get_name();
        stmnt->get_expr()->accept(this);

        auto type = pop();

        if (m_symbols.contains(name)) {
            m_symbols[name]->type = std::make_shared<MultiType>(fmt::format("{} | {}", m_symbols[name]->type->name, type->name), Parser::span_from_spans(m_symbols[name]->span, type->span), std::vector{m_symbols[name]->type, type});
        }
        else{
            m_symbols[name] = std::make_shared<Sym>(name, stmnt->get_span(), type);
        }
    }

    void Resolver::visit_assign(Assign *stmnt) {
        auto name = stmnt->get_name();
        stmnt->get_expr()->accept(this);

        auto type = pop();

        if (m_symbols.contains(name)) {
            m_symbols[name]->type = std::make_shared<MultiType>(fmt::format("{} | {}", m_symbols[name]->type->name, type->name), Parser::span_from_spans(m_symbols[name]->span, type->span), std::vector{m_symbols[name]->type, type});
        }
        else{
            m_symbols[name] = std::make_shared<Sym>(name, stmnt->get_span(), type);
        }
    }

    void Resolver::visit_block(Block *stmnt) {
        for (auto &stmt : stmnt->get_nodes()) {
            stmt->accept(this);
        }
    }

    void Resolver::visit_list(List *expr) {
        auto types = std::vector<std::shared_ptr<Type>>();

        for (auto &item : expr->get_nodes()) {
            item->accept(this);
            types.push_back(pop());
        }

        auto multi = std::make_shared<MultiType>(fmt::format("[{}]", types[0]->name), Parser::span_from_spans(types[0]->span, types[types.size() - 1]->span), types);
        push(std::make_shared<ListType>(fmt::format("[{}]", types[0]->name), Parser::span_from_spans(types[0]->span, types[types.size() - 1]->span), multi));

    }

    void Resolver::visit_get_item(GetItem *expr) {
        expr->get_expr()->accept(this);
        expr->get_index()->accept(this);

        auto index = pop();
        auto expr_type = pop();

        std::vector<std::shared_ptr<Type>> types;

        switch (index->type_type) {
            case TypeType::Multi:
                for (auto &type : dynamic_cast<MultiType *>(index.get())->possible_types ) {
                    types.push_back(type);
                }
                break;
            case TypeType::List:
                //todo list attributes
                break;
            case TypeType::Object:
                //todo object attributes
                break;
            case TypeType::Function:
                //todo function attributes
                break;
            case TypeType::Struct:
                
                break;
            case TypeType::Closure:
                break;
            case TypeType::Unknown:
                break;
        }


    }


    Sym::Sym(const std::string &name, std::shared_ptr<Span> span) {
        this->name = name;
        this->span = std::move(span);
    }

    Sym::Sym(const std::string &name, std::shared_ptr<Span> span, std::shared_ptr<Type> type) {
        this->name = name;
        this->span = std::move(span);
        this->type = std::move(type);
    }


    ObjectType::ObjectType(std::string name, std::shared_ptr<Span> span) {
        this->name = std::move(name);
        this->span = std::move(span);
        this->type_type = TypeType::Object;
    }

    MultiType::MultiType(std::string name, std::shared_ptr<Span> span,
                         std::vector<std::shared_ptr<Type>> possible_types) {
        this->name = std::move(name);
        this->span = std::move(span);
        this->possible_types = std::move(possible_types);
        this->type_type = TypeType::Multi;
    }

    ListType::ListType(std::string name, std::shared_ptr<Span> span, std::shared_ptr<Type> element_type) {
        this->name = std::move(name);
        this->span = std::move(span);
        this->element_type = std::move(element_type);
        this->type_type = TypeType::List;
    }

    FunctionType::FunctionType(std::string name, std::shared_ptr<Span> span, std::vector<std::shared_ptr<Type>> params,
                               std::shared_ptr<Type> return_type) {
        this->name = std::move(name);
        this->span = std::move(span);
        this->params = std::move(params);
        this->return_type = std::move(return_type);

    }

    StructType::StructType(std::string name, std::shared_ptr<Span> span,
                           std::vector<std::shared_ptr<Type>> attributes) {
        this->name = std::move(name);
        this->span = std::move(span);
        this->attributes = std::move(attributes);
        this->type_type = TypeType::Struct;
    }
}
