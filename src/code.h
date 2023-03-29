//
// Created by travor on 3/18/23.
//

#ifndef BOND_CODE_H
#define BOND_CODE_H

#include "nodevisitor.h"
#include "ast.h"
#include <cstdint>
#include <vector>
// #include "vm.h"
#include "object.h"
#include "parser.h"


namespace bond {

    class CodeGenerator : public NodeVisitor {
    public:

        CodeGenerator(Context *ctx, Symbols *symbols);

        GcPtr<Code> generate_code(const std::vector<std::shared_ptr<Node>> &nodes);

        void visit_bin_op(BinaryOp *expr) override;

        void visit_unary(Unary *expr) override;

        void visit_true_lit(TrueLiteral *expr) override;

        void visit_false_lit(FalseLiteral *expr) override;

        void visit_num_lit(NumberLiteral *expr) override;

        void visit_string_lit(StringLiteral *expr) override;

        void visit_nil_lit(NilLiteral *expr) override;

        void visit_print(Print *stmnt) override;
        void visit_expr_stmnt(ExprStmnt* stmnt) override;
        void visit_identifier(Identifier* expr) override;

        void visit_new_var(NewVar *stmnt) override;

        void visit_assign(Assign *stmnt) override;


    private:
        GcPtr<Code> m_code;
        Context *m_ctx;
        Symbols *m_symbols;
    };

} // bond

#endif //BOND_CODE_H
