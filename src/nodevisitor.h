//
// Created by travor on 3/18/23.
//

#ifndef BOND_NODEVISITOR_H
#define BOND_NODEVISITOR_H


namespace bond{
    class BinaryOp;
    class Unary;
    class TrueLiteral;
    class FalseLiteral;
    class NumberLiteral;
    class StringLiteral;
    class NilLiteral;

    class NodeVisitor{
    public:
        NodeVisitor();
        virtual void visit_bin_op(BinaryOp* expr) = 0;
        virtual void visit_unary(Unary* expr) = 0;
        virtual void visit_true_lit(TrueLiteral* expr) = 0;
        virtual void visit_false_lit(FalseLiteral* expr) = 0;
        virtual void visit_num_lit(NumberLiteral* expr) = 0;
        virtual void visit_string_lit(StringLiteral* expr) = 0;
        virtual void visit_nil_lit(NilLiteral* expr) = 0;
    };
}


#endif //BOND_NODEVISITOR_H
