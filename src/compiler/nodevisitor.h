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

    class ListLiteral;

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

    class StructuredAssign;

    class CallMethod;

    class ResultStatement;

    class DictLiteral;

    class InplaceOp;

    class InplaceOpItem;

    class InplaceOpAttribute;

    class NodeVisitor {
    public:
        NodeVisitor();

        virtual ~NodeVisitor();

        virtual void visit(BinaryOp *expr) = 0;

        virtual void visit(InplaceOpItem *expr) = 0;

        virtual void visit(InplaceOpAttribute *expr) = 0;

        virtual void visit(InplaceOp *expr) = 0;

        virtual void visit(Unary *expr) = 0;

        virtual void visit(TrueLiteral *expr) = 0;

        virtual void visit(FalseLiteral *expr) = 0;

        virtual void visit(NumberLiteral *expr) = 0;

        virtual void visit(StringLiteral *expr) = 0;

        virtual void visit(NilLiteral *expr) = 0;

        virtual void visit(Identifier *expr) = 0;

        virtual void visit(ExprStmnt *stmnt) = 0;

        virtual void visit(NewVar *stmnt) = 0;

        virtual void visit(Assign *stmnt) = 0;

        virtual void visit(Block *stmnt) = 0;

        virtual void visit(ListLiteral *expr) = 0;

        virtual void visit(GetItem *expr) = 0;

        virtual void visit(SetItem *expr) = 0;

        virtual void visit(If *stmnt) = 0;

        virtual void visit(While *stmnt) = 0;

        virtual void visit(Call *expr) = 0;

        virtual void visit(For *stmnt) = 0;

        virtual void visit(FuncDef *stmnt) = 0;

        virtual void visit(Return *stmnt) = 0;

        virtual void visit(ClosureDef *stmnt) = 0;

        virtual void visit(StructNode *stmnt) = 0;

        virtual void visit(GetAttribute *expr) = 0;

        virtual void visit(SetAttribute *expr) = 0;

        virtual void visit(ImportDef *stmnt) = 0;

        virtual void visit(Try *stmnt) = 0;

        virtual void visit(Break *stmnt) = 0;

        virtual void visit(Continue *stmnt) = 0;

        virtual void visit(AsyncDef *stmnt) = 0;

        virtual void visit(Await *expr) = 0;

        virtual void visit(StructuredAssign *expr) = 0;

        virtual void visit(CallMethod *expr) = 0;

        virtual void visit(ResultStatement *expr) = 0;

        virtual void visit(DictLiteral *expr) = 0;
    };

}
