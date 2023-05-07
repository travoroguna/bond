//
// Created by travor on 3/18/23.
//
#pragma once

#include "ast.h"

namespace bond {
    class BinaryOp;

    class Unary;

    class TrueLiteral;

    class FalseLiteral;

    class NumberLiteral;

    class StringLiteral;

    class NilLiteral;

    class Identifier;
    class List;
    class GetItem;
    class SetItem;
    class ExprStmnt;
    class NewVar;
    class Block;
    class Assign;
    class If;
    class While;
    class Call;
    class For;

    class FuncDef;

    class ClosureDef;

    class Return;

    class StructNode;

    class GetAttribute;

    class SetAttribute;

    class ImportDef;

    class Try;

    class Break;

    class Continue;

    class AsyncDef;

    class Await;

    class NodeVisitor {
    public:
        NodeVisitor();

        virtual ~NodeVisitor();

        virtual void visit_bin_op(BinaryOp *expr) = 0;

        virtual void visit_unary(Unary *expr) = 0;

        virtual void visit_true_lit(TrueLiteral *expr) = 0;

        virtual void visit_false_lit(FalseLiteral *expr) = 0;

        virtual void visit_num_lit(NumberLiteral *expr) = 0;
        virtual void visit_string_lit(StringLiteral *expr) = 0;
        virtual void visit_nil_lit(NilLiteral *expr) = 0;
        virtual void visit_identifier(Identifier *expr) = 0;
        virtual void visit_expr_stmnt(ExprStmnt *stmnt) = 0;
        virtual void visit_new_var(NewVar *stmnt) = 0;
        virtual void visit_assign(Assign *stmnt) = 0;
        virtual void visit_block(Block *stmnt) = 0;
        virtual void visit_list(List *expr) = 0;
        virtual void visit_get_item(GetItem *expr) = 0;
        virtual void visit_set_item(SetItem *expr) = 0;
        virtual void visit_if(If *stmnt) = 0;
        virtual void visit_while(While *stmnt) = 0;
        virtual void visit_call(Call *expr) = 0;
        virtual void visit_for(For *stmnt) = 0;

        virtual void visit_func_def(FuncDef *stmnt) = 0;

        virtual void visit_return(Return *stmnt) = 0;

        virtual void visit_closure_def(ClosureDef *stmnt) = 0;

        virtual void visit_struct(StructNode *stmnt) = 0;

        virtual void visit_get_attribute(GetAttribute *expr) = 0;

        virtual void visit_set_attribute(SetAttribute *expr) = 0;

        virtual void visit_import(ImportDef *stmnt) = 0;

        virtual void visit_try(Try *stmnt) = 0;

        virtual void visit_break(Break *stmnt) = 0;

        virtual void visit_continue(Continue *stmnt) = 0;

        virtual void visit_async_def(AsyncDef *stmnt) = 0;

        virtual void visit_await(Await *expr) = 0;
    };
}
